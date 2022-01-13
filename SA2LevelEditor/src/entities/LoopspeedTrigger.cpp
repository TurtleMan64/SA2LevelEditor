#include <list>

#include "../toolbox/vector.h"
#include "loopspeedtrigger.h"
#include "../toolbox/maths.h"
#include "../loading/objLoader.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"
#include "../collision/collisionchecker.h"
#include "dummy.h"

std::list<TexturedModel*> LoopspeedTrigger::modelsSphere;
CollisionModel* LoopspeedTrigger::cmBaseSphere = nullptr;

LoopspeedTrigger::LoopspeedTrigger(float x,  float y,  float z)
{
    LoopspeedTrigger::loadStaticModels();

    position.x = x;
    position.y = y;
    position.z = z;
    rotationX = 0;
    rotationY = 0;
    rotationZ = 0;
    scaleX = 40;
    scaleY = 40;
    scaleZ = 40;

    visible = Global::displayLoopspeedTriggers;

    collideModelOriginal = LoopspeedTrigger::cmBaseSphere;

    collideModelTransformed = collideModelOriginal->duplicateMe();
    collideModelTransformed->parent = nullptr;
    CollisionChecker::addCollideModel(collideModelTransformed);

    updateTransformationMatrixYXZ();
    updateCollisionModelYXZ();
    baseColour.set(0.0f, 0.5f, 1.0f);
}

void LoopspeedTrigger::step()
{
    visible = Global::displayLoopspeedTriggers;
    collideModelTransformed->isVisible = visible;

    if (collideModelTransformed->wasCollidedWith)
    {
        baseColour.set(0.0f, 1.0f, 2.0f);
    }
    else
    {
        baseColour.set(0.0f, 0.5f, 1.0f);
    }
}

std::list<TexturedModel*>* LoopspeedTrigger::getModels()
{
    return &LoopspeedTrigger::modelsSphere;
}

void LoopspeedTrigger::loadStaticModels()
{
    if (LoopspeedTrigger::modelsSphere.size() == 0)
    {
        #ifdef DEV_MODE
        std::fprintf(stdout, "Loading LoopspeedTrigger sphere static models...\n");
        #endif

        loadModel(&LoopspeedTrigger::modelsSphere, "res/Models/GlobalObjects/Trigger/", "TriggerSphere");
    }

    if (LoopspeedTrigger::cmBaseSphere == nullptr)
    {
        LoopspeedTrigger::cmBaseSphere = loadCollisionModel("res/Models/GlobalObjects/Trigger/", "TriggerSphere");
    }
}
