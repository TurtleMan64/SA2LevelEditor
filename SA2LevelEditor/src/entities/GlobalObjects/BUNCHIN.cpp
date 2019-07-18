#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "bunchin.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"
#include "../dummy.h"
#include "../unknown.h"

#include <list>



std::list<TexturedModel*> BUNCHIN::models;
CollisionModel* BUNCHIN::cmBase;

BUNCHIN::BUNCHIN()
{

}

void BUNCHIN::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }

	despawnGuides();
}

BUNCHIN::BUNCHIN(char data[32], bool useDefaultValues)
{
	std::memcpy(rawData, data, 32);

	ID = data[1];

	signed short rX;
	signed short rY;
	signed short rZ;

	char* ptr = (char*)(&rX);
	memset(ptr, data[3], 1);
	memset(ptr + 1, data[2], 1);

	ptr = (char*)(&rY);
	memset(ptr, data[5], 1);
	memset(ptr + 1, data[4], 1);

	ptr = (char*)(&rZ);
	memset(ptr, data[7], 1);
	memset(ptr + 1, data[6], 1);

	dropType = (int)rX;
	rotationY = (int)rY;
	switchID = (int)rZ;

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
	scaleX = scaleToUnits(scaleX);

	float var2;
	char* v2 = (char*)&var2;
	v2[3] = data[24];
	v2[2] = data[25];
	v2[1] = data[26];
	v2[0] = data[27];
	weightHeight = var2 + 30.0f;

	char* v3 = (char*)&scaleZ;
	v3[3] = data[28];
	v3[2] = data[29];
	v3[1] = data[30];
	v3[0] = data[31];
	scaleZ = scaleToUnits(scaleZ);

	if (useDefaultValues)
	{
		rotationY = 0;
		dropType = 0;
		weightHeight = 30.0f;
		switchID = 0;
		scaleX = scaleToUnits(0);
		scaleZ = scaleToUnits(0);
	}

	rotationX = 0;
	rotationZ = 0;
	scaleY = 10;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrixYXZ();

	collideModelOriginal = BUNCHIN::cmBase;
	collideModelTransformed = BUNCHIN::cmBase->duplicateMe();
	collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelYXZ();
}

bool BUNCHIN::isSA2Object()
{
	return true;
}

void BUNCHIN::step()
{
	if (Global::selectedSA2Object == this)
	{
		baseColour.set(1.75f, 1.75f, 1.75f);
	}
	else
	{
		if (Global::selectedSA2Object && Global::selectedSA2Object->getSwitchID() == switchID) {
			baseColour.set(1.0f, 1.0f, 2.0f);
		}
		else {
			baseColour.set(1.0f, 1.0f, 1.0f);
		}
		if (guides.size() > 0)
		{
			despawnGuides();
			Global::redrawWindow = true;
		}
	}
}

std::list<TexturedModel*>* BUNCHIN::getModels()
{
	return &BUNCHIN::models;
}

void BUNCHIN::loadStaticModels()
{
	if (BUNCHIN::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading BUNCHIN static models...\n");
	#endif

	loadModel(&BUNCHIN::models, "res/Models/GlobalObjects/Bunchin/", "Unknown");

	if (BUNCHIN::cmBase == nullptr)
	{
		BUNCHIN::cmBase = loadCollisionModel("res/Models/GlobalObjects/Bunchin/", "Unknown");
	}
}

void BUNCHIN::deleteStaticModels()
{
#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting BUNCHIN static models...\n");
#endif

	Entity::deleteModels(&BUNCHIN::models);
	Entity::deleteCollisionModel(&BUNCHIN::cmBase);
}

void BUNCHIN::updateValue(int btnIndex)
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
				char data[32] = { 0 };
				data[1] = (char)newid;

				data[8] = *(((char*)&position.x) + 3);
				data[9] = *(((char*)&position.x) + 2);
				data[10] = *(((char*)&position.x) + 1);
				data[11] = *(((char*)&position.x) + 0);
				data[12] = *(((char*)&position.y) + 3);
				data[13] = *(((char*)&position.y) + 2);
				data[14] = *(((char*)&position.y) + 1);
				data[15] = *(((char*)&position.y) + 0);
				data[16] = *(((char*)&position.z) + 3);
				data[17] = *(((char*)&position.z) + 2);
				data[18] = *(((char*)&position.z) + 1);
				data[19] = *(((char*)&position.z) + 0);

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
			int newRotX = std::stoi(text);
			dropType = newRotX;
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[5], std::to_string(dropType).c_str());
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
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
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
			switchID = newRotZ;
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[7], std::to_string(switchID).c_str());
			break;
		}
		catch (...) { break; }
	}

	case 8:
	{
		try
		{
			float newVar1 = std::stof(text);
			scaleX = scaleToUnits(newVar1);
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[8], std::to_string(newVar1).c_str());
			break;
		}
		catch (...) { break; }
	}

	case 9:
	{
		try
		{
			int newVar2 = std::stoi(text);
			weightHeight = 30.0f + newVar2;
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[9], std::to_string(newVar2).c_str());
			break;
		}
		catch (...) { break; }
	}

	case 10:
	{
		try
		{
			float newVar3 = std::stof(text);
			scaleZ = scaleToUnits(newVar3);
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[10], std::to_string(newVar3).c_str());
			break;
		}
		catch (...) { break; }
	}

	default: break;
	}

	spawnGuides();
}

