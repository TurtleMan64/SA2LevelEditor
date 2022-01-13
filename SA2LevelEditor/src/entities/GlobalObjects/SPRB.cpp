#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "sprb.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../dummy.h"
#include "../unknown.h"

#include <list>



std::list<TexturedModel*> SPRB::models;
CollisionModel* SPRB::cmBase;

SPRB::SPRB()
{

}

void SPRB::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    despawnGuides();
}

SPRB::SPRB(char data[32], bool useDefaultValues)
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
    char* v1 = (char*)&var1;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];
    controlLockTime = (int)var1;

    char* v2 = (char*)&power;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
    power += 5.0f;

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
        controlLockTime = 30;
        power = 8.0f;
        var3 = 0.0f;
    }

    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
    visible = true;
    baseColour.set(1, 1, 1);
    updateTransformationMatrixYXZ();

    collideModelOriginal = SPRB::cmBase;
    collideModelTransformed = SPRB::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);
    updateCollisionModelYXZ();
}

bool SPRB::isSA2Object()
{
    return true;
}

void SPRB::step()
{
    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
        if (guides.size() > 0)
        {
            despawnGuides();
            Global::redrawWindow = true;
        }
    }
}

std::list<TexturedModel*>* SPRB::getModels()
{
    return &SPRB::models;
}

void SPRB::loadStaticModels()
{
    if (SPRB::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading SPRB static models...\n");
    #endif

    loadModel(&SPRB::models, "res/Models/GlobalObjects/Spring/", "SpringB");

    if (SPRB::cmBase == nullptr)
    {
        SPRB::cmBase = loadCollisionModel("res/Models/GlobalObjects/Spring/", "SpringB");
    }
}

void SPRB::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting SPRB static models...\n");
    #endif

    Entity::deleteModels(&SPRB::models);
    Entity::deleteCollisionModel(&SPRB::cmBase);
}

void SPRB::updateValue(int btnIndex)
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
                    return;
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
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
            int newRotX = std::stoi(text);
            rotationX = newRotX;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[5], std::to_string(rotationX).c_str());
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[6], std::to_string(rotationY).c_str());
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[7], std::to_string(rotationZ).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            int newVar1 = std::stoi(text);
            controlLockTime = newVar1;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[8], std::to_string(controlLockTime).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newVar2 = std::stof(text);
            power = newVar2;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[9], std::to_string(power).c_str());
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
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }

    spawnGuides();
}

void SPRB::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Time Lock");
    SetWindowTextA(Global::windowLabels[ 9], "Power");
    SetWindowTextA(Global::windowLabels[10], "Unknown");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "SPRB");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(controlLockTime).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(power).c_str());
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
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "When the player touches this spring, the player's Y rotation is set to the spring's Y rotation.");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "Time (in frames) that controls are locked.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Speed that the player goes once the touch this spring.");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    spawnGuides();
}

void SPRB::despawnGuides()
{
    for (Dummy* guide : guides)
    {
        Global::deleteEntity(guide);
    }
    guides.clear();
}

void SPRB::spawnGuides()
{
    despawnGuides();
    
    Vector3f pos(&position);
    Vector3f dir(0, 10, 0);
    Vector3f xAxis(1, 0, 0);
    Vector3f zAxis(0, 0, 1);
    dir = Maths::rotatePoint(&dir, &xAxis, Maths::bamsToRad(rotationX));
    dir = Maths::rotatePoint(&dir, &zAxis, Maths::bamsToRad(rotationZ));
    
    #ifndef SAB_GUIDES
    for (int i = 0; i < 30; i++)
    {
        Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
        guide->setPosition(&pos);
        guide->visible = true;
        guide->updateTransformationMatrixYXZ();
        Global::addEntity(guide);
        guides.push_back(guide);
    
        pos = pos + dir;
    }
    #else
    Vector3f vel = dir;
    vel.setLength(power*60.0f);
    float timeLeft = ((float)controlLockTime)/60.0f;
    const float dt = 0.0166666666666f;
    while (timeLeft > 0)
    {
        Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
        guide->setPosition(&pos);
        guide->visible = true;
        guide->updateTransformationMatrixYXZ();
        Global::addEntity(guide);
        guides.push_back(guide);
    
        pos = pos + vel.scaleCopy(dt);

        const float airNeutralFriction = 1.25f;
        float storedVelY = vel.y;
        vel.y = 0;
        vel = Maths::applyDrag(&vel, -airNeutralFriction, dt);
        vel.y = storedVelY;

        const float gravityForce = 280.0f;
        const float gravityTerminal = -650.0f;
        const float gravityApproach = 0.45f;
        vel.y = Maths::approach(vel.y, gravityTerminal, gravityApproach, dt);

        timeLeft -= dt;
    }
    #endif
}

void SPRB::fillData(char data[32])
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

    float var1 = (float)controlLockTime;
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = (power - 5.0f);
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

std::string SPRB::toSabString()
{
    Vector3f dir(0, 1, 0);
    Vector3f xAxis(1, 0, 0);
    Vector3f zAxis(0, 0, 1);
    dir = Maths::rotatePoint(&dir, &xAxis, Maths::bamsToRad(rotationX));
    dir = Maths::rotatePoint(&dir, &zAxis, Maths::bamsToRad(rotationZ));
    
    int cam = (int)std::round(var3);
    if (cam != 1)
    {
        cam = 0;
    }

    return "12 " + 
        std::to_string(position.x) + " " + 
        std::to_string(position.y) + " " + 
        std::to_string(position.z) + " " + 
        std::to_string(dir.x) + " " + 
        std::to_string(dir.y) + " " + 
        std::to_string(dir.z) + " " + 
        std::to_string(power*60) + " " + 
        std::to_string(((float)controlLockTime)/60) + " " + 
        std::to_string(cam);
}
