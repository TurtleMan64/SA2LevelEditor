#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ring.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>



std::list<TexturedModel*> RING::models;
CollisionModel* RING::cmBase;

RING::RING()
{

}

//Ring::Ring(Vector3f* p)
//{
//    position.set(p);
//    scale = 1;
//	visible = true;
//	baseColour.set(1,1,1);
//	updateTransformationMatrix();
//}

RING::RING(char data[32])
{
    std::memcpy(rawData, data, 32);

    ID = data[1];

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

	rotationX = 0;
	rotationY = (int)Maths::random()*65536;
	rotationZ = 0; 
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrix();

    collideModelOriginal = RING::cmBase;
	collideModelTransformed = RING::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

bool RING::isSA2Object()
{
    return true;
}

void RING::step()
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

std::list<TexturedModel*>* RING::getModels()
{
	return &RING::models;
}

void RING::loadStaticModels()
{
	if (RING::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading RING static models...\n");
	#endif

	loadModel(&RING::models, "res/Models/GlobalObjects/Ring/", "Ring");

    if (RING::cmBase == nullptr)
	{
		RING::cmBase = loadCollisionModel("res/Models/GlobalObjects/Ring/", "Ring");
	}
}

void RING::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting RING static models...\n");
	#endif

	Entity::deleteModels(&RING::models);
    Entity::deleteCollisionModel(&RING::cmBase);
}

void RING::updateValue(int btnIndex)
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
    
    default: break;
    }
}

void RING::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "");
    SetWindowTextA(Global::windowLabels[ 6], "");
    SetWindowTextA(Global::windowLabels[ 7], "");
    SetWindowTextA(Global::windowLabels[ 8], "");
    SetWindowTextA(Global::windowLabels[ 9], "");
    SetWindowTextA(Global::windowLabels[10], "");

    SetWindowTextA(Global::windowValues[0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[1], "RING");
    SetWindowTextA(Global::windowValues[2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], "");
    SetWindowTextA(Global::windowValues[ 6], "");
    SetWindowTextA(Global::windowValues[ 7], "");
    SetWindowTextA(Global::windowValues[ 8], "");
    SetWindowTextA(Global::windowValues[ 9], "");
    SetWindowTextA(Global::windowValues[10], "");

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 1, 0);
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
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");
}

void RING::fillData(char data[32])
{
    data[0] = 0x00; //S has 0 as the second byte, U has 0x80. we will export every object to S file
    data[1] = (char)ID;

    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;

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

    data[20] = 0;
    data[21] = 0;
    data[22] = 0;
    data[23] = 0;
    data[24] = 0;
    data[25] = 0;
    data[26] = 0;
    data[27] = 0;
    data[28] = 0;
    data[29] = 0;
    data[30] = 0;
    data[31] = 0;
}
