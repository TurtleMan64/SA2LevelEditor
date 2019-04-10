#include <glad/glad.h>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ring.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../main/main.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>


std::list<TexturedModel*> Ring::models;


Ring::Ring()
{

}

Ring::Ring(char data[32])
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
	rotationY = (float)(((int)(this))%360);
	rotationZ = 0; 
	scale = 1;
	visible = true;
	baseColour.set(1,1,1);
	updateTransformationMatrix();
}

void Ring::step()
{

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
	std::fprintf(stdout, "Loading ring static models...\n");
	#endif

	loadObjModel(&Ring::models, "res/Models/GlobalObjects/Ring/", "Ring.obj");
}

void Ring::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting ring static models...\n");
	#endif

	Entity::deleteModels(&Ring::models);
}
