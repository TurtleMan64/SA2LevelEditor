#include <cmath>
#include <list>
#include <algorithm>
#include <unordered_set>

#include "collisionchecker.h"
#include "collisionmodel.h"
#include "triangle3d.h"
#include "../main/main.h"
#include "../entities/sa2object.h"

Vector3f CollisionChecker::collidePosition;
Triangle3D* CollisionChecker::collideTriangle;
std::list<CollisionModel*> CollisionChecker::collideModels;
bool CollisionChecker::checkPlayer;
bool CollisionChecker::debug;

void CollisionChecker::initChecker()
{
    CollisionChecker::collidePosition.x = 0;
    CollisionChecker::collidePosition.y = 0;
    CollisionChecker::collidePosition.z = 0;

    CollisionChecker::collideTriangle = nullptr;

    CollisionChecker::checkPlayer = false;
}

void CollisionChecker::setCheckPlayer()
{
    CollisionChecker::checkPlayer = true;
}


void CollisionChecker::falseAlarm()
{
    for (CollisionModel* cm : CollisionChecker::collideModels)
    {
        cm->wasCollidedWith = false;
    }
}

bool CollisionChecker::checkCollision(
    Vector3f* p1, Vector3f* p2)
{
    return CollisionChecker::checkCollision(p1->x, p1->y, p1->z, p2->x, p2->y, p2->z);
}

