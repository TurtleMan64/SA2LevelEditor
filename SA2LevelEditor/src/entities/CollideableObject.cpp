#include "collideableobject.h"

#include "../collision/collisionmodel.h"


CollideableObject::CollideableObject()
{

}

void CollideableObject::updateCMJustPosition()
{
	collideModelOriginal->transformModel(collideModelTransformed, &position);
}

void CollideableObject::updateCollisionModel()
{
	collideModelOriginal->transformModel(collideModelTransformed, &position, -rotationY);
}

void CollideableObject::updateCollisionModelWithScale()
{
	collideModelOriginal->transformModelWithScale(collideModelTransformed, &position, -rotationY, scale);
}

void CollideableObject::updateCollisionModelWithZ()
{
	collideModelOriginal->transformModel(collideModelTransformed, &position, -rotationY, rotationZ);
}

void CollideableObject::updateCMJustPosition(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModel(cmToUpdate, &position);
}

void CollideableObject::updateCollisionModel(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModel(cmToUpdate, &position, -rotationY);
}

void CollideableObject::updateCollisionModelWithZ(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModel(cmToUpdate, &position, -rotationY, rotationZ);
}
