#include <list>

#include "../toolbox/vector.h"
#include "cameratrigger.h"
#include "../toolbox/maths.h"
#include "../loading/objLoader.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"
#include "dummy.h"

std::list<TexturedModel*> CameraTrigger::modelsSphere;
std::list<TexturedModel*> CameraTrigger::modelsCube;
CollisionModel* CameraTrigger::cmBaseCube = nullptr;
CollisionModel* CameraTrigger::cmBaseSphere = nullptr;

CameraTrigger::CameraTrigger(int type,
                  float x,  float y,  float z,
                  float xr, float yr, float zr, 
                  float xs, float ys, float zs,
                  float ax, float ay, float az,
                  float bx, float by, float bz)
{
    CameraTrigger::loadStaticModels();

    triggerType = type;
    position.x = x;
    position.y = y;
    position.z = z;
    rotationX = Maths::degToBams(xr);
    rotationY = Maths::degToBams(yr);
    rotationZ = Maths::degToBams(zr);
    scaleX = xs;
    scaleY = ys;
    scaleZ = zs;

    visible = Global::displayCameraTriggers;
    if (triggerType == 1)
    {
        scaleY = xs;
        scaleZ = xs;

        collideModelOriginal = CameraTrigger::cmBaseSphere;
    }
    else
    {
        collideModelOriginal = CameraTrigger::cmBaseCube;
    }
    collideModelTransformed = collideModelOriginal->duplicateMe();
    collideModelTransformed->parent = nullptr;
    CollisionChecker::addCollideModel(collideModelTransformed);

    pointA = new Dummy(&modelsSphere); INCR_NEW("Entity");
    pointB = new Dummy(&modelsSphere); INCR_NEW("Entity");
    pointA->position.set(ax, ay, az);
    pointB->position.set(bx, by, bz);
    pointA->setScale(12, 12, 12);
    pointB->setScale(12, 12, 12);
    pointA->visible = false;
    pointB->visible = false;
    pointA->baseColour.set(2.0f, 0.0f, 0.0f);
    pointB->baseColour.set(0.0f, 0.0f, 2.0f);
    pointA->updateTransformationMatrixYXZ();
    pointB->updateTransformationMatrixYXZ();
    Global::addTransparentEntity(pointA);
    Global::addTransparentEntity(pointB);

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    baseColour.set(1.0f, 0.5f, 0.0f);
}

void CameraTrigger::step()
{
    visible = Global::displayCameraTriggers;
    collideModelTransformed->isVisible = visible;

    if (collideModelTransformed->wasCollidedWith)
    {
        baseColour.set(2.0f, 1.0f, 0.0f);
        pointA->visible = true;
        pointB->visible = true;
    }
    else
    {
        baseColour.set(1.0f, 0.5f, 0.0f);
        pointA->visible = false;
        pointB->visible = false;
    }
}

std::list<TexturedModel*>* CameraTrigger::getModels()
{
    switch (triggerType)
    {
        case 1:
            return &CameraTrigger::modelsSphere;

        default:
            break;
    }

	return &CameraTrigger::modelsCube;
}

void CameraTrigger::loadStaticModels()
{
    if (CameraTrigger::modelsCube.size() == 0)
	{
		#ifdef DEV_MODE
	    std::fprintf(stdout, "Loading CameraTrigger cube static models...\n");
	    #endif

	    loadModel(&CameraTrigger::modelsCube, "res/Models/GlobalObjects/Trigger/", "TriggerCube");
	}

    if (CameraTrigger::modelsSphere.size() == 0)
	{
		#ifdef DEV_MODE
	    std::fprintf(stdout, "Loading CameraTrigger sphere static models...\n");
	    #endif

	    loadModel(&CameraTrigger::modelsSphere, "res/Models/GlobalObjects/Trigger/", "TriggerSphere");
	}

    if (CameraTrigger::cmBaseCube == nullptr)
	{
		CameraTrigger::cmBaseCube = loadCollisionModel("res/Models/GlobalObjects/Trigger/", "TriggerCube");
	}

    if (CameraTrigger::cmBaseSphere == nullptr)
	{
		CameraTrigger::cmBaseSphere = loadCollisionModel("res/Models/GlobalObjects/Trigger/", "TriggerSphere");
	}
}
