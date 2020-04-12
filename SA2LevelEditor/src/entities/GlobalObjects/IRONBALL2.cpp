#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ironball2.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>



std::list<TexturedModel*> IRONBALL2::baseModels;
std::list<TexturedModel*> IRONBALL2::rodModels;
std::list<TexturedModel*> IRONBALL2::ballModels;
CollisionModel* IRONBALL2::cmBase;
CollisionModel* IRONBALL2::cmRod;
CollisionModel* IRONBALL2::cmBall;

IRONBALL2::IRONBALL2()
{

}

void IRONBALL2::cleanUp()
{
    if (collideModelTransformed != nullptr)
    {
        CollisionChecker::deleteCollideModel(collideModelTransformed);
        collideModelTransformed = nullptr;
    }
    
    despawnGuides();
    destroyModel();
}

IRONBALL2::IRONBALL2(char data[32], bool useDefaultValues)
{
    std::memcpy(rawData, data, 32);

    ID = data[1];
	signed short rZ;

	char* ptr = (char*)(&rZ);
	memset(ptr, data[7], 1);
	memset(ptr + 1, data[6], 1);

	rotZ = (int)rZ;
	oscillates = rotZ / 2 % 2; //result is either 0 or 1, which are falsy and truthy and the results we want.

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
	radius = var1;

	float var2;
    char* v2 = (char*)&var2;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
	rotSpeed = var2;

	float var3;
	char* v3 = (char*)&var3;
	v3[3] = data[24];
	v3[2] = data[25];
	v3[1] = data[26];
	v3[0] = data[27];
	oscillation = var3;

    if (useDefaultValues)
    {
        radius = 0.0f;
        rotSpeed = 0.0f;
		oscillates = false;
		oscillation = 0.0f;
		rotZ = 0;
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

	//base model
    collideModelOriginal = IRONBALL2::cmBase;
	collideModelTransformed = IRONBALL2::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelYXZ();

	createModel();
}

bool IRONBALL2::isSA2Object()
{
    return true;
}

void IRONBALL2::step()
{
    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
		std::get<0>(*rod)->baseColour.set(1.75f, 1.75f, 1.75f);
		std::get<0>(*ball1)->baseColour.set(1.75f, 1.75f, 1.75f);
		std::get<0>(*ball2)->baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
		std::get<0>(*rod)->baseColour.set(1.0f, 1.0f, 1.0f);
		std::get<0>(*ball1)->baseColour.set(1.0f, 1.0f, 1.0f);
		std::get<0>(*ball2)->baseColour.set(1.0f, 1.0f, 1.0f);
		if (guides.size() > 0)
		{
			despawnGuides();
			Global::redrawWindow = true;
		}
    }
}

std::list<TexturedModel*>* IRONBALL2::getModels()
{
	return &IRONBALL2::baseModels;
}

void IRONBALL2::loadStaticModels()
{
	if (IRONBALL2::baseModels.size() > 0 && IRONBALL2::rodModels.size() > 0 && IRONBALL2::ballModels.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading IRONBALL2 static models...\n");
	#endif

	loadModel(&IRONBALL2::baseModels, "res/Models/GlobalObjects/IronBall2/", "Base");
	loadModel(&IRONBALL2::rodModels,  "res/Models/GlobalObjects/IronBall2/", "Rod");
	loadModel(&IRONBALL2::ballModels, "res/Models/GlobalObjects/IronBall2/", "Ball");

    if (IRONBALL2::cmBase == nullptr)
	{
		IRONBALL2::cmBase = loadCollisionModel("res/Models/GlobalObjects/IronBall2/", "Base");
	}
	if (IRONBALL2::cmRod == nullptr)
	{
		IRONBALL2::cmRod = loadCollisionModel("res/Models/GlobalObjects/IronBall2/", "Rod");
	}
	if (IRONBALL2::cmBall == nullptr)
	{
		IRONBALL2::cmBall = loadCollisionModel("res/Models/GlobalObjects/IronBall2/", "Ball");
	}
}

void IRONBALL2::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting IRONBALL2 static models...\n");
	#endif

	Entity::deleteModels(&IRONBALL2::baseModels);
	Entity::deleteModels(&IRONBALL2::rodModels);
	Entity::deleteModels(&IRONBALL2::ballModels);
    Entity::deleteCollisionModel(&IRONBALL2::cmBase);
	Entity::deleteCollisionModel(&IRONBALL2::cmRod);
	Entity::deleteCollisionModel(&IRONBALL2::cmBall);
}

