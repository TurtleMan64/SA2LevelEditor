#include <glad/glad.h>

#include "entity.h"
#include "../toolbox/vector.h"
#include "unknown.h"
#include "../models/texturedmodel.h"
#include "../loading/objLoader.h"
#include "../main/main.h"
#include "../collision/collisionchecker.h"
#include "../collision/collisionmodel.h"
#include "../toolbox/maths.h"
#include <Windows.h>

#include <list>


std::vector<std::list<TexturedModel*>> Unknown::models;

CollisionModel* Unknown::cmBase;


Unknown::Unknown()
{

}

Unknown::Unknown(char data[32])
{
    //std::fprintf(stdout, "ring\n");
    //std::memcpy(rawData, data, 32);
    //memcpy(&position.x, &data[8],  4);
    //memcpy(&position.y, &data[12], 4);
    //memcpy(&position.z, &data[16], 4);
    id = data[1];

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

	scaleX = 12;
    scaleY = 12;
    scaleZ = 12;
	visible = true;
	baseColour.set(1,1,1);
	updateTransformationMatrix();

    collideModelOriginal = Unknown::cmBase;
	collideModelTransformed = Unknown::cmBase->duplicateMe();
    collideModelTransformed->parent = this;
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

void Unknown::step()
{
    collideModelTransformed->isVisible = visible;

    if (collideModelTransformed->wasCollidedWith)
    {
        baseColour.set(1.75f, 1.75f, 1.75f);
    }
    else
    {
        baseColour.set(1.0f, 1.0f, 1.0f);
    }
}

std::list<TexturedModel*>* Unknown::getModels()
{
    if (id > 109)
    {
        return &Unknown::models[0];
    }

	return &Unknown::models[id];
}

void Unknown::loadStaticModels()
{
	if (Unknown::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading Unknown static models...\n");
	#endif

    for (int i = 0; i <= 109; i++)
    {
        std::list<TexturedModel*> list;
        Unknown::models.push_back(list);
    }

	loadObjModel(&Unknown::models[0], "res/Models/GlobalObjects/Unknown/", "Unknown.obj");

    for (int i = 1; i <= 109; i++)
    {
        std::string s = "Unknown"+std::to_string(i);
        s = s + ".obj";
	    loadObjModel(&Unknown::models[i], "res/Models/GlobalObjects/Unknown/", s);
    }

    if (Unknown::cmBase == nullptr)
	{
		Unknown::cmBase = loadCollisionModel("res/Models/GlobalObjects/Unknown/", "Unknown");
	}
}

void Unknown::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting Unknown static models...\n");
	#endif

    for (int i = 0; i < (int)Unknown::models.size(); i++)
    {
	    Entity::deleteModels(&Unknown::models[i]);
    }
    Unknown::models.clear();
    Entity::deleteCollisionModel(&Unknown::cmBase);
}

void Unknown::updateEditorWindows()
{
    SetWindowTextA(Global::windowValues[ 0], std::to_string(ID).c_str());
    SetWindowTextA(Global::windowValues[ 1], "Unknown");
    SetWindowTextA(Global::windowValues[ 2], std::to_string(position.x).c_str());
    SetWindowTextA(Global::windowValues[ 3], std::to_string(position.y).c_str());
    SetWindowTextA(Global::windowValues[ 4], std::to_string(position.z).c_str());
    SetWindowTextA(Global::windowValues[ 5], std::to_string(rotationX).c_str());
    SetWindowTextA(Global::windowValues[ 6], std::to_string(rotationY).c_str());
    SetWindowTextA(Global::windowValues[ 7], std::to_string(rotationZ).c_str());
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
}
