#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../../entity.h"
#include "../../../toolbox/vector.h"
#include "dashring.h"
#include "../../../models/texturedmodel.h"
#include "../../../loading/objLoader.h"
#include "../../../loading/levelloader.h"
#include "../../../main/main.h"
#include "../../../collision/collisionmodel.h"
#include "../../../collision/collisionchecker.h"
#include "../../../toolbox/maths.h"
#include "../../dummy.h"
#include "../../unknown.h"

#include <list>



std::list<TexturedModel*> DASHRING::modelsDash;
std::list<TexturedModel*> DASHRING::modelsRainbow;
CollisionModel* DASHRING::cmBaseDash;
CollisionModel* DASHRING::cmBaseRainbow;

DASHRING::DASHRING()
{

}

void DASHRING::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    despawnGuides();
}

DASHRING::DASHRING(char data[32], bool useDefaultValues)
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
    power = var1;

    float var2;
    char* v2 = (char*)&var2;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
    controlLockTime = (int)var2;

    changeCamera = data[28];
    rainbow = data[29];

    if (useDefaultValues)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        controlLockTime = 15;
        power = 11.6666666667f;
        changeCamera = 0;
        rainbow = 0;
    }

    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
    visible = true;
    baseColour.set(1, 1, 1);
    updateTransformationMatrixZY();

    collideModelOriginal = DASHRING::cmBaseDash;
    collideModelTransformed = DASHRING::cmBaseDash->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);
    updateCollisionModelZY();
}

bool DASHRING::isSA2Object()
{
    return true;
}

void DASHRING::step()
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

std::list<TexturedModel*>* DASHRING::getModels()
{
    return &DASHRING::modelsDash;
}

void DASHRING::loadStaticModels()
{
    if (DASHRING::modelsDash.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading DASHRING static models...\n");
    #endif

    loadModel(&DASHRING::modelsDash,    "res/Models/GlobalObjects/Heroes/DashRing/", "DashRing");
    loadModel(&DASHRING::modelsRainbow, "res/Models/GlobalObjects/Heroes/RainbowRing/", "RainbowRing");

    if (DASHRING::cmBaseDash == nullptr)
    {
        DASHRING::cmBaseDash = loadCollisionModel("res/Models/GlobalObjects/Heroes/DashRing/", "DashRing");
    }

    if (DASHRING::cmBaseRainbow == nullptr)
    {
        DASHRING::cmBaseRainbow = loadCollisionModel("res/Models/GlobalObjects/Heroes/RainbowRing/", "RainbowRing");
    }
}

void DASHRING::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting DASHRING static models...\n");
    #endif

    Entity::deleteModels(&DASHRING::modelsDash);
    Entity::deleteModels(&DASHRING::modelsRainbow);
    Entity::deleteCollisionModel(&DASHRING::cmBaseDash);
    Entity::deleteCollisionModel(&DASHRING::cmBaseRainbow);
}

void DASHRING::updateValue(int btnIndex)
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
            SetWindowTextA(Global::windowValues[7], std::to_string(rotationZ).c_str());
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
            controlLockTime = newVar2;
            SetWindowTextA(Global::windowValues[9], std::to_string(controlLockTime).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            float newVar3 = std::stof(text);
            rainbow = (char)newVar3;
            SetWindowTextA(Global::windowValues[10], std::to_string(rainbow).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }

    updateTransformationMatrixZY();
    updateCollisionModelZY();
    Global::redrawWindow = true;
    spawnGuides();
}

void DASHRING::updateEditorWindows()
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
    SetWindowTextA(Global::windowLabels[ 9], "Time Lock");
    SetWindowTextA(Global::windowLabels[10], "Rainbow");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "DASHRING");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(power).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(controlLockTime).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(rainbow).c_str());

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
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateTransformationMatrixZY();
    updateCollisionModelZY();
    spawnGuides();
}

void DASHRING::despawnGuides()
{
    for (Dummy* guide : guides)
    {
        Global::deleteEntity(guide);
    }
    guides.clear();
}

void DASHRING::spawnGuides()
{
    despawnGuides();
    
    Vector3f pos(&position);
    Vector3f dir(1, 0, 0);
    Vector3f yAxis(0, 1, 0);
    Vector3f zAxis(0, 0, 1);
    dir = Maths::rotatePoint(&dir, &zAxis, Maths::bamsToRad(rotationZ));
    dir = Maths::rotatePoint(&dir, &yAxis, Maths::bamsToRad(rotationY));
    
    #ifndef SAB_GUIDES
    //for (int i = 0; i < 30; i++)
    //{
    //    Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
    //    guide->setPosition(&pos);
    //    guide->visible = true;
    //    guide->updateTransformationMatrixYXZ();
    //    Global::addEntity(guide);
    //    guides.push_back(guide);
    //
    //    pos = pos + dir;
    //}
    #else

    float dts[] = {1/60.0f, 1/720.0f};
    for (int i = 0; i < 2; i++)
    {
        float dt = dts[i];
        Vector3f vel = dir;
        vel.setLength(power*60.0f);
        pos = position;
        float timeLeft = ((float)controlLockTime)/60.0f;
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

        if (((float)controlLockTime)/60.0f < 10.0f)
        {
            timeLeft = 10.0f - ((float)controlLockTime)/60.0f;
        }
        while (timeLeft > 0)
        {
            Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
            guide->setPosition(&pos);
            guide->visible = true;
            guide->updateTransformationMatrixYXZ();
            guide->baseColour.set(0, 0, 0);
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
    }

    #endif
}

void DASHRING::fillData(char data[32])
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

    float var1 = (float)power;
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = (float)(controlLockTime);
    ptr = (char*)(&var2);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

    data[28] = changeCamera;
    data[29] = rainbow;
    data[30] = 0;
    data[31] = 0;
}

std::string DASHRING::toSabString()
{
    Vector3f dir(1, 0, 0);
    Vector3f yAxis(0, 1, 0);
    Vector3f zAxis(0, 0, 1);
    dir = Maths::rotatePoint(&dir, &zAxis, Maths::bamsToRad(rotationZ));
    dir = Maths::rotatePoint(&dir, &yAxis, Maths::bamsToRad(rotationY));

    return "118 " + 
        std::to_string(position.x) + " " + 
        std::to_string(position.y) + " " + 
        std::to_string(position.z) + " " + 
        std::to_string(dir.x) + " " + 
        std::to_string(dir.y) + " " + 
        std::to_string(dir.z) + " " + 
        std::to_string(power*60) + " " + 
        std::to_string(((float)controlLockTime)/60) + " " + 
        std::to_string(changeCamera) + " " +
        std::to_string(rainbow);
}