bool CollisionChecker::checkCollision(
    float px1, float py1, float pz1,
    float px2, float py2, float pz2)
{
    bool triangleCollide = false;
    CollisionModel* finalModel = nullptr;
    float firstAbove = -2;
    float secondAbove = -2;
    float A, B, C, D;
    float numerator, denominator, u;

    //the distance from p1 to the current collision location
    float minDist = -1;

    //int debugCount = 1;

    for (CollisionModel* cm : CollisionChecker::collideModels)
    {
        //if (CollisionChecker::checkPlayer)
        //{
            cm->wasCollidedWith = false;
        //}

        if (cm->isVisible)
        //Bounds check on entire model
        //if any of these are true, we can skip the model
        //if (!((px1 <= cm->minX && px2 <= cm->minX) || (px1 >= cm->maxX && px2 >= cm->maxX) ||
        //      (pz1 <= cm->minZ && pz2 <= cm->minZ) || (pz1 >= cm->maxZ && pz2 >= cm->maxZ) ||
        //      (py1 <= cm->minY && py2 <= cm->minY) || (py1 >= cm->maxY && py2 >= cm->maxY)))
        {
            // only display objects if the toggle is on
            if (Global::displayObjects || cm->parent == nullptr)
            {
                for (Triangle3D* currTriangle : cm->triangles)
                {
                    //Bounds check on individual triangle
                    //if any of these are true, we can skip the triangle
                    if (!((px1 <= currTriangle->minX && px2 <= currTriangle->minX) || (px1 >= currTriangle->maxX && px2 >= currTriangle->maxX) ||
                          (pz1 <= currTriangle->minZ && pz2 <= currTriangle->minZ) || (pz1 >= currTriangle->maxZ && pz2 >= currTriangle->maxZ) ||
                          (py1 <= currTriangle->minY && py2 <= currTriangle->minY) || (py1 >= currTriangle->maxY && py2 >= currTriangle->maxY)))
                    {
                        A = currTriangle->A;
                        B = currTriangle->B;
                        C = currTriangle->C;
                        D = currTriangle->D;

                        numerator = (A*px1 + B*py1 + C*pz1 + D);
                        denominator = (A*(px1 - px2) + B*(py1 - py2) + C*(pz1 - pz2));

                        if (denominator != 0)
                        {
                            u = (numerator / denominator);
                            float cix = px1 + u*(px2 - px1);
                            float ciy = py1 + u*(py2 - py1);
                            float ciz = pz1 + u*(pz2 - pz1);

                            if (B != 0)
                            {
                                float planey1 = (((-A*px1) + (-C*pz1) - D) / B);
                                float planey2 = (((-A*px2) + (-C*pz2) - D) / B);
                                firstAbove = copysignf(1, py1 - planey1);
                                secondAbove = copysignf(1, py2 - planey2);
                            }
                            else if (A != 0)
                            {
                                float planex1 = (((-B*py1) + (-C*pz1) - D) / A);
                                float planex2 = (((-B*py2) + (-C*pz2) - D) / A);
                                firstAbove = copysignf(1, px1 - planex1);
                                secondAbove = copysignf(1, px2 - planex2);
                            }
                            else if (C != 0)
                            {
                                float planez1 = (((-B*py1) + (-A*px1) - D) / C);
                                float planez2 = (((-B*py2) + (-A*px2) - D) / C);
                                firstAbove = copysignf(1, pz1 - planez1);
                                secondAbove = copysignf(1, pz2 - planez2);
                            }

                            if (secondAbove != firstAbove && checkPointInTriangle3D(cix, ciy, ciz, currTriangle))
                            {
                                //what is the distance to the triangle? set it to maxdist
                                float thisDist = (sqrtf(fabsf((cix - px1)*(cix - px1) + (ciy - py1)*(ciy - py1) + (ciz - pz1)*(ciz - pz1))));
                                if (minDist == -1 || thisDist < minDist)
                                {
                                    triangleCollide = true;
                                    collideTriangle = currTriangle;
                                    collidePosition.x = cix;
                                    collidePosition.y = ciy;
                                    collidePosition.z = ciz;
                                    minDist = thisDist;
                                    finalModel = cm;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Global::selectedSA2Object = nullptr;
    if (/*CollisionChecker::checkPlayer && */finalModel != nullptr)
    {
        finalModel->wasCollidedWith = true;

        SA2Object* parent = finalModel->parent;
        if (parent != nullptr)
        {
            parent->updateEditorWindows();
            Global::selectedSA2Object = parent;
        }
    }

    if (Global::selectedSA2Object == nullptr)
    {
        Global::resetObjectWindow();
    }

    //CollisionChecker::checkPlayer = false;

    return triangleCollide;
}

bool CollisionChecker::checkPointInTriangle3D(
    float checkx, float checky, float checkz,
    Triangle3D* tri)
{
    float nX = fabsf(tri->normal.x);
    float nY = fabsf(tri->normal.y);
    float nZ = fabsf(tri->normal.z);

    if (nY > nX && nY > nZ)
    {
        //from the top
        return (checkPointInTriangle2D(
                checkx, checkz, 
                tri->p1X, tri->p1Z, 
                tri->p2X, tri->p2Z, 
                tri->p3X, tri->p3Z));
    }
    else if (nX > nZ)
    {
        //from the left
        return (checkPointInTriangle2D(
                checkz, checky, 
                tri->p1Z, tri->p1Y, 
                tri->p2Z, tri->p2Y, 
                tri->p3Z, tri->p3Y));
    }

    //from the front
    return (checkPointInTriangle2D(
            checkx, checky, 
            tri->p1X, tri->p1Y, 
            tri->p2X, tri->p2Y, 
            tri->p3X, tri->p3Y));
}

bool CollisionChecker::checkPointInTriangle2D(
    float x,  float y,
    float x1, float y1,
    float x2, float y2,
    float x3, float y3)
{
    float denominator = ((y2 - y3)*(x1 - x3) + (x3 - x2)*(y1 - y3));
    float a = ((y2 - y3)*(x - x3) + (x3 - x2)*(y - y3)) / denominator;
    float b = ((y3 - y1)*(x - x3) + (x1 - x3)*(y - y3)) / denominator;
    float c = 1 - a - b;

    return (0 <= a && a <= 1 && 0 <= b && b <= 1 && 0 <= c && c <= 1);
}

void CollisionChecker::deleteAllCollideModels()
{
    while (CollisionChecker::collideModels.size() > 0)
    {
        CollisionModel* cm = CollisionChecker::collideModels.front();
        CollisionChecker::collideModels.remove(cm);
        cm->deleteMe();
        delete cm; INCR_DEL("CollisionModel");
    }
}

void CollisionChecker::deleteAllCollideModelsExceptQuadTrees()
{
    std::list<CollisionModel*> modelsToDelete;

    for (CollisionModel* cm : CollisionChecker::collideModels)
    {
        if (cm->hasQuadTree() == false)
        {
            modelsToDelete.push_back(cm);
        }
    }

    for (CollisionModel* cm : modelsToDelete)
    {
        CollisionChecker::collideModels.remove(cm);
        cm->deleteMe();
        delete cm; INCR_DEL("CollisionModel");
    }
}

void CollisionChecker::deleteCollideModel(CollisionModel* cm)
{
    CollisionChecker::collideModels.remove(cm);
    cm->deleteMe();
    delete cm; INCR_DEL("CollisionModel");
}

//The model added must be created with the new keyword, as it will be deleted
// here later, via deleteCollideModel or deleteAlmostAllCollideModels call
void CollisionChecker::addCollideModel(CollisionModel* cm)
{
    CollisionChecker::collideModels.push_back(cm);
}

Triangle3D* CollisionChecker::getCollideTriangle()
{
    return CollisionChecker::collideTriangle;
}

Vector3f* CollisionChecker::getCollidePosition()
{
    return &CollisionChecker::collidePosition;
}
