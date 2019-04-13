#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

class Triangle3D;
class CollisionModel;

#include <math.h>
#include <list>
#include <unordered_set>
#include "../toolbox/vector.h"

class CollisionChecker
{
private:
	static Vector3f collidePosition;
	static Triangle3D* collideTriangle;
	static std::list<CollisionModel*> collideModels;
	static bool checkPlayer;

public:
	static bool debug;

public:
	static void initChecker();

	// Makes the next collision check set which collision
	// model the player has collided with, and sets that model
	// to touching the player.
	static void setCheckPlayer();

	// Sets all collision models to not have the player on them
	static void falseAlarm();

    static bool checkCollision(
		Vector3f* p1, Vector3f* p2);

	static bool checkCollision(
		float px1, float py1, float pz1,
		float px2, float py2, float pz2);

	static bool checkPointInTriangle3D(
		float checkx, float checky, float checkz,
		Triangle3D* tri);

	static bool checkPointInTriangle2D(
		float x,  float y,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3);

	//delete's all collide models
	static void deleteAllCollideModels();

	//use this when reloading the same level - then you dont have to regenerate the quad trees
	static void deleteAllCollideModelsExceptQuadTrees();

	static void deleteCollideModel(CollisionModel* cm);

    //The model added must be created with the new keyword, as it will be deleted
    // here later, via deleteCollideModel or deleteAlmostAllCollideModels call
	static void addCollideModel(CollisionModel* cm);

	//based off of the last collision check
	static Triangle3D* getCollideTriangle();

	//based off of the last collision check
	static Vector3f* getCollidePosition();
};

#endif
