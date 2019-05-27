#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "rocket.h"
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



std::list<TexturedModel*> ROCKET::models;
CollisionModel* ROCKET::cmBase;

ROCKET::ROCKET()
{

}

ROCKET::ROCKET(char data[32], bool useDefaultValues)
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

    needsSwitch = (bool)rX;
    ogRotY      = (int)rY;
    switchID    = (int)rZ;

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

    char* v1 = (char*)&endX;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];

    char* v2 = (char*)&endY;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];

    char* v3 = (char*)&endZ;
    v3[3] = data[28];
    v3[2] = data[29];
    v3[1] = data[30];
    v3[0] = data[31];

    if (useDefaultValues)
    {
        rotationX = 0;
        rotationY = 0;
        rotationZ = 0;
        needsSwitch = false;
        switchID = 0;
        endX = position.x+100;
        endY = position.y;
        endZ = position.z;
    }

    rotationX = 0;
    calculateRotY();
    rotationZ = 0;
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrixYXZ();

    collideModelOriginal = ROCKET::cmBase;
	collideModelTransformed = ROCKET::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelYXZ();
}

void ROCKET::calculateRotY()
{
    Vector3f end(endX, endY, endZ);
    Vector3f diff = end-position;

    float yawRadians = atan2f(-diff.z, diff.x);
    float yawDeg = Maths::toDegrees(yawRadians)-90.0f;
    rotationY = Maths::degToBams(yawDeg);
}

bool ROCKET::isSA2Object()
{
    return true;
}

void ROCKET::step()
{
    if (Global::selectedSA2Object == this)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
        if (guides.size() > 0)
        {
            despawnGuides();
            Global::redrawWindow = true;
        }
    }
}

std::list<TexturedModel*>* ROCKET::getModels()
{
	return &ROCKET::models;
}

void ROCKET::loadStaticModels()
{
	if (ROCKET::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading ROCKET static models...\n");
	#endif

	loadModel(&ROCKET::models, "res/Models/GlobalObjects/Rocket/", "Rocket");

    if (ROCKET::cmBase == nullptr)
	{
		ROCKET::cmBase = loadCollisionModel("res/Models/GlobalObjects/Rocket/", "Rocket");
	}
}

void ROCKET::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting ROCKET static models...\n");
	#endif

	Entity::deleteModels(&ROCKET::models);
    Entity::deleteCollisionModel(&ROCKET::cmBase);
}

void ROCKET::updateValue(int btnIndex)
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
                    despawnGuides();
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
            int newNeedsSwitch = std::stoi(text);
            needsSwitch = (bool)newNeedsSwitch;
            SetWindowTextA(Global::windowValues[5], std::to_string((int)needsSwitch).c_str());
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
            int newSwitchID = std::stoi(text);
            switchID = newSwitchID;
            SetWindowTextA(Global::windowValues[7], std::to_string(switchID).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 8:
    {
        try
        {
            float newEndX = std::stof(text);
            endX = newEndX;
            SetWindowTextA(Global::windowValues[8], std::to_string(endX).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 9:
    {
        try
        {
            float newEndY = std::stof(text);
            endY = newEndY;
            SetWindowTextA(Global::windowValues[9], std::to_string(endY).c_str());
            break;
        }
        catch (...) { break; }
    }

    case 10:
    {
        try
        {
            float newEndZ = std::stof(text);
            endZ = newEndZ;
            SetWindowTextA(Global::windowValues[10], std::to_string(endZ).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }

    calculateRotY();
    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    Global::redrawWindow = true;

    spawnGuides();
}

void ROCKET::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Switch Activated");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Switch ID");
    SetWindowTextA(Global::windowLabels[ 8], "End X");
    SetWindowTextA(Global::windowLabels[ 9], "End Y");
    SetWindowTextA(Global::windowLabels[10], "End Z");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "ROCKET");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string((int)needsSwitch).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(ogRotY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(switchID).c_str());
    SetWindowTextA(Global::windowValues[ 8], std::to_string(endX).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(endY).c_str());
    SetWindowTextA(Global::windowValues[10], std::to_string(endZ).c_str());

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 0, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 0, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "Rocket that transports the player.");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "Set to 1 to make the rocket require a switch to turn on.");
    SetWindowTextA(Global::windowDescriptions[ 6], "Doesn't actually affect rotation, because rotation is automatically calculated to point to the drop-off.");
    SetWindowTextA(Global::windowDescriptions[ 7], "An ID that tells the game which switch needs to be turned on to activate this rocket.");
    SetWindowTextA(Global::windowDescriptions[ 8], "X position of drop-off.");
    SetWindowTextA(Global::windowDescriptions[ 9], "Y position of drop-off.");
    SetWindowTextA(Global::windowDescriptions[10], "Z position of drop-off.");

    calculateRotY();
    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    spawnGuides();
}


void ROCKET::despawnGuides()
{
    for (Dummy* guide : guides)
    {
        Global::deleteEntity(guide);
    }
    guides.clear();
}

void ROCKET::spawnGuides()
{
    despawnGuides();
    
    Vector3f pos(&position);
    Vector3f end(endX, endY, endZ);

    Vector3f diff = end-pos;
    diff.scale(1/50.0f);
    
    for (int i = 0; i <= 50; i++)
    {
        Dummy* guide = new Dummy(&Unknown::modelsGuide); INCR_NEW("Entity");
        guide->setPosition(&pos);
        guide->visible = true;
        guide->updateTransformationMatrixYXZ();
        Global::addEntity(guide);
        guides.push_back(guide);
    
        pos = pos + diff;
    }
}

void ROCKET::fillData(char data[32])
{
    data[1] = (char)ID;

    data[2] = (char)(((int)needsSwitch >> 8) & 0xFF);
    data[3] = (char)(((int)needsSwitch >> 0) & 0xFF);
    data[4] = (char)((ogRotY           >> 8) & 0xFF);
    data[5] = (char)((ogRotY           >> 0) & 0xFF);
    data[6] = (char)((switchID         >> 8) & 0xFF);
    data[7] = (char)((switchID         >> 0) & 0xFF);

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

    ptr = (char*)(&endX);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    ptr = (char*)(&endY);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));

    ptr = (char*)(&endZ);
    data[28] = (char)(*(ptr + 3));
    data[29] = (char)(*(ptr + 2));
    data[30] = (char)(*(ptr + 1));
    data[31] = (char)(*(ptr + 0));
}
