#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

class Vector3f;
class Triangle3D;
class SA2Object;

#include <math.h>
#include <list>
#include "../toolbox/vector.h"

struct PCMeshset {
    uint16_t typeAndMaterialID;
    uint16_t numMeshes;
    int16_t* __ptr32 meshes;
    uint32_t* __ptr32 attrA;
    Vector3f* __ptr32 normals;
    uint32_t* __ptr32 vertexColor;
    uint32_t* __ptr32 vertexUV;
};

struct PCMeshModel {
    Vector3f* __ptr32 points;
    Vector3f* __ptr32 normals;
    uint32_t numPoints;
    struct PCMeshset* __ptr32 meshsets;
    uint32_t materials;
    uint16_t numMeshsets;
    uint16_t numMaterials;
    Vector3f center;
    float radius;
};

struct PCMeshObject {
    uint32_t evalFlags;
    struct PCMeshModel* __ptr32 model;
    Vector3f pos;
    int32_t ang[3];
    Vector3f scale;
    struct PCMeshObject* __ptr32 child;
    struct PCMeshObject* __ptr32 sibling;
};

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

    void transformModelZXY(CollisionModel* targetModel, Vector3f* translate, int bamsXRot, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale);

    void transformModelXZY(CollisionModel* targetModel, Vector3f* translate, int bamsXRot, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale);

    void transformModelZY(CollisionModel* targetModel, Vector3f* translate, int bamsYRot, int bamsZRot, float xScale, float yScale, float zScale);

    void transformModelXY(CollisionModel* targetModel, Vector3f* translate, int bamsXRot, int bamsYRot, float xScale, float yScale, float zScale);

    void transformModelY(CollisionModel* targetModel, Vector3f* translate, int bamsYRot, float xScale, float yScale, float zScale);

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
