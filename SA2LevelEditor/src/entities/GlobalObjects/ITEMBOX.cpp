#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "itembox.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../../toolbox/hex.h"
#include "../dummy.h"

#include <list>
#include <vector>


std::list<TexturedModel*> ITEMBOX::modelsBase;
std::list<TexturedModel*> ITEMBOX::modelsShell;
std::vector<std::list<TexturedModel*>> ITEMBOX::modelsItem;

CollisionModel* ITEMBOX::cmBaseBase = nullptr;
CollisionModel* ITEMBOX::cmBaseShell = nullptr;

ITEMBOX::ITEMBOX()
{

}

void ITEMBOX::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    if (collideModelShellTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelShellTransformed);
        collideModelShellTransformed = nullptr;
    }
    
    if (shell != nullptr)
    {
        Global::deleteTransparentEntity(shell);
        shell = nullptr;
    }
    
    if (item != nullptr)
    {
        Global::deleteEntity(item);
        item = nullptr;
    }
}

ITEMBOX::ITEMBOX(char data[32], bool useDefaultValues)
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    signed short rY = 0;
    signed short rZ = 0;

    char* ptr = (char*)(&rY);
    memset(ptr, data[5], 1);
    memset(ptr+1, data[4], 1);

    ptr = (char*)(&rZ);
    memset(ptr, data[7], 1);
    memset(ptr+1, data[6], 1);

    rotationY = (int)(rY);
    rotationZ = (int)(rZ);

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
    
    itemType = (int)var1;

    if (useDefaultValues)
    {
        itemType = 1;
        var2 = 0.0f;
        var3 = 0.0f;
        rotationY = 0;
        rotationZ = 0; 
    }

    rotationX = 0;
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
    visible = true;
    updateTransformationMatrixZY();

    collideModelOriginal         = ITEMBOX::cmBaseBase;
    collideModelTransformed      = ITEMBOX::cmBaseBase->duplicateMe();
    collideModelShellTransformed = ITEMBOX::cmBaseShell->duplicateMe();

    collideModelTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelTransformed);
    updateCollisionModelZY();

    collideModelShellTransformed->parent = this;
    CollisionChecker::addCollideModel(collideModelShellTransformed);
    updateCollisionModelZY(ITEMBOX::cmBaseShell, collideModelShellTransformed);

    shell = new Dummy(&ITEMBOX::modelsShell); INCR_NEW("Entity");
    shell->setPosition(&position);
    shell->setRotation(0, rotationY, rotationZ);
    shell->visible = true;
    shell->baseColour.set(1.0f, 1.0f, 1.0f);
    shell->updateTransformationMatrixZY();
    Global::addTransparentEntity(shell);

    item = new Dummy(&ITEMBOX::modelsItem[std::min(10, std::max(0, itemType))]); INCR_NEW("Entity");
    item->setPosition(&position);
    item->setRotation(0, rotationY, rotationZ);
    item->visible = true;
    item->baseColour.set(1.0f, 1.0f, 1.0f);
    item->updateTransformationMatrixZY();
    Global::addEntity(item);
}

bool ITEMBOX::isSA2Object()
{
    return true;
}

std::string ITEMBOX::toSabString()
{
    Vector3f dir(0, 1, 0);
    Vector3f yAxis(0, 1, 0);
    Vector3f zAxis(0, 0, 1);

    dir = Maths::rotatePoint(&dir, &zAxis, Maths::bamsToRad(rotationZ));
    dir = Maths::rotatePoint(&dir, &yAxis, Maths::bamsToRad(rotationY));

    return "27 " + 
        std::to_string(position.x) + " " + 
        std::to_string(position.y) + " " + 
        std::to_string(position.z) + " " + 
        std::to_string(dir.x)      + " " + 
        std::to_string(dir.y)      + " " + 
        std::to_string(dir.z)      + " " + 
        std::to_string(itemType)   + " 0";
}

void ITEMBOX::step()
{
    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.6f, 1.6f, 1.6f);
        shell->baseColour.set(1.6f, 1.6f, 1.6f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
        shell->baseColour.set(1.0f, 1.0f, 1.0f);
    }
}

std::list<TexturedModel*>* ITEMBOX::getModels()
{
    return &ITEMBOX::modelsBase;
}

