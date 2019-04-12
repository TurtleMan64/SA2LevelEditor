#include <cmath>
#include <list>

#include "triangle3d.h"
#include "../toolbox/vector.h"
#include "collisionmodel.h"
#include "../main/main.h"
#include "../toolbox/maths.h"


CollisionModel::CollisionModel()
{
	wasCollidedWith = false;
	treeMaxDepth = -1;
    isVisible = true;
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

void CollisionModel::offsetModel(Vector3f* offset)
{
	float x = offset->x;
	float y = offset->y;
	float z = offset->z;

	for (Triangle3D* tri : triangles)
	{
		tri->p1X += x;
		tri->p2X += x;
		tri->p3X += x;
		tri->p1Y += y;
		tri->p2Y += y;
		tri->p3Y += y;
		tri->p1Z += z;
		tri->p2Z += z;
		tri->p3Z += z;

		tri->generateValues();
	}

	generateMinMaxValues();
}

void CollisionModel::rotateModelY(float yRot, Vector3f* center)
{
	float angleRad = Maths::toRadians(yRot);
	float cosAng = cosf(angleRad);
	float sinAng = sinf(angleRad);
	for (Triangle3D* tri : triangles)
	{
		float xDiff = tri->p1X - center->x;
		float zDiff = tri->p1Z - center->z;
		tri->p1X = center->x + ((xDiff)*cosAng - (zDiff)*sinAng);
		tri->p1Z = center->z - (-(zDiff)*cosAng - (xDiff)*sinAng);

		xDiff = tri->p2X - center->x;
		zDiff = tri->p2Z - center->z;
		tri->p2X = center->x + ((xDiff)*cosAng - (zDiff)*sinAng);
		tri->p2Z = center->z - (-(zDiff)*cosAng - (xDiff)*sinAng);

		xDiff = tri->p3X - center->x;
		zDiff = tri->p3Z - center->z;
		tri->p3X = center->x + ((xDiff)*cosAng - (zDiff)*sinAng);
		tri->p3Z = center->z - (-(zDiff)*cosAng - (xDiff)*sinAng);

		tri->generateValues();
	}

	generateMinMaxValues();
}

//makes a collision model be the transformed version of this collision model
void CollisionModel::transformModel(CollisionModel* targetModel, Vector3f* translate, float yRot, float zRot)
{
	float offX = translate->x;
	float offY = translate->y;
	float offZ = translate->z;

	float angleRad = Maths::toRadians(yRot);
	float cosAng = cosf(angleRad);
	float sinAng = sinf(angleRad);

	float angleRadZ = Maths::toRadians(zRot);
	float cosAngZ = cosf(angleRadZ);
	float sinAngZ = sinf(angleRadZ);

	targetModel->deleteMe();

	for (Triangle3D* tri : triangles)
	{
		float xDiff = tri->p1X;
		float zDiff = tri->p1Z;
		float yDiff = tri->p1Y;

		float newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		float newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		Vector3f newP1(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));

		xDiff = tri->p2X;
		zDiff = tri->p2Z;
		yDiff = tri->p2Y;
		newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		Vector3f newP2(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));

		xDiff = tri->p3X;
		zDiff = tri->p3Z;
		yDiff = tri->p3Y;
		newX = (xDiff*cosAngZ - yDiff*sinAngZ);
		newY = (yDiff*cosAngZ + xDiff*sinAngZ);
		Vector3f newP3(offX + (newX*cosAng - zDiff*sinAng), offY + newY, offZ + (zDiff*cosAng + newX*sinAng));


		Triangle3D* newTri = new Triangle3D(&newP1, &newP2, &newP3); INCR_NEW("Triangle3D");

		targetModel->triangles.push_back(newTri);
	}

	targetModel->generateMinMaxValues();
}

void transformModel(CollisionModel* , Vector3f* , float , float , float )
{
	
}

//makes a collision model be the transformed version of this collision model
void CollisionModel::transformModel(CollisionModel* targetModel, Vector3f* translate, float yRot)
{
	float offX = translate->x;
	float offY = translate->y;
	float offZ = translate->z;

	float angleRad = Maths::toRadians(yRot);
	float cosAng = cosf(angleRad);
	float sinAng = sinf(angleRad);

	targetModel->deleteMe();

	for (Triangle3D* tri : triangles)
	{
		float xDiff = tri->p1X;
		float zDiff = tri->p1Z;
		Vector3f newP1(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p1Y, offZ + (zDiff*cosAng + xDiff*sinAng));

		xDiff = tri->p2X;
		zDiff = tri->p2Z;
		Vector3f newP2(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p2Y, offZ + (zDiff*cosAng + xDiff*sinAng));

		xDiff = tri->p3X;
		zDiff = tri->p3Z;
		Vector3f newP3(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p3Y, offZ + (zDiff*cosAng + xDiff*sinAng));

		Triangle3D* newTri = new Triangle3D(&newP1, &newP2, &newP3); INCR_NEW("Triangle3D");

		targetModel->triangles.push_back(newTri);
	}

	targetModel->generateMinMaxValues();
}

void CollisionModel::transformModelWithScale(CollisionModel* targetModel, Vector3f* translate, float yRot, float scale)
{
	float offX = translate->x;
	float offY = translate->y;
	float offZ = translate->z;

	float angleRad = Maths::toRadians(yRot);
	float cosAng = cosf(angleRad);
	float sinAng = sinf(angleRad);

	targetModel->deleteMe();

	for (Triangle3D* tri : triangles)
	{
		float xDiff = tri->p1X*scale;
		float zDiff = tri->p1Z*scale;
		Vector3f newP1(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p1Y*scale, offZ + (zDiff*cosAng + xDiff*sinAng));

		xDiff = tri->p2X*scale;
		zDiff = tri->p2Z*scale;
		Vector3f newP2(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p2Y*scale, offZ + (zDiff*cosAng + xDiff*sinAng));

		xDiff = tri->p3X*scale;
		zDiff = tri->p3Z*scale;
		Vector3f newP3(offX + (xDiff*cosAng - zDiff*sinAng), offY + tri->p3Y*scale, offZ + (zDiff*cosAng + xDiff*sinAng));

		Triangle3D* newTri = new Triangle3D(&newP1, &newP2, &newP3); INCR_NEW("Triangle3D");

		targetModel->triangles.push_back(newTri);
	}

	targetModel->generateMinMaxValues();
}

//makes a collision model be the transformed version of this collision model
void CollisionModel::transformModel(CollisionModel* targetModel, Vector3f* translate)
{
	float offX = translate->x;
	float offY = translate->y;
	float offZ = translate->z;

	targetModel->deleteMe();

	for (Triangle3D* tri : triangles)
	{
		Vector3f newP1(offX + tri->p1X, offY + tri->p1Y, offZ + tri->p1Z);
		Vector3f newP2(offX + tri->p2X, offY + tri->p2Y, offZ + tri->p2Z);
		Vector3f newP3(offX + tri->p3X, offY + tri->p3Y, offZ + tri->p3Z);

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
