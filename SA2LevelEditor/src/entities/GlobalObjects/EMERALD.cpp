#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "emerald.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../../toolbox/hex.h"
#include "../dummy.h"
#include "../unknown.h"

#include <list>



std::list<TexturedModel*> EMERALD::models;
CollisionModel* EMERALD::cmBase;

EMERALD::EMERALD()
{

}

void EMERALD::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    if (hitbox != nullptr)
    {
        Global::deleteTransparentEntity(hitbox);
        hitbox = nullptr;
    }
}

EMERALD::EMERALD(char data[32], bool useDefaultValues)
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

    pieceID = (int)rX;
    rotY    = (int)rY;
    rotZ    = (int)rZ;

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

    char* v2 = (char*)&radius;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
    radius += 5;

    char* v3 = (char*)&var3;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];

    if (useDefaultValues)
    {
        pieceID = 0;
        rotY = 0;
        rotZ = 0;
        var1 = 0.0f;
        radius = 10.0f;
        var3 = 0.0f;
    }

	rotationX = 0;
    rotationY = 0;
	rotationZ = 0; 
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrixYXZ();

    collideModelOriginal = EMERALD::cmBase;
	collideModelTransformed = EMERALD::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelYXZ();

    hitbox = nullptr;
    hitbox = new Dummy(&Unknown::modelsTriggerSphere); INCR_NEW("Entity");
    hitbox->setPosition(&position);
    hitbox->setRotation(0, 0, 0);
    hitbox->setScale(fminf(radius, 400), fminf(radius, 400), fminf(radius, 400));
    hitbox->visible = true;
    hitbox->updateTransformationMatrixYXZ();
    Global::addTransparentEntity(hitbox);
}

bool EMERALD::isSA2Object()
{
    return true;
}

void EMERALD::step()
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

std::list<TexturedModel*>* EMERALD::getModels()
{
	return &EMERALD::models;
}

void EMERALD::loadStaticModels()
{
	if (EMERALD::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading EMERALD static models...\n");
	#endif

	loadModel(&EMERALD::models, "res/Models/GlobalObjects/HuntingPieces/", "Shard");

    if (EMERALD::cmBase == nullptr)
	{
		EMERALD::cmBase = loadCollisionModel("res/Models/GlobalObjects/HuntingPieces/", "Shard");
	}
}

void EMERALD::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting EMERALD static models...\n");
	#endif

	Entity::deleteModels(&EMERALD::models);
    Entity::deleteCollisionModel(&EMERALD::cmBase);
}

void EMERALD::updateValue(int btnIndex)
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

    case 5:
    {
        try
        {
            short newPieceID = Hex::stohshort(text);
            pieceID = newPieceID;
            SetWindowTextA(Global::windowValues[5], Hex::to_string_short(pieceID).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 6:
    {
        try
        {
            short newRotY = Hex::stohshort(text);
            rotY = newRotY;
            SetWindowTextA(Global::windowValues[6], Hex::to_string_short(rotY).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 7:
    {
        try
        {
            short newRotZ = Hex::stohshort(text);
            rotZ = newRotZ;
            SetWindowTextA(Global::windowValues[7], Hex::to_string_short(rotZ).c_str());
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
            SetWindowTextA(Global::windowValues[8], std::to_string(var1).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newRadius = std::stof(text);
            if (newRadius > 400.0f)
            {
                MessageBox(NULL, "Warning: The collision radius is too large, emerald won't be picked up any further than 400.", "Warning", MB_OK);
            }
            radius = newRadius;
            SetWindowTextA(Global::windowValues[9], std::to_string(radius).c_str());
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

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    Global::redrawWindow = true;
    hitbox->setPosition(&position);
    hitbox->setRotation(0, 0, 0);
    hitbox->setScale(fminf(radius, 400), fminf(radius, 400), fminf(radius, 400));
    hitbox->updateTransformationMatrixYXZ();
}

void EMERALD::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Piece Number");
    SetWindowTextA(Global::windowLabels[ 6], "");
    SetWindowTextA(Global::windowLabels[ 7], "");
    SetWindowTextA(Global::windowLabels[ 8], "Path");
    SetWindowTextA(Global::windowLabels[ 9], "Radius");
    SetWindowTextA(Global::windowLabels[10], "Speed");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "EMERALD");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], Hex::to_string_short(pieceID).c_str());
    SetWindowTextA(Global::windowValues[ 6], Hex::to_string_short(rotY).c_str());
    SetWindowTextA(Global::windowValues[ 7], Hex::to_string_short(rotZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(var1).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(radius).c_str());
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
    SetWindowTextA(Global::windowDescriptions[ 1], "Treasure hunting item");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "The identifier for this specific piece");
    SetWindowTextA(Global::windowDescriptions[ 6], "Unused?");
    SetWindowTextA(Global::windowDescriptions[ 7], "Unused?");
    SetWindowTextA(Global::windowDescriptions[ 8], "On moving pieces, determines the movement pattern. Each integer has an assigned pattern by stage.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Radius of hit detection (calculated from centerpoint of character). In practice, has a max value of 400, (SA2 will not calculate the collision when the player is > 400 away).");
    SetWindowTextA(Global::windowDescriptions[10], "On moving pieces, determines movement speed, with -3 serving as 0.");

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    hitbox->setPosition(&position);
    hitbox->setRotation(0, 0, 0);
    hitbox->setScale(fminf(radius, 400), fminf(radius, 400), fminf(radius, 400));
    hitbox->updateTransformationMatrixYXZ();
}

void EMERALD::fillData(char data[32])
{
    data[1] = (char)ID;

    data[2] = (char)((pieceID >> 8) & 0xFF);
    data[3] = (char)((pieceID >> 0) & 0xFF);
    data[4] = (char)((rotY    >> 8) & 0xFF);
    data[5] = (char)((rotY    >> 0) & 0xFF);
    data[6] = (char)((rotZ    >> 8) & 0xFF);
    data[7] = (char)((rotZ    >> 0) & 0xFF);

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

    float var2 = radius - 5.0f;
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
