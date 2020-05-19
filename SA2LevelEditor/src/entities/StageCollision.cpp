#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagecollision.h"
#include "../loading/loader.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"

#include <list>

std::list<TexturedModel*> StageCollision::models;
CollisionModel* StageCollision::collisionModel;

StageCollision::StageCollision()
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

void StageCollision::step()
{
    visible = Global::displayStageCollision;
    if (StageCollision::collisionModel != nullptr)
    {
        StageCollision::collisionModel->isVisible = visible;
    }
}

std::list<TexturedModel*>* StageCollision::getModels()
{
    return &StageCollision::models;
}

void StageCollision::loadModels(const char* objFolder, const char* objFilename)
{
    if (StageCollision::models.size() == 0)
    {
        #ifdef DEV_MODE
        std::fprintf(stdout, "Loading StageCollision static models...\n");
        #endif

        loadModel(&StageCollision::models, objFolder, objFilename);
    }

    StageCollision::collisionModel = loadCollisionModel(objFolder, objFilename);
    CollisionChecker::addCollideModel(StageCollision::collisionModel);
}

void StageCollision::deleteStaticModels()
{
    #ifdef DEV_MODE
    std::fprintf(stdout, "Deleting StageCollision static models...\n");
    #endif

    Entity::deleteModels(&StageCollision::models);
}
