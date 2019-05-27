#include "collideableobject.h"

#include "../collision/collisionmodel.h"


CollideableObject::CollideableObject()
{

}

void CollideableObject::updateCollisionModelYXZ()
{
	collideModelOriginal->transformModelYXZ(collideModelTransformed, &position, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void CollideableObject::updateCollisionModelYXZ(float sclX, float sclY, float sclZ)
{
    collideModelOriginal->transformModelYXZ(collideModelTransformed, &position, rotationX, rotationY, rotationZ, sclX, sclY, sclZ);
}

void CollideableObject::updateCollisionModelYXZ(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModelYXZ(cmToUpdate, &position, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void CollideableObject::updateCollisionModelZY()
{
	collideModelOriginal->transformModelZY(collideModelTransformed, &position, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void CollideableObject::updateCollisionModelZY(float sclX, float sclY, float sclZ)
{
    collideModelOriginal->transformModelZY(collideModelTransformed, &position, rotationY, rotationZ, sclX, sclY, sclZ);
}

void CollideableObject::updateCollisionModelZY(CollisionModel* cmBase, CollisionModel* cmToUpdate)
{
	cmBase->transformModelZY(cmToUpdate, &position, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}
