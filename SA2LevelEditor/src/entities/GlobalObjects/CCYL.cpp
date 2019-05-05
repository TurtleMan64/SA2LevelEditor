#include <glad/glad.h>
#include <cstring>
#include <Windows.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ccyl.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../loading/levelloader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>



std::list<TexturedModel*> CCYL::models;
CollisionModel* CCYL::cmBase;

CCYL::CCYL()
{

}

CCYL::CCYL(char data[32], bool useDefaultValues)
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

    char* v1 = (char*)&radius;
    v1[3] = data[20];
    v1[2] = data[21];
    v1[1] = data[22];
    v1[0] = data[23];
    radius += 10;

    char* v2 = (char*)&height;
    v2[3] = data[24];
    v2[2] = data[25];
    v2[1] = data[26];
    v2[0] = data[27];
    height += 10;

    if (useDefaultValues)
    {
        radius = 20.0f;
        height = 20.0f;
    }

    scaleX = radius;
    scaleZ = radius;
    scaleY = height;
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
	visible = true;
	baseColour.set(1, 1, 1);
	updateTransformationMatrix();

    collideModelOriginal = CCYL::cmBase;
	collideModelTransformed = CCYL::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

bool CCYL::isSA2Object()
{
    return true;
}

void CCYL::step()
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

std::list<TexturedModel*>* CCYL::getModels()
{
	return &CCYL::models;
}

void CCYL::loadStaticModels()
{
	if (CCYL::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading CCYL static models...\n");
	#endif

	loadModel(&CCYL::models, "res/Models/GlobalObjects/Collision/", "Cylinder");

    if (CCYL::cmBase == nullptr)
	{
		CCYL::cmBase = loadCollisionModel("res/Models/GlobalObjects/Collision/", "Cylinder");
	}
}

void CCYL::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting CCYL static models...\n");
	#endif

	Entity::deleteModels(&CCYL::models);
    Entity::deleteCollisionModel(&CCYL::cmBase);
}

void CCYL::updateValue(int btnIndex)
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

    case 8:
    {
        try
        {
            float newRadius = std::stof(text);
            if (sqrtf(newRadius*newRadius + height*height) > 390.0f)
            {
                MessageBox(NULL, "Warning: The total size of the cylinder is too large, object may not function properly in SA2.", "Warning", MB_OK);
            }
            radius = fmaxf(0.0f, newRadius);
            scaleX = radius;
            scaleZ = radius;
            updateTransformationMatrix();
            updateCollisionModel();
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
            float newHeight = std::stof(text);
            if (sqrtf(newHeight*newHeight + radius*radius) > 390.0f)
            {
                MessageBox(NULL, "Warning: The total size of the cylinder is too large, object may not function properly in SA2.", "Warning", MB_OK);
            }
            height = fmaxf(0.0f, newHeight);
            scaleY = height;
            updateTransformationMatrix();
            updateCollisionModel();
            Global::redrawWindow = true;
            SetWindowTextA(Global::windowValues[9], std::to_string(height).c_str());
            break;
        }
        catch (...) { break; }
    }

    default: break;
    }
}

void CCYL::updateEditorWindows()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "");
    SetWindowTextA(Global::windowLabels[ 6], "");
    SetWindowTextA(Global::windowLabels[ 7], "");
    SetWindowTextA(Global::windowLabels[ 8], "Radius");
    SetWindowTextA(Global::windowLabels[ 9], "Height");
    SetWindowTextA(Global::windowLabels[10], "");

    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "CCYL");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], "");
    SetWindowTextA(Global::windowValues[ 6], "");
    SetWindowTextA(Global::windowValues[ 7], "");
    SetWindowTextA(Global::windowValues[ 8], std::to_string(radius).c_str());
    SetWindowTextA(Global::windowValues[ 9], std::to_string(height).c_str());
    SetWindowTextA(Global::windowValues[10], "");

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
    SetWindowTextA(Global::windowDescriptions[ 1], "Invisible Collision Cylinder. Can be stood on. Rubbing against the side will put Sonic in the \"push\" animation.");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "Radius of cylinder");
    SetWindowTextA(Global::windowDescriptions[ 9], "Height of cylinder (from center)");
    SetWindowTextA(Global::windowDescriptions[10], "");

    updateTransformationMatrix();
    updateCollisionModel();
}

void CCYL::fillData(char data[32])
{
    data[1] = (char)ID;

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

    float var1 = radius-10.0f;
    ptr = (char*)(&var1);
    data[20] = (char)(*(ptr + 3));
    data[21] = (char)(*(ptr + 2));
    data[22] = (char)(*(ptr + 1));
    data[23] = (char)(*(ptr + 0));

    float var2 = height-10.0f;
    ptr = (char*)(&var2);
    data[24] = (char)(*(ptr + 3));
    data[25] = (char)(*(ptr + 2));
    data[26] = (char)(*(ptr + 1));
    data[27] = (char)(*(ptr + 0));
}
