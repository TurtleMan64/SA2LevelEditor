#include <glad/glad.h>
#include <cstring>

#include "../entity.h"
#include "../../toolbox/vector.h"
#include "ringlinear.h"
#include "ring.h"
#include "../../models/texturedmodel.h"
#include "../../loading/objLoader.h"
#include "../../main/main.h"
#include "../../collision/collisionmodel.h"
#include "../../collision/collisionchecker.h"
#include "../../toolbox/maths.h"

#include <list>


//std::list<TexturedModel*> RING_LINEAR::models;
//CollisionModel* RING_LINEAR::cmBase;

RING_LINEAR::RING_LINEAR()
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

RING_LINEAR::RING_LINEAR(char data[32])
{
    std::memcpy(rawData, data, 32);

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

    float var1;
    char* d = (char*)&var1;
    d[3] = data[20];
    d[2] = data[21];
    d[1] = data[22];
    d[0] = data[23];

    float var3;
    char* n = (char*)&var3;
    n[3] = data[28];
    n[2] = data[29];
    n[1] = data[30];
    n[0] = data[31];

    numRings = (int)var3;
    ringDelta = var1 + 10.0f;

    Vector3f ringDirection(0, 0, 1);

    Vector3f xAxis(-1, 0, 0);
    Vector3f yAxis(0, -1, 0);
    ringDirection = Maths::rotatePoint(&ringDirection, &xAxis, Maths::toRadians(rotationX));
    ringDirection = Maths::rotatePoint(&ringDirection, &yAxis, Maths::toRadians(rotationY));

    ringDirection.setLength(ringDelta);

    for (int i = 0; i < numRings; i++)
    {
        Vector3f currOff = ringDirection.scaleCopy((float)i);
        Vector3f ringPos = position + currOff;

        Dummy* ring = new Dummy(&RING::models); INCR_NEW("Entity")
        ring->setPosition(&ringPos);
        ring->visible = true;
        ring->updateTransformationMatrix();
        Global::addEntity(ring);

        CollisionModel* cm = RING::cmBase->duplicateMe();
        RING::cmBase->transformModel(cm, &ring->position, 0, 0, 0, 1, 1, 1);
        CollisionChecker::addCollideModel(cm);

        rings.push_back(ring);
        cms.push_back(cm);
    }

	rotationX = 0;
	rotationY = (int)Maths::random()*65536;
	rotationZ = 0; 
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = false;
	baseColour.set(1,1,1);
	updateTransformationMatrix();
}

void RING_LINEAR::step()
{
    bool isTouched = false;
    for (CollisionModel* cm : cms)
    {
        if (cm->wasCollidedWith)
        {
            isTouched = true;
            break;
        }
    }

    if (isTouched)
    {
        for (Dummy* ring : rings)
        {
            ring->setBaseColour(1.75f, 1.75f, 1.75f);
        }
    }
    else
    {
        for (Dummy* ring : rings)
        {
            ring->setBaseColour(1.0f, 1.0f, 1.0f);
        }
    }
}

std::list<TexturedModel*>* RING_LINEAR::getModels()
{
	return nullptr; //our children are visible, not us
}

void RING_LINEAR::loadStaticModels()
{
	//if (RING_LINEAR::models.size() > 0)
	//{
	//	return;
	//}
    //
	//#ifdef DEV_MODE
	//std::fprintf(stdout, "Loading RING_LINEAR static models...\n");
	//#endif
    //
	//loadModel(&RING_LINEAR::models, "res/Models/GlobalObjects/Ring/", "Ring");
    //
    //if (RING_LINEAR::cmBase == nullptr)
	//{
	//	RING_LINEAR::cmBase = loadCollisionModel("res/Models/GlobalObjects/Ring/", "Ring");
	//}
}

void RING_LINEAR::deleteStaticModels()
{
	//#ifdef DEV_MODE
	//std::fprintf(stdout, "Deleting RING_LINEAR static models...\n");
	//#endif
    //
	//Entity::deleteModels(&RING_LINEAR::models);
    //Entity::deleteCollisionModel(&RING_LINEAR::cmBase);
}
