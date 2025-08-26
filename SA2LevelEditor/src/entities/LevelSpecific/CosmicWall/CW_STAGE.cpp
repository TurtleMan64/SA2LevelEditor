#include <Windows.h>
#include <glad/glad.h>
#include <cstring>

#include "../../entity.h"
#include "../../dummy.h"
#include "../../../toolbox/vector.h"
#include "cw_stage.h"
#include "../../../models/texturedmodel.h"
#include "../../../loading/objLoader.h"
#include "../../../loading/levelloader.h"
#include "../../../main/main.h"
#include "../../../collision/collisionmodel.h"
#include "../../../collision/collisionchecker.h"
#include "../../../toolbox/maths.h"

#include <list>

std::list<TexturedModel*> CW_STAGE::models;
std::list<TexturedModel*> CW_STAGE::modelsRails;

CollisionModel* CW_STAGE::cmBase;
CollisionModel* CW_STAGE::cmBaseRails;

CW_STAGE::CW_STAGE()
{

}

void CW_STAGE::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
}

CW_STAGE::CW_STAGE(char data[32], bool /*useDefaultValues*/)
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
    
    char* v1 = (char*)&scaleX;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];

    char* v2 = (char*)&scaleY;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];

    char* v3 = (char*)&scaleZ;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];

    visible = true;
    baseColour.set(1, 1, 1);
    updateTransformationMatrixY(1.0f, 1.0f, 1.0f);

    if (hasRails())
    {
        collideModelOriginal = CW_STAGE::cmBaseRails;
    }
    else
    {
        collideModelOriginal = CW_STAGE::cmBase;
    }

    collideModelTransformed = collideModelOriginal->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);

    updateTransformationMatrixY(1.0f, 1.0f, 1.0f);
    updateCollisionModelY(1.0f, 1.0f, 1.0f);
}

bool CW_STAGE::isSA2Object()
{
    return true;
}

void CW_STAGE::step()
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

std::list<TexturedModel*>* CW_STAGE::getModels()
{
    if (hasRails())
    {
        return &CW_STAGE::modelsRails;
    }
    else
    {
        return &CW_STAGE::models;
    }
}

void CW_STAGE::loadStaticModels()
{
    if (CW_STAGE::models.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading CW_STAGE static models...\n");
    #endif

    loadModel(&CW_STAGE::models,      "res/Models/LevelObjects/CosmicWall/", "STAGE");
    loadModel(&CW_STAGE::modelsRails, "res/Models/LevelObjects/CosmicWall/", "STAGE_RAILS");

    CW_STAGE::cmBase      = loadCollisionModel("res/Models/LevelObjects/CosmicWall/", "STAGE");
    CW_STAGE::cmBaseRails = loadCollisionModel("res/Models/LevelObjects/CosmicWall/", "STAGE_RAILS");
}

void CW_STAGE::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting SNAKESTATUE static models...\n");
    #endif

    Entity::deleteModels(&CW_STAGE::models);
    Entity::deleteModels(&CW_STAGE::modelsRails);
    Entity::deleteCollisionModel(&CW_STAGE::cmBase);
    Entity::deleteCollisionModel(&CW_STAGE::cmBaseRails);
}

void CW_STAGE::updateValue(int btnIndex)
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
        }
        catch (...) { }
        break;
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
            scaleX = std::stof(text);
            SetWindowTextA(Global::windowValues[8], std::to_string(scaleX).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            scaleY = std::stof(text);
            SetWindowTextA(Global::windowValues[8], std::to_string(scaleY).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            scaleZ = std::stof(text);
            SetWindowTextA(Global::windowValues[8], std::to_string(scaleZ).c_str());
            break;
        }
        catch (...) { break; }
    }
    
    default: break;
    }

    updateTransformationMatrixY(1.0f, 1.0f, 1.0f);
    updateCollisionModelY(1.0f, 1.0f, 1.0f);
    Global::redrawWindow = true;
}

void CW_STAGE::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Has Rail");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "");
    SetWindowTextA(Global::windowLabels[ 8], "");
    SetWindowTextA(Global::windowLabels[ 9], "");
    SetWindowTextA(Global::windowLabels[10], "");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "STAGE");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(scaleX).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(scaleY).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(scaleZ).c_str());

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "Unused?");
    SetWindowTextA(Global::windowDescriptions[ 9], "Unused?");
    SetWindowTextA(Global::windowDescriptions[10], "Unused?");

    updateTransformationMatrixY(1.0f, 1.0f, 1.0f);
    updateCollisionModelY(1.0f, 1.0f, 1.0f);
}

void CW_STAGE::fillData(char data[32])
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

    float var1 = scaleX - 1.0f;
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = scaleY - 1.0f;
    ptr = (char*)(&var2);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

    float var3 = scaleZ - 1.0f;
    ptr = (char*)(&var3);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}

bool CW_STAGE::hasRails()
{
    return (rotationX % 2 == 1);
}