void ITEMBOX::loadStaticModels()
{
    if (ITEMBOX::modelsBase.size() > 0)
    {
        return;
    }

    #ifdef DEV_MODE
    std::fprintf(stdout, "Loading ITEMBOX static models...\n");
    #endif

    loadModel(&ITEMBOX::modelsBase,  "res/Models/GlobalObjects/ItemBox/", "ItemBoxBase");
    loadModel(&ITEMBOX::modelsShell, "res/Models/GlobalObjects/ItemBox/", "ItemBoxShell");

    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemSpeedUp");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemRing5");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "Item1Up");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemRing10");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemRing20");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemBarrierG");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemBomb");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemHealth");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemBarrierB");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemNothing");
    ITEMBOX::loadNewItemModel("res/Models/GlobalObjects/ItemBox/Items/", "ItemInvincible");

    if (ITEMBOX::cmBaseBase == nullptr)
    {
        ITEMBOX::cmBaseBase = loadCollisionModel("res/Models/GlobalObjects/ItemBox/", "ItemBoxBase");
    }
    if (ITEMBOX::cmBaseShell == nullptr)
    {
        ITEMBOX::cmBaseShell = loadCollisionModel("res/Models/GlobalObjects/ItemBox/", "ItemBoxShell");
    }
}

void ITEMBOX::loadNewItemModel(std::string folder, std::string filename)
{
    std::list<TexturedModel*> item;
    loadModel(&item, folder, filename);
    ITEMBOX::modelsItem.push_back(item);
}

void ITEMBOX::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting ITEMBOX static models...\n");
    #endif

    Entity::deleteModels(&ITEMBOX::modelsBase);
    Entity::deleteModels(&ITEMBOX::modelsShell);
    for (int i = 0; i < (int)ITEMBOX::modelsItem.size(); i++)
    {
        Entity::deleteModels(&ITEMBOX::modelsItem[i]);
    }
    ITEMBOX::modelsItem.clear();
    Entity::deleteCollisionModel(&ITEMBOX::cmBaseBase);
    Entity::deleteCollisionModel(&ITEMBOX::cmBaseShell);
}

void ITEMBOX::updateValue(int btnIndex)
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

    case 6:
    {
        try
        {
            short newRotY = Hex::stohshort(text);
            rotationY = (int)newRotY;
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
            itemType = (int)newVar1;
            SetWindowTextA(Global::windowValues[8], std::to_string(itemType).c_str());
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
            SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());
            break;
        }
        catch (...) { break; }
    }
    
    default: break;
    }

    Global::redrawWindow = true;

    shell->setPosition(&position);
    shell->setRotation(0, rotationY, rotationZ);
    item->setPosition(&position);
    item->setRotation(0, rotationY, rotationZ);

    item->setModels(&ITEMBOX::modelsItem[std::min(10, std::max(0, itemType))]);

    updateTransformationMatrixZY();
    shell->updateTransformationMatrixZY();
    item->updateTransformationMatrixZY();

    updateCollisionModelZY();
    updateCollisionModelZY(ITEMBOX::cmBaseShell, collideModelShellTransformed);
}

void ITEMBOX::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Item Type");
    SetWindowTextA(Global::windowLabels[ 9], "Item Rot Speed?");
    SetWindowTextA(Global::windowLabels[10], "Unknown");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "ITEMBOX");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], "");
    SetWindowTextA(Global::windowValues[ 6], Hex::to_string_short((short)rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], Hex::to_string_short((short)rotationZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(itemType).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(var2).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "Anchored item box");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "Unused?");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "0: speed shoes, 1: 5 rings, 2: extra life, 3: 10 rings, 4: 20 rings, 5: shield, 6: bomb, 7: health, 8: electric shield, 9: nothing, 10+: invincibility");
    SetWindowTextA(Global::windowDescriptions[ 9], "Y Rotation of the icon inside of the item box?");
    SetWindowTextA(Global::windowDescriptions[10], "");

    shell->setPosition(&position);
    shell->setRotation(0, rotationY, rotationZ);
    item->setPosition(&position);
    item->setRotation(0, rotationY, rotationZ);

    item->setModels(&ITEMBOX::modelsItem[std::min(10, std::max(0, itemType))]);

    updateTransformationMatrixZY();
    shell->updateTransformationMatrixZY();
    item->updateTransformationMatrixZY();

    updateCollisionModelZY();
    updateCollisionModelZY(ITEMBOX::cmBaseShell, collideModelShellTransformed);
}

void ITEMBOX::fillData(char data[32])
{
    data[1] = (char)ID;

    data[2] = (char)((0 >> 8) & 0xFF);
    data[3] = (char)((0 >> 0) & 0xFF);
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

    float var1 = (float)itemType;
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
