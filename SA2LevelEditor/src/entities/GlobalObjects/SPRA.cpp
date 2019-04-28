#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "spra.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>



std::list<TexturedModel*> SPRA::models;
CollisionModel* SPRA::cmBase;

SPRA::SPRA()
{

}

SPRA::SPRA(char data[32])
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

    rotationX = data[3] + (data[2] << 8);
    rotationY = data[5] + (data[4] << 8);
    rotationZ = data[7] + (data[6] << 8);

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

    char* v3 = (char*)&var3;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];

	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrix();

    collideModelOriginal = SPRA::cmBase;
	collideModelTransformed = SPRA::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

bool SPRA::isSA2Object()
{
    return true;
}

void SPRA::step()
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

std::list<TexturedModel*>* SPRA::getModels()
{
	return &SPRA::models;
}

void SPRA::loadStaticModels()
{
	if (SPRA::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading SPRA static models...\n");
	#endif

	loadModel(&SPRA::models, "res/Models/GlobalObjects/Spring/", "SpringA");

    if (SPRA::cmBase == nullptr)
	{
		SPRA::cmBase = loadCollisionModel("res/Models/GlobalObjects/Spring/", "SpringA");
	}
}

void SPRA::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting SPRA static models...\n");
	#endif

	Entity::deleteModels(&SPRA::models);
    Entity::deleteCollisionModel(&SPRA::cmBase);
}

void SPRA::updateValue(int btnIndex)
{
    char buf[128];
    GetWindowTextA(Global::windowValues[btnIndex], buf, 128);
    std::string text = buf;

    switch (btnIndex)
    {
    case 0:
    {
        //we are going to change into a new object. deal with this later.
        break;
    }

    case 2:
    {
        try
        {
            float newX = std::stof(text);
            position.x = newX;
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
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
            updateTransformationMatrix();
            updateCollisionModel();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[10], std::to_string(var3).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }
}

void SPRA::updateEditorWindows()
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
    SetWindowTextA(Global::windowValues[ 1], "SPRA");
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
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "Time (in frames) that controls are locked.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Multiplier of spring's power.");
    SetWindowTextA(Global::windowDescriptions[10], "");
}

void SPRA::fillData(char data[32])
{
    data[0] = 0x00; //S has 0 as the second byte, U has 0x80. we will export every object to S file
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

    ptr = (char*)(&power);
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
