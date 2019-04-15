#include <glad/glad.h>
#include <cstring>

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
	baseColour.set(1,1,1);
	updateTransformationMatrix();

    collideModelOriginal = RING::cmBase;
	collideModelTransformed = RING::cmBase->duplicateMe();
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModel();
}

void RING::step()
{
    if (collideModelTransformed->wasCollidedWith)
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