void BUNCHIN::updateEditorWindows()
{
	SetWindowTextA(Global::windowLabels[0], "ID");
	SetWindowTextA(Global::windowLabels[1], "Name");
	SetWindowTextA(Global::windowLabels[2], "Position X");
	SetWindowTextA(Global::windowLabels[3], "Position Y");
	SetWindowTextA(Global::windowLabels[4], "Position Z");
	SetWindowTextA(Global::windowLabels[5], "Falling Type");
	SetWindowTextA(Global::windowLabels[6], "Rotation Y");
	SetWindowTextA(Global::windowLabels[7], "Switch ID");
	SetWindowTextA(Global::windowLabels[8], "Scale X");
	SetWindowTextA(Global::windowLabels[9], "Drop Height");
	SetWindowTextA(Global::windowLabels[10], "Scale Z");

	SetWindowTextA(Global::windowValues[0], std::to_string(ID).c_str());
	SetWindowTextA(Global::windowValues[1], "BUNCHIN");
	SetWindowTextA(Global::windowValues[2], std::to_string(position.x).c_str());
	SetWindowTextA(Global::windowValues[3], std::to_string(position.y).c_str());
	SetWindowTextA(Global::windowValues[4], std::to_string(position.z).c_str());
	SetWindowTextA(Global::windowValues[5], std::to_string(dropType).c_str());
	SetWindowTextA(Global::windowValues[6], std::to_string(rotationY).c_str());
	SetWindowTextA(Global::windowValues[7], std::to_string(switchID).c_str());
	SetWindowTextA(Global::windowValues[8], std::to_string(unitsToScale(scaleX)).c_str());
	SetWindowTextA(Global::windowValues[9], std::to_string(weightHeight - 30.0f).c_str());
	SetWindowTextA(Global::windowValues[10], std::to_string(unitsToScale(scaleZ)).c_str());

	SendMessageA(Global::windowValues[0], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[1], EM_SETREADONLY, 1, 0);
	SendMessageA(Global::windowValues[2], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[3], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[4], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[5], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[6], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[7], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[8], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[9], EM_SETREADONLY, 0, 0);
	SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

	SetWindowTextA(Global::windowDescriptions[0], "");
	SetWindowTextA(Global::windowDescriptions[1], "Weight Trap");
	SetWindowTextA(Global::windowDescriptions[2], "");
	SetWindowTextA(Global::windowDescriptions[3], "");
	SetWindowTextA(Global::windowDescriptions[4], "");
	SetWindowTextA(Global::windowDescriptions[5], "Type of weight: 0 drops repeatedly, 1 drops when underneath and resets when away, 2 drops when underneath and stays down.");
	SetWindowTextA(Global::windowDescriptions[6], "");
	SetWindowTextA(Global::windowDescriptions[7], "Hitting a SWITCH with RotationX equal to this number will disable the weight.");
	SetWindowTextA(Global::windowDescriptions[8], "Length is equal to 40 + 40 * value.");
	SetWindowTextA(Global::windowDescriptions[9], "Height that the weight rises to above 30 units.");
	SetWindowTextA(Global::windowDescriptions[10], "Length is equal to 40 + 40 * value.");

	updateTransformationMatrixYXZ();
	updateCollisionModelYXZ();
	spawnGuides();
}

void BUNCHIN::despawnGuides()
{
	for (Dummy* guide : guides)
	{
		Global::deleteEntity(guide);
	}
	guides.clear();
	Global::deleteTransparentEntity(topWeight);
	topWeight = nullptr;
}

void BUNCHIN::spawnGuides()
{
	despawnGuides();

	Vector3f pos(&position);

	for (int i = 0; i < 20; i++) //TODO hard coded value for number of frames
	{
		Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
		guide->setPosition(&pos);
		guide->visible = true;
		guide->updateTransformationMatrixYXZ();
		Global::addEntity(guide);
		guides.push_back(guide);

		pos.y += weightHeight / 20;
	}
	topWeight = new Dummy(&models); INCR_NEW("Entity");
	topWeight->setPosition(&pos);
	topWeight->setRotation(rotationX, rotationY, rotationZ);
	topWeight->setScale(scaleX, scaleY, scaleZ);
	topWeight->visible = true;
	topWeight->updateTransformationMatrixYXZ();
	Global::addTransparentEntity(topWeight);

	printf("done\n\n");
}

float BUNCHIN::scaleToUnits(float scale) {
	return 40 + std::max(0.f,scale) * 40;
}

float BUNCHIN::unitsToScale(float units) {
	return (units - 40) / 40.0f;
}

void BUNCHIN::fillData(char data[32])
{
	data[1] = (char)ID;

	data[2] = (char)((dropType >> 8) & 0xFF);
	data[3] = (char)((dropType >> 0) & 0xFF);
	data[4] = (char)((rotationY >> 8) & 0xFF);
	data[5] = (char)((rotationY >> 0) & 0xFF);
	data[6] = (char)((switchID >> 8) & 0xFF);
	data[7] = (char)((switchID >> 0) & 0xFF);

	char* ptr = (char*)(&position.x);
	data[8] = (char)(*(ptr + 3));
	data[9] = (char)(*(ptr + 2));
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

	float var1 = unitsToScale(scaleX);
	ptr = (char*)(&var1);
	data[20] = (char)(*(ptr + 3));
	data[21] = (char)(*(ptr + 2));
	data[22] = (char)(*(ptr + 1));
	data[23] = (char)(*(ptr + 0));

	float var2 = weightHeight - 30.0f;
	ptr = (char*)(&var2);
	data[24] = (char)(*(ptr + 3));
	data[25] = (char)(*(ptr + 2));
	data[26] = (char)(*(ptr + 1));
	data[27] = (char)(*(ptr + 0));

	float var3 = unitsToScale(scaleZ);
	ptr = (char*)(&var3);
	data[28] = (char)(*(ptr + 3));
	data[29] = (char)(*(ptr + 2));
	data[30] = (char)(*(ptr + 1));
	data[31] = (char)(*(ptr + 0));
}
