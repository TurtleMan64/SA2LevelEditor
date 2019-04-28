#include <cmath>
#include <list>

#include "triangle3d.h"
#include "../toolbox/vector.h"
#include "collisionmodel.h"
#include "../main/main.h"
#include "../toolbox/maths.h"
#include "../entities/sa2object.h"


CollisionModel::CollisionModel()
{
	wasCollidedWith = false;
	treeMaxDepth = -1;
    isVisible = true;
    parent = nullptr;
}

void CollisionModel::generateMinMaxValues()
{
	if (triangles.size() == 0)
	{
		return;
	}

	minX = triangles.front()->minX;
	maxX = triangles.front()->maxX;
	minY = triangles.front()->minY;
	maxY = triangles.front()->maxY;
	minZ = triangles.front()->minZ;
	maxZ = triangles.front()->maxZ;

	for (Triangle3D* tri : triangles)
	{
		minX = fmin(minX, tri->minX);
		maxX = fmax(maxX, tri->maxX);
		minY = fmin(minY, tri->minY);
		maxY = fmax(maxY, tri->maxY);
		minZ = fmin(minZ, tri->minZ);
		maxZ = fmax(maxZ, tri->maxZ);
	}
}

bool CollisionModel::hasQuadTree()
{
	return (false);
}

//makes a collision model be the transformed version of this collision model
void CollisionModel::transformModel(CollisionModel* targetModel, Vector3f* translate, int bamsXRot, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale)
{
	//float offX = translate->x;
	//float offY = translate->y;
	//float offZ = translate->z;

    float angleRadX = Maths::toRadians(bamsXRot);
	//float cosAngX = cosf(angleRadX);
	//float sinAngX = sinf(angleRadX);

	float angleRadY = Maths::toRadians(bamsYRot);
	//float cosAngY = cosf(angleRadY);
	//float sinAngY = sinf(angleRadY);

	float angleRadZ = Maths::toRadians(bamsZRot);
	//float cosAngZ = cosf(angleRadZ);
	//float sinAngZ = sinf(angleRadZ);

	targetModel->deleteMe();

	for (Triangle3D* tri : triangles)
	{
		//float xDiff = tri->p1X*xScale;
		//float zDiff = tri->p1Z*zScale;
		//float yDiff = tri->p1Y*yScale;
        //
		//float newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		//float newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		//Vector3f newP1(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));
        //
		//xDiff = tri->p2X*xScale;
		//zDiff = tri->p2Z*zScale;
		//yDiff = tri->p2Y*yScale;
		//newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		//newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		//Vector3f newP2(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));
        //
		//xDiff = tri->p3X*xScale;
		//zDiff = tri->p3Z*zScale;
		//yDiff = tri->p3Y*yScale;
		//newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		//newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		//Vector3f newP3(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));


        Vector3f newP1(tri->p1X*xScale, tri->p1Y*yScale, tri->p1Z*zScale);
        Vector3f newP2(tri->p2X*xScale, tri->p2Y*yScale, tri->p2Z*zScale);
        Vector3f newP3(tri->p3X*xScale, tri->p3Y*yScale, tri->p3Z*zScale);
        Vector3f axis;

        //order = y, x, z
        axis.set(0, 1, 0);
        newP1 = Maths::rotatePoint(&newP1, &axis, angleRadY);
        newP2 = Maths::rotatePoint(&newP2, &axis, angleRadY);
        newP3 = Maths::rotatePoint(&newP3, &axis, angleRadY);

        axis.set(1, 0, 0);
        newP1 = Maths::rotatePoint(&newP1, &axis, angleRadX);
        newP2 = Maths::rotatePoint(&newP2, &axis, angleRadX);
        newP3 = Maths::rotatePoint(&newP3, &axis, angleRadX);

        axis.set(0, 0, 1);
        newP1 = Maths::rotatePoint(&newP1, &axis, angleRadZ);
        newP2 = Maths::rotatePoint(&newP2, &axis, angleRadZ);
        newP3 = Maths::rotatePoint(&newP3, &axis, angleRadZ);

        newP1 = newP1 + translate;
        newP2 = newP2 + translate;
        newP3 = newP3 + translate;

		Triangle3D* newTri = new Triangle3D(&newP1, &newP2, &newP3); INCR_NEW("Triangle3D");

		targetModel->triangles.push_back(newTri);
	}

	targetModel->generateMinMaxValues();
}

void CollisionModel::deleteMe()
{
	//Delete triangles 
	for (Triangle3D* tri : triangles)
	{
		delete tri; INCR_DEL("Triangle3D");
	}

	triangles.clear();
}

CollisionModel* CollisionModel::duplicateMe()
{
    CollisionModel* copy = new CollisionModel; INCR_NEW("CollisionModel");

    copy->wasCollidedWith = this->wasCollidedWith;
    copy->isVisible       = this->isVisible;
	copy->treeMaxDepth    = this->treeMaxDepth;
    copy->leafNodeWidth   = this->leafNodeWidth;
    copy->leafNodeHeight  = this->leafNodeHeight;
	copy->maxX            = this->maxX;
	copy->minX            = this->minX;
	copy->maxY            = this->maxY;
	copy->minY            = this->minY;
	copy->maxZ            = this->maxZ;
	copy->minZ            = this->minZ;

    for (Triangle3D* tri : triangles)
    {
        Vector3f v1(tri->p1X, tri->p1Y, tri->p1Z);
        Vector3f v2(tri->p2X, tri->p2Y, tri->p2Z);
        Vector3f v3(tri->p3X, tri->p3Y, tri->p3Z);
        Triangle3D* newTri = new Triangle3D(&v1, &v2, &v3); INCR_NEW("Triangle3D");
        newTri->generateValues();
        copy->triangles.push_back(newTri);
    }

    return copy;
}
