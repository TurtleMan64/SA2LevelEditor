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


std::list<TexturedModel*> Ring::models;
CollisionModel* Ring::cmBase;

Ring::Ring()
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

Ring::Ring(char data[32])
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
	rotationY = Maths::random()*360.0f;
	rotationZ = 0; 
	scale = 1;
	visible = true;
	baseColour.set(1,1,1);
	updateTransformationMatrix();

    collideModelOriginal = Ring::cmBase;
	collideModelTransformed = Ring::cmBase->duplicateMe();
	CollisionChecker::addCollideModel(collideModelTransformed);
	updateCollisionModelWithScale();
}

void Ring::step()
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

std::list<TexturedModel*>* Ring::getModels()
{
	return &Ring::models;
}

void Ring::loadStaticModels()
{
	if (Ring::models.size() > 0)
	{
		return;
	}

	#ifdef DEV_MODE
	std::fprintf(stdout, "Loading Ring static models...\n");
	#endif

	loadModel(&Ring::models, "res/Models/GlobalObjects/Ring/", "Ring");

    if (Ring::cmBase == nullptr)
	{
		Ring::cmBase = loadCollisionModel("res/Models/GlobalObjects/Ring/", "Ring");
	}
}

void Ring::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting ring static models...\n");
	#endif

	Entity::deleteModels(&Ring::models);
    Entity::deleteCollisionModel(&Ring::cmBase);
}
