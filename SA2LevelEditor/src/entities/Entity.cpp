#include <glad/glad.h>

#include "entity.h"
#include "../models/texturedmodel.h"
#include "../toolbox/vector.h"
#include "../toolbox/maths.h"
#include "../main/main.h"
#include "../collision/collisionmodel.h"

#include <list>
#include <iostream>
#include <string>

Entity::Entity()
{
	this->position.x = 0;
	this->position.y = 0;
	this->position.z = 0;
	this->rotationX = 0;
	this->rotationY = 0;
	this->rotationZ = 0; 
	this->scaleX = 1;
    this->scaleY = 1;
    this->scaleZ = 1;
	this->visible = true;
	this->baseColour.set(1,1,1);
}

Entity::Entity(Vector3f* position)
{
	this->position.x = position->x;
	this->position.y = position->y;
	this->position.z = position->z;
	this->rotationX = 0;
	this->rotationY = 0;
	this->rotationZ = 0;
	this->scaleX = 1;
    this->scaleY = 1;
    this->scaleZ = 1;
	this->visible = true;
	this->baseColour.set(1,1,1);
}

Entity::~Entity()
{

}

void Entity::step()
{

}

void Entity::increasePosition(float dx, float dy, float dz)
{
	position.x += dx;
	position.y += dy;
	position.z += dz;
}

void Entity::increaseRotation(int dx, int dy, int dz)
{
	rotationX += dx;
	rotationY += dy;
	rotationZ += dz;
}

std::list<TexturedModel*>* Entity::getModels()
{
	return nullptr;
}

bool Entity::isSA2Object()
{
    return false;
}

void Entity::setPosition(Vector3f* newPos)
{
	this->position.x = newPos->x;
	this->position.y = newPos->y;
	this->position.z = newPos->z;
}
void Entity::setPosition(float newX, float newY, float newZ)
{
	this->position.x = newX;
	this->position.y = newY;
	this->position.z = newZ;
}

void Entity::setScale(float sclX, float sclY, float sclZ)
{
    this->scaleX = sclX;
    this->scaleY = sclY;
    this->scaleZ = sclZ;
}

void Entity::setBaseColour(float red, float green, float blue)
{
	baseColour.set(red, green, blue);
}

void Entity::updateTransformationMatrixYXZ()
{
	Maths::createTransformationMatrixYXZ(&transformationMatrix, &position, rotationX, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void Entity::updateTransformationMatrixYXZ(float sclX, float sclY, float sclZ)
{
	Maths::createTransformationMatrixYXZ(&transformationMatrix, &position, rotationX, rotationY, rotationZ, sclX, sclY, sclZ);
}

void Entity::updateTransformationMatrixZY()
{
	Maths::createTransformationMatrixZY(&transformationMatrix, &position, rotationY, rotationZ, scaleX, scaleY, scaleZ);
}

void Entity::updateTransformationMatrixZY(float sclX, float sclY, float sclZ)
{
	Maths::createTransformationMatrixZY(&transformationMatrix, &position, rotationY, rotationZ, sclX, sclY, sclZ);
}

Matrix4f* Entity::getTransformationMatrix()
{
	return &transformationMatrix;
}

void Entity::deleteModels(std::list<TexturedModel*>* modelsToDelete)
{
	for (auto model : (*modelsToDelete))
	{
		model->deleteMe();
		delete model; INCR_DEL("TexturedModel");
	}
	modelsToDelete->clear();
}

void Entity::deleteCollisionModel(CollisionModel** colModelToDelete)
{
	if ((*colModelToDelete) != nullptr)
	{
		(*colModelToDelete)->deleteMe();
		delete (*colModelToDelete); INCR_DEL("CollisionModel");
		(*colModelToDelete) = nullptr;
	}
}
