#ifndef COLLIDEABLEOBJECT_H
#define COLLIDEABLEOBJECT_H

class CollisionModel;

#include "entity.h"

class CollideableObject : public Entity
{
protected:
	CollisionModel* collideModelOriginal;
	CollisionModel* collideModelTransformed;

public:
	CollideableObject();

	void updateCollisionModel(CollisionModel* cmBase, CollisionModel* cmToUpdate);

	void updateCollisionModel();
};
#endif
