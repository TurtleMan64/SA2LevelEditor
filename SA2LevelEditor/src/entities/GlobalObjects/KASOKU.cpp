#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "kasoku.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>



std::list<TexturedModel*> KASOKU::models;
CollisionModel* KASOKU::cmBase;

KASOKU::KASOKU()
{

}

void KASOKU::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
}

KASOKU::KASOKU(char data[32], bool useDefaultValues)
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

    char* v1 = (char*)&power;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];

    float var2;
    char* v2 = (char*)&var2;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
    cooldown = (int)var2;

    char* v3 = (char*)&var3;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];

    if (useDefaultValues)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        power = 11.0f;
        cooldown = 30;
        var3 = 0.0f;
    }

    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
    visible = true;
    baseColour.set(1, 1, 1);

    collideModelOriginal = KASOKU::cmBase;
    collideModelTransformed = KASOKU::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);
    
    refreshMatrixAndCol();
}

bool KASOKU::isSA2Object()
{
    return true;
}

void KASOKU::step()
{
    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
    }
}

std::list<TexturedModel*>* KASOKU::getModels()
{
    return &KASOKU::models;
}

void KASOKU::loadStaticModels()
{
    if (KASOKU::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading KASOKU static models...\n");
    #endif

    #ifndef SAB_LVL
    loadModel(&KASOKU::models, "res/Models/GlobalObjects/Dashpad/", "Dashpad");
    if (KASOKU::cmBase == nullptr)
    {
        KASOKU::cmBase = loadCollisionModel("res/Models/GlobalObjects/Dashpad/", "Dashpad");
    }
    #else
    loadModel(&KASOKU::models, "res/Models/GlobalObjects/Dashpad/", "DashpadSab");
    if (KASOKU::cmBase == nullptr)
    {
        KASOKU::cmBase = loadCollisionModel("res/Models/GlobalObjects/Dashpad/", "DashpadSab");
    }
    #endif
}

void KASOKU::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting KASOKU static models...\n");
    #endif

    Entity::deleteModels(&KASOKU::models);
    Entity::deleteCollisionModel(&KASOKU::cmBase);
}

void KASOKU::updateValue(int btnIndex)
{
    char buf[128];
    GetWindowTextA(Global::windowValues[btnIndex], buf, 128);
    std::string text = buf;

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
            return;
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
            SetWindowTextA(Global::windowValues[5], std::to_string(Maths::bamsToDeg(rotationX)).c_str());
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
            SetWindowTextA(Global::windowValues[6], std::to_string(Maths::bamsToDeg(rotationY)).c_str());
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
            SetWindowTextA(Global::windowValues[7], std::to_string(Maths::bamsToDeg(rotationZ)).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            float newVar1 = std::stof(text);
            power = newVar1;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[8], std::to_string(power).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            int newVar2 = std::stoi(text);
            cooldown = newVar2;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[9], std::to_string(cooldown).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            float newVar3 = std::stof(text);
            var3 = newVar3;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }

    refreshMatrixAndCol();
}

void KASOKU::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Power");
    SetWindowTextA(Global::windowLabels[ 9], "Cooldown");
    SetWindowTextA(Global::windowLabels[10], "Unused");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "KASOKU");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(Maths::bamsToDeg(rotationX)).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(Maths::bamsToDeg(rotationY)).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(Maths::bamsToDeg(rotationZ)).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(power).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(cooldown).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());

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
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "Speed pad");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "(in degrees)");
    SetWindowTextA(Global::windowDescriptions[ 6], "(in degrees)");
    SetWindowTextA(Global::windowDescriptions[ 7], "(in degrees)");
    SetWindowTextA(Global::windowDescriptions[ 8], "Speed that the player goes. If <= 0, 14 is used instead.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Time (in frames) of cooldown after each use. The player's control stick is also locked during this time. If <= 0, 60 is used instead.");
    SetWindowTextA(Global::windowDescriptions[10], "");

    #ifdef SAB_GUIDES
    Vector3f vel(1, 0, 0);
    Vector3f up(0, 1, 0);
    vel.setLength(power*60.0f);
    float timeLeft = ((float)cooldown)/60.0f;
    const float dt = 0.0166666666666f;
    const float groundNeutralFriction = 1.5f;
    while (timeLeft > 0)
    {
        if (vel.lengthSquared() > 35.0f*35.0f) //normal neutral stick friction
        {
            Vector3f fr(0, -0.95f, 0); //0.75
            fr = Maths::projectOntoPlane(&fr, &up);
            float frictionPower = groundNeutralFriction*(1 - fr.length());
            vel = Maths::applyDrag(&vel, -frictionPower, dt); //Slow vel down due to friction
        }
        else //when close to no speed, increase friction
        {
            Vector3f fr(0, -0.95f, 0); //0.75
            fr = Maths::projectOntoPlane(&fr, &up);
            float frictionPower = (groundNeutralFriction*4)*(1 - fr.length()); //multiply by 4, arbitrary
            vel = Maths::applyDrag(&vel, -frictionPower, dt); //Slow vel down due to friction
        }

        timeLeft -= dt;
    }
    std::string endSpeed = std::to_string(vel.length());
    SetWindowTextA(Global::windowDescriptions[10], endSpeed.c_str());
    #endif

    refreshMatrixAndCol();
}

void KASOKU::fillData(char data[32])
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

    ptr = (char*)(&power);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = (float)cooldown;
    ptr = (char*)(&var2);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

    ptr = (char*)(&var3);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}

void KASOKU::refreshMatrixAndCol()
{
    #ifndef SAB_LVL
    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    #else
    updateTransformationMatrixXZY();
    updateCollisionModelXZY();
    #endif
}

std::string KASOKU::toSabString()
{
    return "8 " + 
        std::to_string(position.x)     + " " + 
        std::to_string(position.y)     + " " + 
        std::to_string(position.z)     + " " +
        std::to_string(power*60)       + " " +
        std::to_string(cooldown/60.0f) + " " +
        std::to_string(Maths::bamsToDeg(rotationX)) + " " +
        std::to_string(Maths::bamsToDeg(rotationY)) + " " +
        std::to_string(Maths::bamsToDeg(rotationZ));
}
