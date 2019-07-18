#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "lightsw.h"
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


std::list<TexturedModel*> LIGHT_SW::modelsCube;
std::list<TexturedModel*> LIGHT_SW::modelsSphere;
std::list<TexturedModel*> LIGHT_SW::modelsCylinder;

CollisionModel* LIGHT_SW::cmBaseCube;
CollisionModel* LIGHT_SW::cmBaseSphere;
CollisionModel* LIGHT_SW::cmBaseCylinder;

LIGHT_SW::LIGHT_SW()
{

}

void LIGHT_SW::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    if (box != nullptr)
    {
        Global::deleteTransparentEntity(box);
        box = nullptr;
    }
}

LIGHT_SW::LIGHT_SW(char data[32], bool useDefaultValues)
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    signed short rX = 0;
    signed short rY = 0;
    signed short rZ = 0;

    char* ptr = (char*)(&rX);
    memset(ptr, data[3], 1);
    memset(ptr+1, data[2], 1);

    ptr = (char*)(&rY);
    memset(ptr, data[5], 1);
    memset(ptr+1, data[4], 1);

    ptr = (char*)(&rZ);
    memset(ptr, data[7], 1);
    memset(ptr+1, data[6], 1);

    rotationX = (int)(rX);
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

    collisionType = 0;

    if (useDefaultValues)
    {
        var1 = 10.0f;
        var2 = 10.0f;
        var3 = 10.0f;
        rotationX = 0;
        rotationY = 0;
	    rotationZ = 0; 
    }

    collisionType = (rotationZ & 0x03);

	scaleX = 0;
    scaleY = 0;
    scaleZ = 0;
	visible = true;
	updateTransformationMatrixZY();

    if (collisionType == 0) //box
    {
        collideModelOriginal    = LIGHT_SW::cmBaseCube;
        collideModelTransformed = LIGHT_SW::cmBaseCube->duplicateMe();
    }
    else if (collisionType == 1) //sphere
    {
        collideModelOriginal    = LIGHT_SW::cmBaseSphere;
        collideModelTransformed = LIGHT_SW::cmBaseSphere->duplicateMe();
    }
    else if (collisionType == 2) //cylinder
    {
        collideModelOriginal    = LIGHT_SW::cmBaseCylinder;
        collideModelTransformed = LIGHT_SW::cmBaseCylinder->duplicateMe();
    }
    else
    {
        std::fprintf(stdout, "Warning: LIGHT_SW had bad collision type. Probably going to crash now.\n");
    }

    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateMyCollisionModel();

    box = nullptr;
    box = new Dummy(getSpecificModels(collisionType)); INCR_NEW("Entity");
    box->setPosition(&position);
    box->setRotation(rotationX&0xFFFFFF00, rotationY&0xFFFFFF00, rotationZ&0xFFFFFF00);
    box->visible = true;
    box->baseColour.set(0.8f, 0.8f, 0.8f);
    updateBoxTransformationMatrix();
    Global::addTransparentEntity(box);
}

bool LIGHT_SW::isSA2Object()
{
    return true;
}

void LIGHT_SW::step()
{
    if (Global::selectedSA2Object == this)
    {
        box->baseColour.set(1.6f, 1.6f, 1.6f);
    }
    else
    {
        box->baseColour.set(0.8f, 0.8f, 0.8f);
    }
}

std::list<TexturedModel*>* LIGHT_SW::getModels()
{
	return &LIGHT_SW::modelsCube;
}

std::list<TexturedModel*>* LIGHT_SW::getSpecificModels(int type)
{
    switch (type)
    {
    case 0: return &LIGHT_SW::modelsCube;
    case 1: return &LIGHT_SW::modelsSphere;
    case 2: return &LIGHT_SW::modelsCylinder;
    default: std::fprintf(stdout, "LIGHT_SW had bad collision type. Probably going to crash now.\n"); break;
    }

    return &LIGHT_SW::modelsCube;
}

void LIGHT_SW::loadStaticModels()
{
	if (LIGHT_SW::modelsCube.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading LIGHT_SW static models...\n");
	#endif

	loadModel(&LIGHT_SW::modelsCube,     "res/Models/GlobalObjects/LightVolumes/", "LightBox");
    loadModel(&LIGHT_SW::modelsSphere,   "res/Models/GlobalObjects/LightVolumes/", "LightSphere");
    loadModel(&LIGHT_SW::modelsCylinder, "res/Models/GlobalObjects/LightVolumes/", "LightCylinder");

    if (LIGHT_SW::cmBaseCube == nullptr)
	{
		LIGHT_SW::cmBaseCube = loadCollisionModel("res/Models/GlobalObjects/LightVolumes/", "LightBox");
	}
    if (LIGHT_SW::cmBaseSphere == nullptr)
	{
		LIGHT_SW::cmBaseSphere = loadCollisionModel("res/Models/GlobalObjects/LightVolumes/", "LightSphere");
	}
    if (LIGHT_SW::cmBaseCylinder == nullptr)
	{
		LIGHT_SW::cmBaseCylinder = loadCollisionModel("res/Models/GlobalObjects/LightVolumes/", "LightCylinder");
	}
}

void LIGHT_SW::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting LIGHT_SW static models...\n");
	#endif

	Entity::deleteModels(&LIGHT_SW::modelsCube);
    Entity::deleteModels(&LIGHT_SW::modelsSphere);
    Entity::deleteModels(&LIGHT_SW::modelsCylinder);
    Entity::deleteCollisionModel(&LIGHT_SW::cmBaseCube);
    Entity::deleteCollisionModel(&LIGHT_SW::cmBaseSphere);
    Entity::deleteCollisionModel(&LIGHT_SW::cmBaseCylinder);
}

