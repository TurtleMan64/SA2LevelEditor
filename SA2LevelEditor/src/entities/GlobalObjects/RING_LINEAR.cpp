#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ringlinear.h"
#include "ring.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../dummy.h"

#include <list>


RING_LINEAR::RING_LINEAR()
{

}

RING_LINEAR::RING_LINEAR(char data[32])
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    rotationX = data[3] + (data[2] << 8);
    rotationY = data[5] + (data[4] << 8);
    rotationZ = data[7] + (data[6] << 8);

    char* x = (char*)&position.x;
    x[3] = data[8];
    x[2] = data[9];
    x[1] = data[10];
    x[0] = data[11];

    char* y = (char*)&position.y;
    y[3] = data[12];
    y[2] = data[13];
    y[1] = data[14];
    y[0] = data[15];

    char* z = (char*)&position.z;
    z[3] = data[16];
    z[2] = data[17];
    z[1] = data[18];
    z[0] = data[19];

    float var1;
    char* d = (char*)&var1;
    d[3] = data[20];
    d[2] = data[21];
    d[1] = data[22];
    d[0] = data[23];

    float var3;
    char* n = (char*)&var3;
    n[3] = data[28];
    n[2] = data[29];
    n[1] = data[30];
    n[0] = data[31];

    numRings = (int)var3;
    ringDelta = var1 + 10.0f;

    spawnChildren();

	visible = false;
}

void RING_LINEAR::spawnChildren()
{
    Vector3f ringDirection(0, 0, 1);

    Vector3f xAxis(1, 0, 0);
    Vector3f yAxis(0, 1, 0);
    ringDirection = Maths::rotatePoint(&ringDirection, &xAxis, Maths::toRadians(rotationX));
    ringDirection = Maths::rotatePoint(&ringDirection, &yAxis, Maths::toRadians(rotationY));

    ringDirection.setLength(ringDelta);
    ringDirection.neg();

    for (int i = 0; i < numRings; i++)
    {
        Vector3f currOff = ringDirection.scaleCopy((float)i);
        Vector3f ringPos = position + currOff;

        Dummy* ring = new Dummy(&RING::models); INCR_NEW("Entity");
        ring->setPosition(&ringPos);
        ring->visible = true;
        ring->updateTransformationMatrix();
        Global::addEntity(ring);

        CollisionModel* cm = RING::cmBase->duplicateMe();
        cm->parent = this;
        RING::cmBase->transformModel(cm, &ring->position, 0, 0, 0, 1, 1, 1);
        CollisionChecker::addCollideModel(cm);

        rings.push_back(ring);
        cms.push_back(cm);
    }
}

void RING_LINEAR::step()
{
    if (Global::selectedSA2Object == this)
    {
        for (Dummy* ring : rings)
        {
            ring->setBaseColour(1.75f, 1.75f, 1.75f);
        }
    }
    else
    {
        for (Dummy* ring : rings)
        {
            ring->setBaseColour(1.0f, 1.0f, 1.0f);
        }
    }
}

std::list<TexturedModel*>* RING_LINEAR::getModels()
{
	return nullptr; //our children are visible, not us
}

void RING_LINEAR::loadStaticModels()
{
	//we just use RING models
}

void RING_LINEAR::deleteStaticModels()
{
	//we just use RING models
}

void RING_LINEAR::updateValue(int btnIndex)
{
    char buf[128];
    GetWindowTextA(Global::windowValues[btnIndex], buf, 128);
    std::string text = buf;

    bool remakeRings = false;

    switch (btnIndex)
    {
    case 0:
    {
        //we are going to change into a new object. deal with this later.
        break;
    }

    case 2:
    {
        try
        {
            float newX = std::stof(text);
            position.x = newX;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[2], std::to_string(position.x).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 3:
    {
        try
        {
            float newY = std::stof(text);
            position.y = newY;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[3], std::to_string(position.y).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 4:
    {
        try
        {
            float newZ = std::stof(text);
            position.z = newZ;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[4], std::to_string(position.z).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 5:
    {
        try
        {
            int newRotX = std::stoi(text);
            rotationX = newRotX;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[5], std::to_string(rotationX).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 6:
    {
        try
        {
            int newRotY = std::stoi(text);
            rotationY = newRotY;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[6], std::to_string(rotationY).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 7:
    {
        try
        {
            int newRotZ = std::stoi(text);
            rotationZ = newRotZ;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[7], std::to_string(rotationZ).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            float newRingDelta = std::stof(text);
            ringDelta = newRingDelta;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[8], std::to_string(ringDelta).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            float newRingCount = std::stof(text);
            numRings = (int)newRingCount;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[10], std::to_string(numRings).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }
    
    default: break;
    }

    if (remakeRings)
    {
        for (Dummy* ring : rings)
        {
            Global::deleteEntity(ring);
        }

        for (CollisionModel* cm : cms)
        {
            CollisionChecker::deleteCollideModel(cm);
        }

        rings.clear();
        cms.clear();

        spawnChildren();
    }
}

void RING_LINEAR::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Ring Delta");
    SetWindowTextA(Global::windowLabels[ 9], "");
    SetWindowTextA(Global::windowLabels[10], "Ring Count");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "RING_LINEAR");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(ringDelta).c_str());
    SetWindowTextA(Global::windowValues[ 9], "");
    SetWindowTextA(Global::windowValues[10], std::to_string(numRings).c_str());

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "Distance between each individual ring.");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "Total number of rings in the line.");
}

void RING_LINEAR::fillData(char data[32])
{
    data[0] = 0x00; //S has 0 as the (second? (according to wiki)) byte, U has 0x80. we will export every object to S file
    data[1] = (char)ID;

    data[2] = (char)((rotationX >> 8) & 0xFF);
    data[3] = (char)((rotationX >> 0) & 0xFF);
    data[4] = (char)((rotationY >> 8) & 0xFF);
    data[5] = (char)((rotationY >> 0) & 0xFF);
    data[6] = (char)((rotationZ >> 8) & 0xFF);
    data[7] = (char)((rotationZ >> 0) & 0xFF);

    char* ptr = (char*)(&position.x);
    data[ 8] = (char)(*(ptr + 3));
    data[ 9] = (char)(*(ptr + 2));
    data[10] = (char)(*(ptr + 1));
    data[11] = (char)(*(ptr + 0));

    ptr = (char*)(&position.y);
    data[12] = (char)(*(ptr + 3));
    data[13] = (char)(*(ptr + 2));
    data[14] = (char)(*(ptr + 1));
    data[15] = (char)(*(ptr + 0));

    ptr = (char*)(&position.z);
    data[16] = (char)(*(ptr + 3));
    data[17] = (char)(*(ptr + 2));
    data[18] = (char)(*(ptr + 1));
    data[19] = (char)(*(ptr + 0));

    float var1 = (ringDelta - 10.0f);
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    data[24] = 0;
    data[25] = 0;
    data[26] = 0;
    data[27] = 0;

    float var3 = (float)numRings;
    ptr = (char*)(&var3);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}

bool RING_LINEAR::isSA2Object()
{
    return true;
}