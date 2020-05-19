#ifndef CAMERA_TRIGGER_H
#define CAMERA_TRIGGER_H

class Vector3f;
class TexturedModel;
class Dummy;

#include <list>
#include "collideableobject.h"

class CameraTrigger : public CollideableObject
{
private:
    static std::list<TexturedModel*> modelsCube;
    static std::list<TexturedModel*> modelsSphere;
    static CollisionModel* cmBaseCube;
    static CollisionModel* cmBaseSphere;
    static void loadStaticModels();
    
    int triggerType = 0;
    Dummy* pointA = nullptr;
    Dummy* pointB = nullptr;

public:
	CameraTrigger(int type,
                  float x,  float y,  float z,
                  float xr, float yr, float zr,
                  float xs, float ys, float zs,
                  float ax, float ay, float az,
                  float bx, float by, float bz);

	void step();

	std::list<TexturedModel*>* getModels();
};
#endif