void LIGHT_SW::updateValue(int btnIndex)
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
                    CollisionChecker::deleteCollideModel(collideModelTransformed);
                    Global::deleteEntity(this);
                    cleanUp();
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
            int newRotX = Hex::stoh(text);
            rotationX = newRotX;
            SetWindowTextA(Global::windowValues[5], Hex::to_string(rotationX).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 6:
    {
        try
        {
            int newRotY = Hex::stoh(text);
            rotationY = newRotY;
            SetWindowTextA(Global::windowValues[6], Hex::to_string(rotationY).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 7:
    {
        try
        {
            int newRotZ = Hex::stoh(text);

            //we have switch collision types, must delete collision model
            // and make a new one
            if (collisionType != (newRotZ & 0x03))
            {
                CollisionChecker::deleteCollideModel(collideModelTransformed);

                collisionType = newRotZ & 0x03;
                if (collisionType == 0) //box
                {
                    collideModelOriginal    = LIGHT_SW::cmBaseCube;
                    collideModelTransformed = LIGHT_SW::cmBaseCube->duplicateMe();
                }
                else if (collisionType == 1) //sphere
                {
                    collideModelOriginal    = LIGHT_SW::cmBaseSphere;
                    collideModelTransformed = LIGHT_SW::cmBaseSphere->duplicateMe();
                }
                else if (collisionType == 2) //cylinder
                {
                    collideModelOriginal    = LIGHT_SW::cmBaseCylinder;
                    collideModelTransformed = LIGHT_SW::cmBaseCylinder->duplicateMe();
                }
                else
                {
                    std::fprintf(stdout, "Warning: LIGHT_SW had bad collision type. Probably going to crash now.\n");
                }

                collideModelTransformed->parent = this;
	            CollisionChecker::addCollideModel(collideModelTransformed);
            }

            rotationZ = newRotZ;
            SetWindowTextA(Global::windowValues[7], Hex::to_string(rotationZ).c_str());
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

    updateMyCollisionModel();
    Global::redrawWindow = true;
    box->setPosition(&position);
    box->setRotation(rotationX&0xFFFFFF00, rotationY&0xFFFFFF00, rotationZ&0xFFFFFF00);
    box->setModels(getSpecificModels(collisionType));
    updateBoxTransformationMatrix();
}

void LIGHT_SW::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Scale X");
    SetWindowTextA(Global::windowLabels[ 9], "Scale Y");
    SetWindowTextA(Global::windowLabels[10], "Scale Z");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "LIGHT SW");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], Hex::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], Hex::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], Hex::to_string(rotationZ).c_str());
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
    SetWindowTextA(Global::windowDescriptions[ 1], "Shadow volume. When player is inside, they are rendered with a different light color.");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateMyCollisionModel();
    box->setPosition(&position);
    box->setRotation(rotationX&0xFFFFFF00, rotationY&0xFFFFFF00, rotationZ&0xFFFFFF00);
    box->setModels(getSpecificModels(collisionType));
    updateBoxTransformationMatrix();
}

void LIGHT_SW::fillData(char data[32])
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

void LIGHT_SW::updateBoxTransformationMatrix()
{
    float rad = std::sqrtf(var1*var1 + var2*var2 + var3*var3);
    switch (collisionType)
    {
    case 0: box->updateTransformationMatrixZY(var1, var2, var3); break;
    case 1: box->updateTransformationMatrixZY(rad, rad, rad); break;
    case 2: box->updateTransformationMatrixYXZ(var1, var2, var1); break;
    default: std::fprintf(stdout, "Warning: LIGHT_SW does not have a defined collision type.\n");
    }
}

void LIGHT_SW::updateMyCollisionModel()
{
    float rad = std::sqrtf(var1*var1 + var2*var2 + var3*var3);
    switch (collisionType)
    {
    case 0: updateCollisionModelZY(var1, var2, var3); break;
    case 1: updateCollisionModelZY(rad, rad, rad); break;
    case 2: updateCollisionModelYXZ(var1, var2, var1); break;
    default: std::fprintf(stdout, "Warning: LIGHT_SW does not have a defined collision type.\n");
    }
}