void IRONBALL2::createModel() {
	float unitRadius = 20 * (radius + 1);
	//Rod
	rod = new std::tuple<Dummy*, CollisionModel*>(); INCR_NEW("tuple");

	Dummy* rodDummy = new Dummy(&rodModels); INCR_NEW("Entity");
	rodDummy->setPosition(&position);
	rodDummy->increasePosition(0, 3.f, 0);
	rodDummy->scaleX = unitRadius;
	rodDummy->visible = true;
	rodDummy->updateTransformationMatrixYXZ();
	Global::addEntity(rodDummy);
	std::get<0>(*rod) = rodDummy;

	CollisionModel* rodCM = IRONBALL2::cmRod->duplicateMe();
	rodCM->parent = this;
    Vector3f pos(position.x,position.y + 3.f,position.z);
	IRONBALL2::cmRod->transformModelYXZ(rodCM, &pos, 0, 0, 0, unitRadius, 1, 1);
	updateCollisionModelYXZ();
	CollisionChecker::addCollideModel(rodCM);
	std::get<1>(*rod) = rodCM;

	//Ball 1 (+X)
	ball1 = new std::tuple<Dummy*, CollisionModel*>(); INCR_NEW("tuple");

	Dummy* ballDummy = new Dummy(&ballModels); INCR_NEW("Entity");
	ballDummy->setPosition(&position);
	ballDummy->increasePosition(unitRadius, 0, 0);
	ballDummy->visible = true;
	ballDummy->updateTransformationMatrixYXZ();
	Global::addEntity(ballDummy);
	std::get<0>(*ball1) = ballDummy;

	CollisionModel* ballCM = IRONBALL2::cmBall->duplicateMe();
	ballCM->parent = this;
    pos.set(position.x + unitRadius, position.y, position.z);
	IRONBALL2::cmBall->transformModelYXZ(ballCM, &pos, 0, 0, 0, 1, 1, 1);
	updateCollisionModelYXZ();
	CollisionChecker::addCollideModel(ballCM);
	std::get<1>(*ball1) = rodCM;

	//Ball 2 (-X)
	ball2 = new std::tuple<Dummy*, CollisionModel*>(); INCR_NEW("tuple");
	ballDummy = new Dummy(&ballModels); INCR_NEW("Entity");
	ballDummy->setPosition(&position);
	ballDummy->increasePosition(-unitRadius, 0, 0);
	ballDummy->visible = true;
	ballDummy->updateTransformationMatrixYXZ();
	Global::addEntity(ballDummy);
	std::get<0>(*ball2) = ballDummy;

	ballCM = IRONBALL2::cmBall->duplicateMe();
	ballCM->parent = this;
    pos.set(position.x - unitRadius, position.y, position.z);
	IRONBALL2::cmBall->transformModelYXZ(ballCM, &pos, 0, 0, 0, 1, 1, 1);
	updateCollisionModelYXZ();
	CollisionChecker::addCollideModel(ballCM);
	std::get<1>(*ball2) = rodCM;
}

void IRONBALL2::destroyModel() {
	Global::deleteEntity(std::get<0>(*rod));
	Global::deleteEntity(std::get<0>(*ball1));
	Global::deleteEntity(std::get<0>(*ball2));

	CollisionChecker::deleteCollideModel(std::get<1>(*rod));
	CollisionChecker::deleteCollideModel(std::get<1>(*ball1));
	CollisionChecker::deleteCollideModel(std::get<1>(*ball2));

	delete rod;   INCR_DEL("tuple");
	delete ball1; INCR_DEL("tuple");
	delete ball2; INCR_DEL("tuple");
}

