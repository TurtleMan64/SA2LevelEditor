#include <Windows.h>
#include <glad/glad.h>
#include <cstring>

#include "../../entity.h"
#include "../../dummy.h"
#include "../../../toolbox/vector.h"
#include "snakestatue.h"
#include "../../../models/texturedmodel.h"
#include "../../../loading/objLoader.h"
#include "../../../loading/levelloader.h"
#include "../../../main/main.h"
#include "../../../collision/collisionmodel.h"
#include "../../../collision/collisionchecker.h"
#include "../../../toolbox/maths.h"

#include <list>

std::list<TexturedModel*> SNAKESTATUE::modelsLeft;
std::list<TexturedModel*> SNAKESTATUE::modelsRight;
std::list<TexturedModel*> SNAKESTATUE::modelsLeftCollision;
std::list<TexturedModel*> SNAKESTATUE::modelsRightCollision;

CollisionModel* SNAKESTATUE::cmBaseLeft;
CollisionModel* SNAKESTATUE::cmBaseRight;
CollisionModel* SNAKESTATUE::cmBaseLeftCollision;
CollisionModel* SNAKESTATUE::cmBaseRightCollision;

SNAKESTATUE::SNAKESTATUE()
{

}

void SNAKESTATUE::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
}

SNAKESTATUE::SNAKESTATUE(char data[32], bool useDefaultValues)
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

    if (((int)scaleZ) % 2 == 1)
    {
        isLeft = true;
    }
    else
    {
        rotationY += 32768;
    }

    scaleX = scaleX + 1.0f;
    scaleY = scaleX;
    scaleZ = scaleX;

    if (useDefaultValues)
    {
        scaleX = 1.0f; 
        scaleY = 1.0f;
        scaleZ = 1.0f;
    }

    rotationX = 0;
    rotationZ = 0;

    visible = true;
    baseColour.set(1, 1, 1);
    updateTransformationMatrixZXY();

    if (isLeft)
    {
        collideModelOriginal = SNAKESTATUE::cmBaseLeftCollision;
    }
    else
    {
        collideModelOriginal = SNAKESTATUE::cmBaseRightCollision;
    }

    collideModelTransformed = collideModelOriginal->duplicateMe();
    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
}

bool SNAKESTATUE::isSA2Object()
{
    return true;
}

void SNAKESTATUE::step()
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

std::list<TexturedModel*>* SNAKESTATUE::getModels()
{
    if (isLeft)
    {
        return &SNAKESTATUE::modelsLeftCollision;
    }
    else
    {
        return &SNAKESTATUE::modelsRightCollision;
    }
}

void SNAKESTATUE::loadStaticModels()
{
    if (SNAKESTATUE::modelsLeft.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading SNAKESTATUE static models...\n");
    #endif

    loadModel(&SNAKESTATUE::modelsLeft,           "res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "LEFT");
    loadModel(&SNAKESTATUE::modelsRight,          "res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "RIGHT");
    loadModel(&SNAKESTATUE::modelsLeftCollision,  "res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "LEFT_COLLISION");
    loadModel(&SNAKESTATUE::modelsRightCollision, "res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "RIGHT_COLLISION");

    SNAKESTATUE::cmBaseLeft           = loadCollisionModel("res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "LEFT");
    SNAKESTATUE::cmBaseRight          = loadCollisionModel("res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "RIGHT");
    SNAKESTATUE::cmBaseLeftCollision  = loadCollisionModel("res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "LEFT_COLLISION");
    SNAKESTATUE::cmBaseRightCollision = loadCollisionModel("res/Models/LevelObjects/PyramidCave/SNAKESTATUE/", "RIGHT_COLLISION");
}

void SNAKESTATUE::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting SNAKESTATUE static models...\n");
    #endif

    Entity::deleteModels(&SNAKESTATUE::modelsLeft);
    Entity::deleteModels(&SNAKESTATUE::modelsRight);
    Entity::deleteModels(&SNAKESTATUE::modelsLeftCollision);
    Entity::deleteModels(&SNAKESTATUE::modelsRightCollision);
    Entity::deleteCollisionModel(&SNAKESTATUE::cmBaseLeft);
    Entity::deleteCollisionModel(&SNAKESTATUE::cmBaseRight);
    Entity::deleteCollisionModel(&SNAKESTATUE::cmBaseLeftCollision);
    Entity::deleteCollisionModel(&SNAKESTATUE::cmBaseRightCollision);
}

void SNAKESTATUE::updateValue(int btnIndex)
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
            float newScale = std::stof(text);
            scaleX = newScale;
            scaleY = newScale;
            scaleZ = newScale;
            SetWindowTextA(Global::windowValues[8], std::to_string(newScale).c_str());
            break;
        }
        catch (...) { break; }
    }

    //case 9:
    //{
    //    try
    //    {
    //        float newScale = std::stof(text);
    //        scaleY = newScale;
    //        SetWindowTextA(Global::windowValues[9], std::to_string(newScale).c_str());
    //        break;
    //    }
    //    catch (...) { break; }
    //}
    
    default: break;
    }

    updateTransformationMatrixZXY();
    updateCollisionModelZXY();
    Global::redrawWindow = true;
}

void SNAKESTATUE::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Scale?");
    SetWindowTextA(Global::windowLabels[ 9], "Unknown");
    SetWindowTextA(Global::windowLabels[10], "Side");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "SNAKESTATUE");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(scaleX).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(scaleY).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(isLeft).c_str());

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
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "Not sure");
    SetWindowTextA(Global::windowDescriptions[10], "Snake direction");

    updateTransformationMatrixZXY();
    updateCollisionModelZXY();
}

void SNAKESTATUE::fillData(char data[32])
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

    float var3 = 0.0f;
    if (isLeft)
    {
        var3 = 1.0f;
    }
    ptr = (char*)(&var3);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}
