#ifndef DOLPHIN_BASE_H
#define DOLPHIN_BASE_H

#define u8 unsigned char
#define u64 unsigned long long

class DolphinBase
{
public:
    static constexpr u64 ARAM_FAKESIZE = 0x02000000LL;
    static constexpr u64 ARAM_START = 0x7e000000LL;
    static constexpr u64 ARAM_END = 0x7f000000LL;
    static constexpr u64 MEM1_START = 0x80000000LL;
    static constexpr u64 MEM1_END = 0x81800000LL;
    static constexpr u64 MEM2_START = 0x90000000LL;
    static constexpr u64 MEM2_END = 0x94000000LL;

    static u64 m_emuRAMAddressStart;
    static u64 m_emuARAMAdressStart;
    static u64 m_MEM2AddressStart;
    static bool m_MEM2Present;
    static bool m_ARAMAccessible;

    // Returns false if dolphin is not emulating a game.
    static bool obtainEmuRAMInformations(HANDLE dolphinHandle);

    // Given the address of a value shown in DME, returns address in memory to read.
    static u64 getAddressOfDolphinMemoryToRead(u64 uiAddr);
};

#endif