void IRONBALL2::spawnGuides() {
	despawnGuides();

	if (oscillates) { //Spawns guides to show oscillation
		Vector3f pos(&position);
		pos.y -= oscillation;

		for (int i = 0; i < 10; i++) //TODO hard coded value for number of frames
		{
			Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
			guide->setPosition(&pos);
			guide->visible = true;
			guide->updateTransformationMatrixYXZ();
			Global::addEntity(guide);
			guides.push_back(guide);

			pos.y += oscillation / 5;
		}
	}

	//Spawns cylinder to show range of motion
	swingCylinder = new Dummy(&Unknown::modelsTriggerCylinder); INCR_NEW("Entity");
	swingCylinder->setPosition(&position);
	swingCylinder->setScale(std::get<0>(*rod)->scaleX + 13.5f, 13.5f, std::get<0>(*rod)->scaleX + 13.5f);
	swingCylinder->visible = true;
	swingCylinder->updateTransformationMatrixYXZ();
	Global::addTransparentEntity(swingCylinder);
}

void IRONBALL2::despawnGuides() {
	for (Dummy* guide : guides)
	{
		Global::deleteEntity(guide);
	}
	guides.clear();
	Global::deleteTransparentEntity(swingCylinder);
	swingCylinder = nullptr;
}

void IRONBALL2::updateValue(int btnIndex)
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

	case 7:
	{
		try
		{
			int newRotZ = std::stoi(text);
			rotZ = newRotZ;
			oscillates = (int)rotZ / 2 % 2;
			updateTransformationMatrixYXZ();
			SetWindowTextA(Global::windowValues[4], std::to_string(position.z).c_str());
			break;
		}
		catch (...) { break; }
	}

    case 8:
    {
        try
        {
            float newRadius = std::stof(text);
			radius = newRadius;
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[8], std::to_string(radius).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newSpeed = std::stof(text);
			rotSpeed = newSpeed;
            SetWindowTextA(Global::windowValues[9], std::to_string(rotSpeed).c_str());
            break;
        }
        catch (...) { break; }
    }

	case 10:
	{
		try
		{
			float newOscillation = std::stof(text);
			oscillation = newOscillation;
			updateTransformationMatrixYXZ();
			updateCollisionModelYXZ();
			Global::redrawWindow = true;
			SetWindowTextA(Global::windowValues[9], std::to_string(rotSpeed).c_str());
			break;
		}
		catch (...) { break; }
	}

    default: break;
    }

	destroyModel();
	createModel();
	spawnGuides();
}

void IRONBALL2::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "");
    SetWindowTextA(Global::windowLabels[ 6], "");
    SetWindowTextA(Global::windowLabels[ 7], "Oscillates?");
    SetWindowTextA(Global::windowLabels[ 8], "Rod Radius");
    SetWindowTextA(Global::windowLabels[ 9], "Turn Speed");
    SetWindowTextA(Global::windowLabels[10], "Oscillation");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "IRONBALL2");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], "");
    SetWindowTextA(Global::windowValues[ 6], "");
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotZ).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(radius).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(rotSpeed).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(oscillation).c_str());

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "Spinning Spike Balls");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "If value / 2 is odd, the object will oscillate.\nCurrent value: " + oscillates);
    SetWindowTextA(Global::windowDescriptions[ 8], "Radius of rod holding the spikes. Value is additive, meaning that -1 causes the spike balls to converge into a single ball.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Value is additive, meaning that -1 counters the default value and makes it not move. Default rotation is clockwise.");
    SetWindowTextA(Global::windowDescriptions[10], "Oscillation amount, in game units.");

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
	spawnGuides();
}

void IRONBALL2::fillData(char data[32])
{
    data[1] = (char)ID;

	data[6] = (char)((rotZ >> 8) & 0xFF);
	data[7] = (char)((rotZ >> 0) & 0xFF);

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

    float var1 = radius;
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = rotSpeed;
    ptr = (char*)(&var2);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

	float var3 = oscillation;
	ptr = (char*)(&var3);
	data[28] = (char)(*(ptr + 3));
	data[29] = (char)(*(ptr + 2));
	data[30] = (char)(*(ptr + 1));
	data[31] = (char)(*(ptr + 0));
}
