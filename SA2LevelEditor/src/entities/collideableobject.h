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

	void updateCollisionModelYXZ(CollisionModel* cmBase, CollisionModel* cmToUpdate);
	void updateCollisionModelYXZ();
    void updateCollisionModelYXZ(float sclX, float sclY, float sclZ);

    void updateCollisionModelZY(CollisionModel* cmBase, CollisionModel* cmToUpdate);
	void updateCollisionModelZY();
    void updateCollisionModelZY(float sclX, float sclY, float sclZ);
};
#endif
