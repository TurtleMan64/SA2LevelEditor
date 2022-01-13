#include <Windows.h>
#include <glad/glad.h>
#include <list>

#include "entity.h"
#include "../toolbox/vector.h"
#include "unknown.h"
#include "../models/texturedmodel.h"
#include "../loading/objLoader.h"
#include "../loading/levelloader.h"
#include "../main/main.h"
#include "../collision/collisionchecker.h"
#include "../collision/collisionmodel.h"
#include "../toolbox/maths.h"
#include "../toolbox/hex.h"


std::vector<std::list<TexturedModel*>> Unknown::models;
std::list<TexturedModel*> Unknown::modelsGuide;
std::list<TexturedModel*> Unknown::modelsTriggerCube;
std::list<TexturedModel*> Unknown::modelsTriggerSphere;
std::list<TexturedModel*> Unknown::modelsTriggerCylinder;

CollisionModel* Unknown::cmBase;


Unknown::Unknown()
{

}

void Unknown::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
}

Unknown::Unknown(char data[32], bool /*useDefaultValues*/)
{
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

    char* v1 = (char*)&var1;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];

    char* v2 = (char*)&var2;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];

    char* v3 = (char*)&var3;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];


    scaleX = 12;
    scaleY = 12;
    scaleZ = 12;
    visible = true;
    baseColour.set(1,1,1);
    updateTransformationMatrixYXZ();

    collideModelOriginal = Unknown::cmBase;
    collideModelTransformed = Unknown::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);
    updateCollisionModelYXZ();
}

void Unknown::step()
{
    collideModelTransformed->isVisible = visible;

    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
    }
}

void Unknown::fillData(char data[32])
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

    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

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

bool Unknown::isSA2Object()
{
    return true;
}

std::list<TexturedModel*>* Unknown::getModels()
{
    if (ID > 109)
    {
        return &Unknown::models[0];
    }

    return &Unknown::models[ID];
}

void Unknown::loadStaticModels()
{
    if (Unknown::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading Unknown static models...\n");
    #endif

    for (int i = 0; i <= 109; i++)
    {
        std::list<TexturedModel*> list;
        Unknown::models.push_back(list);
    }

    loadObjModel(&Unknown::models[0], "res/Models/GlobalObjects/Unknown/", "Unknown.obj");

    for (int i = 1; i <= 109; i++)
    {
        std::string s = "Unknown"+std::to_string(i);
        s = s + ".obj";
        loadObjModel(&Unknown::models[i], "res/Models/GlobalObjects/Unknown/", s);
    }

    if (Unknown::cmBase == nullptr)
    {
        Unknown::cmBase = loadCollisionModel("res/Models/GlobalObjects/Unknown/", "Unknown");
    }

    loadModel(&Unknown::modelsGuide,           "res/Models/GlobalObjects/Guide/",   "Guide");
    loadModel(&Unknown::modelsTriggerCube,     "res/Models/GlobalObjects/Trigger/", "TriggerCube");
    loadModel(&Unknown::modelsTriggerSphere,   "res/Models/GlobalObjects/Trigger/", "TriggerSphere");
    loadModel(&Unknown::modelsTriggerCylinder, "res/Models/GlobalObjects/Trigger/", "TriggerCylinder");
}

void Unknown::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting Unknown static models...\n");
    #endif

    for (int i = 0; i < (int)Unknown::models.size(); i++)
    {
        Entity::deleteModels(&Unknown::models[i]);
    }
    Unknown::models.clear();
    Entity::deleteCollisionModel(&Unknown::cmBase);
}

void Unknown::updateValue(int btnIndex)
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

                data[ 2] = *(((char*)&rotationX)+1);
                data[ 3] = *(((char*)&rotationX)+0);
                data[ 4] = *(((char*)&rotationY)+1);
                data[ 5] = *(((char*)&rotationY)+0);
                data[ 6] = *(((char*)&rotationZ)+1);
                data[ 7] = *(((char*)&rotationZ)+0);

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

                data[20] = *(((char*)&var1)+3);
                data[21] = *(((char*)&var1)+2);
                data[22] = *(((char*)&var1)+1);
                data[23] = *(((char*)&var1)+0);
                data[24] = *(((char*)&var2)+3);
                data[25] = *(((char*)&var2)+2);
                data[26] = *(((char*)&var2)+1);
                data[27] = *(((char*)&var2)+0);
                data[28] = *(((char*)&var3)+3);
                data[29] = *(((char*)&var3)+2);
                data[30] = *(((char*)&var3)+1);
                data[31] = *(((char*)&var3)+0);

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
                else
                {
                    ID = newid;
                    Global::redrawWindow = true;
                    SetWindowTextA(Global::windowValues[0], std::to_string(ID).c_str());
                }
            }
            break;
        }
        catch (...) { break; }
    }

    case 1: break; //shouldnt ever happen

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
            short newRotX = Hex::stohshort(text);
            rotationX = (int)newRotX;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[5], Hex::to_string_short((short)rotationX).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 6:
    {
        try
        {
            short newRotY = Hex::stohshort(text);
            rotationY = (int)newRotY;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[6], Hex::to_string_short((short)rotationY).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 7:
    {
        try
        {
            short newRotZ = Hex::stohshort(text);
            rotationZ = (int)newRotZ;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[7], Hex::to_string_short((short)rotationZ).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            float newVar1 = std::stof(text);
            var1 = newVar1;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[8], std::to_string(var1).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newVar2 = std::stof(text);
            var2 = newVar2;
            updateTransformationMatrixYXZ();
            updateCollisionModelYXZ();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[9], std::to_string(var2).c_str());
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
}

void Unknown::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Variable 1");
    SetWindowTextA(Global::windowLabels[ 9], "Variable 2");
    SetWindowTextA(Global::windowLabels[10], "Variable 3");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], LevelLoader::getObjectName(Global::levelID, ID).c_str());
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], Hex::to_string_short((short)rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], Hex::to_string_short((short)rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], Hex::to_string_short((short)rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(var1).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(var2).c_str());
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
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
}
