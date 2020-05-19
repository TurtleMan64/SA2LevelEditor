#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagekillplanes.h"
#include "../loading/loader.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"

#include <list>

std::list<TexturedModel*> StageKillplanes::models;
CollisionModel* StageKillplanes::collisionModel;

StageKillplanes::StageKillplanes()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
    baseColour.set(0.70f, 0.70f, 0.70f);
	
	updateTransformationMatrixYXZ();
}

void StageKillplanes::step()
{
    visible = Global::displayStageKillplanes;
    if (StageKillplanes::collisionModel != nullptr)
    {
        StageKillplanes::collisionModel->isVisible = visible;
    }
}

std::list<TexturedModel*>* StageKillplanes::getModels()
{
	return &StageKillplanes::models;
}

void StageKillplanes::loadModels(const char* objFolder, const char* objFilename)
{
    if (StageKillplanes::models.size() == 0)
	{
		#ifdef DEV_MODE
		std::fprintf(stdout, "Loading StageKillplanes static models...\n");
		#endif

		loadModel(&StageKillplanes::models, objFolder, objFilename);
	}

	StageKillplanes::collisionModel = loadCollisionModel(objFolder, objFilename);
    CollisionChecker::addCollideModel(StageKillplanes::collisionModel);
}

void StageKillplanes::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting StageKillplanes static models...\n");
	#endif

	Entity::deleteModels(&StageKillplanes::models);
}
