#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "linklink.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../collision/triangle3d.h"
#include "../../toolbox/maths.h"
#include "../../toolbox/hex.h"

#include <list>


std::list<TexturedModel*> LINKLINK::modelsSphere;
std::list<TexturedModel*> LINKLINK::modelsCube;

CollisionModel* LINKLINK::cmBaseSphere;
CollisionModel* LINKLINK::cmBaseCube;

LINKLINK::LINKLINK()
{

}

void LINKLINK::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
}

LINKLINK::LINKLINK(char data[32], bool useDefaultValues)
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    char* ptr = (char*)(&timer);
    memset(ptr, data[3], 1);
    memset(ptr+1, data[2], 1);

    ptr = (char*)(&rotY);
    memset(ptr, data[5], 1);
    memset(ptr+1, data[4], 1);

    ptr = (char*)(&rotZ);
    memset(ptr, data[7], 1);
    memset(ptr+1, data[6], 1);

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
    
    isSphere = rotZ & 0x01;
    
    rotationX = 0;
    rotationY = rotY & 0xFFFFFF00;
    rotationZ = rotZ & 0xFFFFFF00;
    //rotationY = 0;
    //rotationZ = 0;

    if (useDefaultValues)
    {
        isSphere = false;
        timer = 0;
        rotY = 0;
        rotZ = 0;
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
    }
    
    visible = true;
	baseColour.set(1, 1, 1);
    
    if (isSphere)
    {
        scaleX = var1;
        scaleY = var1;
        scaleZ = var1;
        
        collideModelOriginal    = LINKLINK::cmBaseSphere;
        collideModelTransformed = LINKLINK::cmBaseSphere->duplicateMe();
    }
    else
    {
        scaleX = var1;
        scaleY = var2;
        scaleZ = var3;
        
        collideModelOriginal    = LINKLINK::cmBaseCube;
        collideModelTransformed = LINKLINK::cmBaseCube->duplicateMe();
    }

    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);

	updateCollisionModelZY();
    updateTransformationMatrixZY();
}

bool LINKLINK::isSA2Object()
{
    return true;
}

void LINKLINK::step()
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

std::list<TexturedModel*>* LINKLINK::getModels()
{
    if (isSphere)
    {
        return &LINKLINK::modelsSphere;
    }
    
	return &LINKLINK::modelsCube;
}

void LINKLINK::loadStaticModels()
{
	if (LINKLINK::modelsSphere.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading LINKLINK static models...\n");
	#endif

	loadModel(&LINKLINK::modelsCube, "res/Models/GlobalObjects/LinkLink/", "Cube");
    loadModel(&LINKLINK::modelsSphere, "res/Models/GlobalObjects/LinkLink/", "Sphere");

    if (LINKLINK::cmBaseSphere == nullptr)
	{
		LINKLINK::cmBaseSphere = loadCollisionModel("res/Models/GlobalObjects/LinkLink/", "Sphere");
	}
    if (LINKLINK::cmBaseCube == nullptr)
	{
		LINKLINK::cmBaseCube = loadCollisionModel("res/Models/GlobalObjects/LinkLink/", "Cube");
	}
}

void LINKLINK::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting LINKLINK static models...\n");
	#endif

	Entity::deleteModels(&LINKLINK::modelsSphere);
    Entity::deleteModels(&LINKLINK::modelsCube);
    Entity::deleteCollisionModel(&LINKLINK::cmBaseSphere);
    Entity::deleteCollisionModel(&LINKLINK::cmBaseCube);
}

