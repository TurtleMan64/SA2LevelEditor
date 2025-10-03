#include <Windows.h>
#include <tlhelp32.h>
#include <Psapi.h>

#include <cstdio>
#include <cstring>
#include <string>

#include "dolphinbase.h"

u64  DolphinBase::m_emuRAMAddressStart = 0;
u64  DolphinBase::m_emuARAMAdressStart = 0;
u64  DolphinBase::m_MEM2AddressStart = 0;
bool DolphinBase::m_MEM2Present = false;
bool DolphinBase::m_ARAMAccessible = false;

// magic copied from https://github.com/aldelaro5/Dolphin-memory-engine
bool DolphinBase::obtainEmuRAMInformations(HANDLE dolphinHandle)
{
    MEMORY_BASIC_INFORMATION info;
    bool MEM1Found = false;
    for (unsigned char* p = nullptr; VirtualQueryEx(dolphinHandle, p, &info, sizeof(info)) == sizeof(info); p += info.RegionSize)
    {
        // Check region size so that we know it's MEM2
        if (!m_MEM2Present && info.RegionSize == 0x4000000)
        {
            u64 regionBaseAddress = 0;
            std::memcpy(&regionBaseAddress, &(info.BaseAddress), sizeof(info.BaseAddress));
            if (MEM1Found && regionBaseAddress > m_emuRAMAddressStart + 0x10000000)
            {
                // In some cases MEM2 could actually be before MEM1. Once we find MEM1, ignore regions of
                // this size that are too far away. There apparently are other non-MEM2 regions of size
                break;
            }
            // View the comment for MEM1.
            PSAPI_WORKING_SET_EX_INFORMATION wsInfo;
            wsInfo.VirtualAddress = info.BaseAddress;
            if (QueryWorkingSetEx(dolphinHandle, &wsInfo, sizeof(PSAPI_WORKING_SET_EX_INFORMATION)))
            {
                if (wsInfo.VirtualAttributes.Valid)
                {
                    std::memcpy(&m_MEM2AddressStart, &(regionBaseAddress), sizeof(regionBaseAddress));
                    m_MEM2Present = true;
                }
            }
        }
        else if (info.RegionSize == 0x2000000 && info.Type == MEM_MAPPED)
        {
            // Here, it's likely the right page, but it can happen that multiple pages with these criteria
            // exists and have nothing to do with the emulated memory. Only the right page has valid
            // working set information so an additional check is required that it is backed by physical
            // memory.
            PSAPI_WORKING_SET_EX_INFORMATION wsInfo;
            wsInfo.VirtualAddress = info.BaseAddress;
            if (QueryWorkingSetEx(dolphinHandle, &wsInfo, sizeof(PSAPI_WORKING_SET_EX_INFORMATION)))
            {
                if (wsInfo.VirtualAttributes.Valid)
                {
                    if (!MEM1Found)
                    {
                        std::memcpy(&m_emuRAMAddressStart, &(info.BaseAddress), sizeof(info.BaseAddress));
                        MEM1Found = true;
                    }
                    else
                    {
                        u64 aramCandidate = 0;
                        std::memcpy(&aramCandidate, &(info.BaseAddress), sizeof(info.BaseAddress));
                        if (aramCandidate == m_emuRAMAddressStart + 0x2000000)
                        {
                            m_emuARAMAdressStart = aramCandidate;
                            m_ARAMAccessible = true;
                        }
                    }
                }
            }
        }
    }

    if (m_MEM2Present)
    {
        m_emuARAMAdressStart = 0;
        m_ARAMAccessible = false;
    }

    if (m_emuRAMAddressStart == 0)
    {
        // Here, Dolphin is running, but the emulation hasn't started
        return false;
    }

    return true;
}

//uiAddr is the address shown in the UI of dolphin memory engine
u64 DolphinBase::getAddressOfDolphinMemoryToRead(u64 uiAddr)
{
    u64 addr = uiAddr;

    // ARAM address
    if (addr >= ARAM_START && addr < ARAM_END)
    {
        addr -= ARAM_START;
    }
    // MEM1 address
    else if (addr >= MEM1_START && addr < MEM1_END)
    {
        addr -= MEM1_START;
        if (m_ARAMAccessible)
        {
            addr += ARAM_FAKESIZE;
        }
    }
    // MEM2 address
    else if (addr >= MEM2_START && addr < MEM2_END)
    {
        addr -= MEM2_START;
        addr += (MEM2_START - MEM1_START);
    }

    u64 RAMAddress = 0;
    if (m_ARAMAccessible)
    {
        if (addr >= ARAM_FAKESIZE)
        {
            RAMAddress = m_emuRAMAddressStart + addr - ARAM_FAKESIZE;
        }
        else
        {
            RAMAddress = m_emuARAMAdressStart + addr;
        }
    }
    else if (addr >= (MEM2_START - MEM1_START))
    {
        RAMAddress = m_MEM2AddressStart + addr - (MEM2_START - MEM1_START);
    }
    else
    {
        RAMAddress = m_emuRAMAddressStart + addr;
    }

    return RAMAddress;
}
