#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ringlinear.h"
#include "ring.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../dummy.h"
#include "../../toolbox/hex.h"

#include <list>


RING_LINEAR::RING_LINEAR()
{

}

void RING_LINEAR::cleanUp()
{
    despawnChildren();
}

RING_LINEAR::RING_LINEAR(char data[32], bool useDefaultValues)
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    signed short rX;
    signed short rY;
    signed short rZ;

    char* ptr = (char*)(&rX);
    memset(ptr, data[3], 1);
    memset(ptr+1, data[2], 1);

    ptr = (char*)(&rY);
    memset(ptr, data[5], 1);
    memset(ptr+1, data[4], 1);

    ptr = (char*)(&rZ);
    memset(ptr, data[7], 1);
    memset(ptr+1, data[6], 1);

    rotationX = (int)rX;
    rotationY = (int)rY;
    rotationZ = (int)rZ;

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

    float var2;
    char* u = (char*)&curveHeight;
    u[3] = data[24];
    u[2] = data[25];
    u[1] = data[26];
    u[0] = data[27];

    float var3;
    char* n = (char*)&var3;
    n[3] = data[28];
    n[2] = data[29];
    n[1] = data[30];
    n[0] = data[31];

    numRings = (int)var3;
    ringDelta = var1 + 10.0f;

    #ifdef SAB_MODE
    numRings = std::max(2, numRings);
    numRings = std::min(50, numRings);
    #else
    numRings = std::max(1, numRings);
    numRings = std::min(8, numRings);
    #endif

    if (useDefaultValues)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        numRings = 5;
        var2 = 0.0f;
        ringDelta = 20.0f;
    }

    spawnChildren();

    visible = false;
}

void RING_LINEAR::despawnChildren()
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
}

void RING_LINEAR::spawnChildren()
{
    //no rotation goes in the -Z direction
    Vector3f ringDirection(0, 0, -1);

    //for use with the var2 sine curve offset
    Vector3f relativeUp(0, 1, 0);
    float angleDelta = 0.0f;
    if (numRings >= 2)
    {
        angleDelta = 1.0f/(numRings-1);
    }

    Vector3f xAxis(1, 0, 0);
    Vector3f yAxis(0, 1, 0);
    Vector3f zAxis(0, 0, 1);

    ringDirection = Maths::rotatePoint(&ringDirection, &xAxis, Maths::bamsToRad(rotationX));
    ringDirection = Maths::rotatePoint(&ringDirection, &yAxis, Maths::bamsToRad(rotationY));
    ringDirection = Maths::rotatePoint(&ringDirection, &zAxis, Maths::bamsToRad(rotationZ));
    ringDirection.setLength(ringDelta);

    relativeUp = Maths::rotatePoint(&relativeUp, &xAxis, Maths::bamsToRad(rotationX));
    relativeUp = Maths::rotatePoint(&relativeUp, &yAxis, Maths::bamsToRad(rotationY));
    relativeUp = Maths::rotatePoint(&relativeUp, &zAxis, Maths::bamsToRad(rotationZ));

    for (int i = 0; i < numRings; i++)
    {
        Vector3f currOffsetLine = ringDirection.scaleCopy((float)i);
        Vector3f currOffsetCurve = relativeUp.scaleCopy(curveHeight*sinf((i*angleDelta)*(Maths::PI/2)));
        Vector3f ringPos = position + currOffsetLine + currOffsetCurve;

        Dummy* ring = new Dummy(&RING::models); INCR_NEW("Entity");
        ring->setPosition(&ringPos);
        ring->visible = true;
        ring->updateTransformationMatrixYXZ();
        Global::addEntity(ring);

        CollisionModel* cm = RING::cmBase->duplicateMe();
        cm->parent = this;
        RING::cmBase->transformModelYXZ(cm, &ring->position, 0, 0, 0, 1, 1, 1);
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
        try
        {
            //we are going to change into a new object.
            int newid = std::stoi(text);

            if (newid != ID)
            {
                char data[32] = {0};
                data[ 1] = (char)newid;

                data[ 8] = *(((char*)&position.x)+3);
                data[ 9] = *(((char*)&position.x)+2);
                data[10] = *(((char*)&position.x)+1);
                data[11] = *(((char*)&position.x)+0);
                data[12] = *(((char*)&position.y)+3);
                data[13] = *(((char*)&position.y)+2);
                data[14] = *(((char*)&position.y)+1);
                data[15] = *(((char*)&position.y)+0);
                data[16] = *(((char*)&position.z)+3);
                data[17] = *(((char*)&position.z)+2);
                data[18] = *(((char*)&position.z)+1);
                data[19] = *(((char*)&position.z)+0);

                SA2Object* newObject = LevelLoader::newSA2Object(Global::levelID, newid, data, true);
                if (newObject != nullptr)
                {
                    newObject->lvlLineNum = lvlLineNum;
                    Global::addEntity(newObject);
                    Global::selectedSA2Object = newObject;
                    newObject->updateEditorWindows();
                    Global::redrawWindow = true;
                    cleanUp();
                    Global::deleteEntity(this);
                }
            }
            break;
        }
        catch (...) { break; }
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
            float newRotX = std::stof(text);
            rotationX = Maths::degToBams(newRotX);
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[5], std::to_string(Maths::bamsToDeg((signed short)rotationX)).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 6:
    {
        try
        {
            float newRotY = std::stof(text);
            rotationY = Maths::degToBams(newRotY);
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[6], std::to_string(Maths::bamsToDeg((signed short)rotationY)).c_str());
            remakeRings = true;
            break;
        }
        catch (...) { break; }
    }

    case 7:
    {
        try
        {
            float newRotZ = std::stof(text);
            rotationZ = Maths::degToBams(newRotZ);
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[7], std::to_string(Maths::bamsToDeg((signed short)rotationZ)).c_str());
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

    case 9:
    {
        try
        {
            float newCurveHeight = std::stof(text);
            curveHeight = newCurveHeight;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[9], std::to_string(curveHeight).c_str());
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
        despawnChildren();
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
    SetWindowTextA(Global::windowLabels[ 9], "Curve Height");
    SetWindowTextA(Global::windowLabels[10], "Ring Count");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "RING_LINEAR");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(Maths::bamsToDeg((signed short)rotationX)).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(Maths::bamsToDeg((signed short)rotationY)).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(Maths::bamsToDeg((signed short)rotationZ)).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(ringDelta).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(curveHeight).c_str());
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
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 0, 0);
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
    SetWindowTextA(Global::windowDescriptions[ 9], "Height of the curve that the rings follow.");
    SetWindowTextA(Global::windowDescriptions[10], "Total number of rings in the line.");

    despawnChildren();
    spawnChildren();
}

void RING_LINEAR::fillData(char data[32])
{
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

    ptr = (char*)(&curveHeight);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

    float var3 = (float)numRings;
    ptr = (char*)(&var3);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}

std::string RING_LINEAR::toSabString()
{
    int nRings = (int)rings.size();
    Vector3f pos1 = rings[0]->position;
    Vector3f pos2 = rings[nRings-1]->position;
    Vector3f diff = pos2-pos1;
    diff.normalize();
    diff.scale((nRings-1)*ringDelta);
    pos2 = pos1+diff;

    return "98 " + 
        std::to_string(pos1.x) + " " +
        std::to_string(pos1.y) + " " +
        std::to_string(pos1.z) + " " +
        std::to_string(pos2.x) + " " +
        std::to_string(pos2.y) + " " +
        std::to_string(pos2.z) + " " +
        std::to_string(nRings);
}

bool RING_LINEAR::isSA2Object()
{
    return true;
}