void LINKLINK::updateValue(int btnIndex)
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
            short newTimer = (short)std::stoi(text);
            timer = newTimer;
            SetWindowTextA(Global::windowValues[5], std::to_string(timer).c_str());
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
            rotationY = rotY & 0xFFFFFF00;
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

            //we have switch collision types, must delete collision model
            // and make a new one
            if (isSphere != (bool)(newRotZ & 0x01))
            {
                CollisionChecker::deleteCollideModel(collideModelTransformed);

                isSphere = newRotZ & 0x01;
                if (isSphere)
                {
                    collideModelOriginal    = LINKLINK::cmBaseSphere;
                    collideModelTransformed = LINKLINK::cmBaseSphere->duplicateMe();
                }
                else
                {
                    collideModelOriginal    = LINKLINK::cmBaseCube;
                    collideModelTransformed = LINKLINK::cmBaseCube->duplicateMe();
                }

                collideModelTransformed->parent = this;
	            CollisionChecker::addCollideModel(collideModelTransformed);
            }

            rotZ = newRotZ;
            rotationZ = rotZ & 0xFFFFFF00;
            SetWindowTextA(Global::windowValues[7], Hex::to_string_short(rotZ).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            float newScaleX = std::stof(text);
            if (sqrtf(var1*var1 + var2*var2 + var3*var3) > 390.0f)
            {
                MessageBox(NULL, "Warning: The total size of the cube is too large, object may not function properly in SA2.", "Warning", MB_OK);
            }
            var1 = fmaxf(0.0f, newScaleX);
            SetWindowTextA(Global::windowValues[8], std::to_string(var1).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newScaleY = std::stof(text);
            if (sqrtf(var1*var1 + var2*var2 + var3*var3) > 390.0f)
            {
                MessageBox(NULL, "Warning: The total size of the cube is too large, object may not function properly in SA2.", "Warning", MB_OK);
            }
            var2 = fmaxf(0.0f, newScaleY);
            SetWindowTextA(Global::windowValues[9], std::to_string(var2).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            float newScaleZ = std::stof(text);
            if (sqrtf(var1*var1 + var2*var2 + var3*var3) > 390.0f)
            {
                MessageBox(NULL, "Warning: The total size of the cube is too large, object may not function properly in SA2.", "Warning", MB_OK);
            }
            var3 = fmaxf(0.0f, newScaleZ);
            SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }

    isSphere = rotZ & 0x01;

    if (isSphere)
    {
        scaleX = var1;
        scaleY = var1;
        scaleZ = var1;
    }
    else
    {
        scaleX = var1;
        scaleY = var2;
        scaleZ = var3;
    }
    
    updateTransformationMatrixZY();
    updateCollisionModelZY();
    Global::redrawWindow = true;
}

void LINKLINK::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Timer");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Scale X");
    SetWindowTextA(Global::windowLabels[ 9], "Scale Y");
    SetWindowTextA(Global::windowLabels[10], "Scale Z");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "LINKLINK");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(timer).c_str());
    SetWindowTextA(Global::windowValues[ 6], Hex::to_string_short(rotY & 0xFFFF).c_str());
    SetWindowTextA(Global::windowValues[ 7], Hex::to_string_short(rotZ & 0xFFFF).c_str());
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
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "Point bonus trigger");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "How many frames you have until the combo runs out.");
    SetWindowTextA(Global::windowDescriptions[ 6], "Left byte = y rotation (used for cube collision?). Right byte: 4 most significant bits -> counter. 4 least significant bits -> points");
    SetWindowTextA(Global::windowDescriptions[ 7], "Left byte = y rotation (used for cube collision?). Right byte: least significant bit -> is sphere or is cube (1 for sphere)");
    SetWindowTextA(Global::windowDescriptions[ 8], "If a cube, x scale. If a sphere, radius of the sphere.");
    SetWindowTextA(Global::windowDescriptions[ 9], "If a cube, y scale.");
    SetWindowTextA(Global::windowDescriptions[10], "If a cube, z scale.");

    updateTransformationMatrixZY();
    updateCollisionModelZY();
}

void LINKLINK::fillData(char data[32])
{
    data[1] = (char)ID;

    data[2] = (char)((timer >> 8) & 0xFF);
    data[3] = (char)((timer >> 0) & 0xFF);
    data[4] = (char)((rotY  >> 8) & 0xFF);
    data[5] = (char)((rotY  >> 0) & 0xFF);
    data[6] = (char)((rotZ  >> 8) & 0xFF);
    data[7] = (char)((rotZ  >> 0) & 0xFF);

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
