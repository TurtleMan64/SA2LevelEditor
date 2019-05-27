#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

class Vector3f;
class Triangle3D;
class SA2Object;

#include <math.h>
#include <list>


class CollisionModel
{
public:
    SA2Object* parent;

	std::list<Triangle3D*> triangles;

	bool wasCollidedWith;

    bool isVisible;

	int treeMaxDepth;
    float leafNodeWidth;
    float leafNodeHeight;

	float maxX;
	float minX;
	float maxY;
	float minY;
	float maxZ;
	float minZ;

	CollisionModel();

	void generateMinMaxValues();

	bool hasQuadTree();

	//makes a collision model be the transformed version of this collision model
	void transformModelYXZ(CollisionModel* targetModel, Vector3f* translate, int bamsXRot, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale);

    void transformModelZY(CollisionModel* targetModel, Vector3f* translate, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale);

	//calls delete on every Triangle3D contained within triangles list, 
	// and every QuadTreeNode in this quad tree. 
	// this MUST be called before this object is deleted, or you memory leak
	// the triangles in the list and the nodes!
	void deleteMe();

    //makes a new CollisionModel object on the heap and copies our values over to it.
    // it is a completely new object, so you need to call deleteMe() and delete it later.
    CollisionModel* duplicateMe();
};

#endif
