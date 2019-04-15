#include "collideableobject.h"

#include "../collision/collisionmodel.h"


CollideableObject::CollideableObject()
{

}

void CollideableObject::updateCollisionModel()
{
	collideModelOriginal->transformModel(collideModelTransformed, &position, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void CollideableObject::updateCollisionModel(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModel(cmToUpdate, &position, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

