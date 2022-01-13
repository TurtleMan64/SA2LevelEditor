#ifndef LOOPSPEED_TRIGGER_H
#define LOOPSPEED_TRIGGER_H

class Vector3f;
class TexturedModel;
class Dummy;

#include <list>
#include "collideableobject.h"

class LoopspeedTrigger : public CollideableObject
{
private:
    static std::list<TexturedModel*> modelsSphere;
    static CollisionModel* cmBaseSphere;
    static void loadStaticModels();

public:
    LoopspeedTrigger(float x,  float y,  float z);

    void step();

    std::list<TexturedModel*>* getModels();
};
#endif
