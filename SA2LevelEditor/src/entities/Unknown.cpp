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

#include <list>


std::vector<std::list<TexturedModel*>> Unknown::models;

CollisionModel* Unknown::cmBase;


Unknown::Unknown()
{

}

Unknown::Unknown(char data[32], int id)
{
    //std::fprintf(stdout, "ring\n");
    //std::memcpy(rawData, data, 32);
    //memcpy(&position.x, &data[8],  4);
    //memcpy(&position.y, &data[12], 4);
    //memcpy(&position.z, &data[16], 4);

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
	rotationY = 0;
	rotationZ = 0; 
	scale = 12;
	visible = true;
	baseColour.set(1,1,1);
	updateTransformationMatrix();

    this->id = id;



    collideModelOriginal = Unknown::cmBase;
	collideModelTransformed = Unknown::cmBase->duplicateMe();
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelWithScale();
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
