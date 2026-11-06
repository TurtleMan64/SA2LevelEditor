#include <math.h>
#include "triangle3d.h"
#include "../toolbox/vector.h"

Triangle3D::Triangle3D(Vector3f* newP1, Vector3f* newP2, Vector3f* newP3)
{
    this->p1X = newP1->x;
    this->p1Y = newP1->y;
    this->p1Z = newP1->z;

    this->p2X = newP2->x;
    this->p2Y = newP2->y;
    this->p2Z = newP2->z;

    this->p3X = newP3->x;
    this->p3Y = newP3->y;
    this->p3Z = newP3->z;

    generateValues();
}

void Triangle3D::generateValues()
{
    Vector3f vec1(p2X - p1X, p2Y - p1Y, p2Z - p1Z);
    Vector3f vec2(p3X - p1X, p3Y - p1Y, p3Z - p1Z);

    Vector3f cross = vec1.cross(&vec2);

    float newD = cross.x*p1X + cross.y*p1Y + cross.z*p1Z;

    A = cross.x;
    B = cross.y;
    C = cross.z;
    D = -newD;

    float mag = sqrtf(A*A + B*B + C*C);

    if (mag != 0)
    {
        normal.x = A / mag;
        normal.y = B / mag;
        normal.z = C / mag;
    }
    else
    {
        normal.x = 0;
        normal.y = 1;
        normal.z = 0;
    }

    maxX = fmaxf(p1X, fmaxf(p2X, p3X));
    minX = fminf(p1X, fminf(p2X, p3X));
    maxY = fmaxf(p1Y, fmaxf(p2Y, p3Y));
    minY = fminf(p1Y, fminf(p2Y, p3Y));
    maxZ = fmaxf(p1Z, fmaxf(p2Z, p3Z));
    minZ = fminf(p1Z, fminf(p2Z, p3Z));
}
