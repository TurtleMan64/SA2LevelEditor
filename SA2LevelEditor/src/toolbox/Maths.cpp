#include <Windows.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <random>
#include <chrono>

#include "../entities/camera.h"
#include "matrix.h"
#include "vector.h"
#include "maths.h"
#include "../main/main.h"
#include "../main/displaymanager.h"
#include "../rendering/masterrenderer.h"

std::mt19937* Maths::generatorUniform = new std::mt19937(0);
std::uniform_real_distribution<float>* Maths::distributionUniform = new std::uniform_real_distribution<float>(0.0f, 1.0f);

std::default_random_engine* Maths::generatorNormal = new std::default_random_engine(0);
std::normal_distribution<float>* Maths::distributionNormal = new std::normal_distribution<float>(0.0f, 1.0f);

const float Maths::PI = 3.14159265358979323846f;

float Maths::toRadians(float degrees)
{
    return (degrees*0.01745329251f);
}

float Maths::toDegrees(float radians)
{
    return (radians*57.2957795131f);
}

float Maths::bamsToRad(int bams)
{
    return toRadians(bamsToDeg(bams));
}

float Maths::bamsToRad(signed short bams)
{
    return toRadians(bamsToDeg((int)bams));
}

float Maths::bamsToDeg(int bams)
{
    return (bams/182.044444444444444f);
}

int Maths::radToBams(float rad)
{
    return degToBams(toDegrees(rad));
}

int Maths::degToBams(float deg)
{
    return (int)std::round(deg*182.044444444444444f);
}

/*
** Blank Matrix4f should have already been created. This function does not allocate any memory to matrix.
*/
//void Maths::createTransformationMatrix(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float scale)
//{
//    matrix->setIdentity();
//    matrix->translate(translation);
//    Vector3f vec;
//
//    vec.set(0, 1, 0);
//    matrix->rotate(toRadians(ry), &vec);
//
//    vec.set(0, 0, 1);
//    matrix->rotate(toRadians(rz), &vec);
//
//    vec.set(1, 0, 0);
//    matrix->rotate(toRadians(rx), &vec);
//
//    vec.set(scale, scale, scale);
//    matrix->scale(&vec);
//}

//void Maths::createTransformationMatrix(Matrix4f* matrix, Vector3f* translation, float rx, float ry, float rz, float scaleX, float scaleY, float scaleZ)
//{
//    matrix->setIdentity();
//    matrix->translate(translation);
//    Vector3f vec;
//
//    vec.set(0, 1, 0);
//    matrix->rotate(toRadians(ry), &vec);
//
//    vec.set(0, 0, 1);
//    matrix->rotate(toRadians(rz), &vec);
//
//    vec.set(1, 0, 0);
//    matrix->rotate(toRadians(rx), &vec);
//
//    vec.set(scaleX, scaleY, scaleZ);
//    matrix->scale(&vec);
//}

//For 2D GUI Images
void Maths::createTransformationMatrix(Matrix4f* result, Vector2f* translation, float rotation, Vector2f* scale)
{
    result->setIdentity();
    result->translate(translation);
    Vector3f axis(0, 0, 1);
    result->rotate(toRadians(rotation), &axis);
    result->scale(scale);
}

void Maths::createTransformationMatrixYXZ(Matrix4f* matrix, Vector3f* translation, int rx, int ry, int rz,  float sX, float sY, float sZ)
{
    matrix->setIdentity();
    matrix->translate(translation);
    Vector3f vec;

    vec.set(0, 0, 1);
    matrix->rotate(bamsToRad(rz), &vec);

    vec.set(1, 0, 0);
    matrix->rotate(bamsToRad(rx), &vec);

    vec.set(0, 1, 0);
    matrix->rotate(bamsToRad(ry), &vec);

    vec.set(sX, sY, sZ);
    matrix->scale(&vec);
}

void Maths::createTransformationMatrixZXY(Matrix4f* matrix, Vector3f* translation, int rx, int ry, int rz,  float sX, float sY, float sZ)
{
    matrix->setIdentity();
    matrix->translate(translation);
    Vector3f vec;

    vec.set(0, 1, 0);
    matrix->rotate(bamsToRad(ry), &vec);

    vec.set(1, 0, 0);
    matrix->rotate(bamsToRad(rx), &vec);

    vec.set(0, 0, 1);
    matrix->rotate(bamsToRad(rz), &vec);

    vec.set(sX, sY, sZ);
    matrix->scale(&vec);
}

void Maths::createTransformationMatrixXZY(Matrix4f* matrix, Vector3f* translation, int rx, int ry, int rz,  float sX, float sY, float sZ)
{
    matrix->setIdentity();
    matrix->translate(translation);
    Vector3f vec;

    vec.set(0, 1, 0);
    matrix->rotate(bamsToRad(ry), &vec);

    vec.set(0, 0, 1);
    matrix->rotate(bamsToRad(rz), &vec);

    vec.set(1, 0, 0);
    matrix->rotate(bamsToRad(rx), &vec);

    vec.set(sX, sY, sZ);
    matrix->scale(&vec);
}

void Maths::createTransformationMatrixZY(Matrix4f* matrix, Vector3f* translation, int ry, int rz,  float sX, float sY, float sZ)
{
    matrix->setIdentity();
    matrix->translate(translation);
    Vector3f vec;

    //Z, then Y. No X

    vec.set(0, 1, 0);
    matrix->rotate(Maths::bamsToRad(ry), &vec);

    vec.set(0, 0, 1);
    matrix->rotate(Maths::bamsToRad(rz), &vec);

    vec.set(sX, sY, sZ);
    matrix->scale(&vec);
}

void Maths::createTransformationMatrixXY(Matrix4f* matrix, Vector3f* translation, int rx, int ry, float sX, float sY, float sZ)
{
    matrix->setIdentity();
    matrix->translate(translation);
    Vector3f vec;

    //X, then Y. No Z

    vec.set(0, 1, 0);
    matrix->rotate(Maths::bamsToRad(ry), &vec);

    vec.set(1, 0, 0);
    matrix->rotate(Maths::bamsToRad(rx), &vec);

    vec.set(sX, sY, sZ);
    matrix->scale(&vec);
}

void Maths::createViewMatrix(Matrix4f* matrix, Camera* cam)
{
    //needed for arbitrary up vectors. shouldnt be needed for this
    //Matrix4f viewMatrix = Maths::lookAt(&cam->eye, &cam->target, &cam->up);
    //matrix->set(&viewMatrix);

    matrix->setIdentity();

    Vector3f vec(1, 0, 0);
    matrix->rotate(toRadians(cam->pitch), &vec);

    vec.set(0, 1, 0);
    matrix->rotate(toRadians(cam->yaw), &vec);

    Vector3f camPos(&cam->eye);
    camPos.neg();
    matrix->translate(&camPos);
}

Matrix4f Maths::lookAt(Vector3f* eye, Vector3f* focus, Vector3f* up)
{
    Vector3f f(focus);
    f = f-eye;
    f.normalize();

    Vector3f s = f.cross(up);
    s.normalize();

    Vector3f u = s.cross(&f);

    Matrix4f result;

    result.m00 =  s.x;
    result.m10 =  s.y;
    result.m20 =  s.z;
    result.m01 =  u.x;
    result.m11 =  u.y;
    result.m21 =  u.z;
    result.m02 = -f.x;
    result.m12 = -f.y;
    result.m22 = -f.z;
    result.m30 = -s.dot(eye);
    result.m31 = -u.dot(eye);
    result.m32 =  f.dot(eye);

    return result;
}

//Given two vectors that are perpencicular to each other, calculate
// a third vector that is perpendicular to both
Vector3f Maths::calcThirdAxis(Vector3f* A, Vector3f* B)
{
    //float third[3];
    //rotatePoint(third, 0, 0, 0, A->x, A->y, A->z, B->x, B->y, B->z, -(M_PI/2));

    //return Vector3f(third[0], third[1], third[2]);

    return B->cross(A);
}

//Given a basis {v1, v2, v3}, computes the coordinate vector
// of a point a in the new basis
Vector3f Maths::coordinatesRelativeToBasis(Vector3f* v1, Vector3f* v2, Vector3f* v3, Vector3f* a)
{
    // Set up the augmented matrix
    float A[3][4] = {{v1->x, v2->x, v3->x, a->x},
                     {v1->y, v2->y, v3->y, a->y},
                     {v1->z, v2->z, v3->z, a->z}};

    const int nrows = 3; // number of rows
    const int ncols = 4; // number of columns

    int lead = 0; 

    // Row reduce
    while (lead < nrows)
    {
        float d, m;

        for (int r = 0; r < nrows; r++)
        {
            d = A[lead][lead];
            m = A[r][lead] / A[lead][lead];

            for (int c = 0; c < ncols; c++)
            {
                if (r == lead)
                {
                    A[r][c] /= d;
                }
                else
                {
                    A[r][c] -= A[lead][c] * m;
                }
            }
        }

        lead++;
    }

    return Vector3f(A[0][3], A[1][3], A[2][3]);
}

//Returns the difference between the two angles
//ang1 and ang2 should be in degrees
float Maths::compareTwoAngles(float origAng1, float origAng2)
{
    float ang1 = fmodf(fmodf(origAng1, 360) + 360, 360);
    float ang2 = fmodf(fmodf(origAng2, 360) + 360, 360);

    float d = fmodf(fabsf(ang1 - ang2), 360);
    float r = d > 180 ? 360 - d : d;

    //calculate sign
    int sign = (ang1 - ang2 >= 0 && ang1 - ang2 <= 180) || (ang1 - ang2 <= -180 && ang1 - ang2 >= -360) ? 1 : -1;
    r *= sign;

    return r;
}

int Maths::sign(float value)
{
    //int v = *(int*)&value; //get bits of value casted as an int

    //return 1 - 2*((v>>31) & 0x01);

    if (value > 0)
    {
        return 1;
    }
    else if(value < 0)
    {
        return -1;
    }
    return 0;
}

int Maths::numDigits(int number)
{
    int numChars = (int)ceil(log10f((float)(number+1)));
    if (number == 0)
    {
        numChars = 1;
    }
    return numChars;
}

float Maths::approach(float initialValue, float terminalValue, float approachConstant, float timePassed)
{
    return ((initialValue-terminalValue)*powf(2.718281828459f, -approachConstant*timePassed) + terminalValue);
}

Vector3f Maths::applyDrag(Vector3f* velocity, float drag, float deltaTime)
{
    float length = velocity->length();
    if (length < 0.0001f)
    {
        return Vector3f(velocity);
    }
    float newLength = length*powf(2.718281828459f, drag*deltaTime);
    return velocity->scaleCopy(newLength/length);
}

float Maths::applyDrag(float velocity, float drag, float deltaTime)
{
    float length = fabsf(velocity);
    if (length < 0.0001f)
    {
        return velocity;
    }
    float newLength = length*powf(2.718281828459f, drag*deltaTime);
    return velocity*(newLength/length);
}

//angle in radians
Vector3f Maths::mapInputs3(float angle, float mag, Vector3f* VecC)
{
    angle = fmod(angle, (Maths::PI * 2));
    float tempx = cosf(angle)*mag;
    float tempz = sinf(angle)*mag;

    float CDir = atan2f(VecC->z, VecC->x);
    CDir += (Maths::PI / 2);
    float Cx = cosf(CDir);
    float Cz = sinf(CDir);

    float CDist = sqrtf(VecC->x*VecC->x + VecC->z*VecC->z);
    float CPitch = (Maths::PI / 2 + atan2f(VecC->y, CDist));

    float result[3] = { 0, 0, 0 }; //storage for the answer
    rotatePoint(result, 0, 0, 0, Cx, 0, Cz, tempx, 0, tempz, CPitch);

    Vector3f res(result[0], result[1], result[2]);

    return res;
}

//angle in radians
Vector3f Maths::mapCamera(float yaw, float pitch, float mag, Vector3f* VecC)
{
    yaw = fmod(yaw, (Maths::PI * 2));
    float tempx = cosf(yaw)*mag;
    float tempz = sinf(yaw)*mag;
    float tempy = 0;

    float rotAxisX = cosf(yaw + (Maths::PI / 2.0f));
    float rotAxisZ = sinf(yaw + (Maths::PI / 2.0f));

    pitch = fmod(pitch, (Maths::PI * 2));

    float result2[3] = { 0, 0, 0 };
    rotatePoint(result2, 0, 0, 0, rotAxisX, 0, rotAxisZ, tempx, tempy, tempz, pitch);
    tempx = result2[0];
    tempy = result2[1];
    tempz = result2[2];


    float CDir = atan2f(VecC->z, VecC->x);
    CDir += (Maths::PI / 2);
    float Cx = cosf(CDir);
    float Cz = sinf(CDir);

    float CDist = sqrtf(VecC->x*VecC->x + VecC->z*VecC->z);
    float CPitch = (Maths::PI / 2 + atan2f(VecC->y, CDist));

    float result[3] = { 0, 0, 0 }; //storage for the answer
    rotatePoint(result, 0, 0, 0, Cx, 0, Cz, tempx, tempy, tempz, CPitch);

    Vector3f res(result[0], result[1], result[2]);

    return res;
}

/**
* Calculate the x and z speeds relative to a plane based off
* the previous position you are coming in from
*
* @param xspd the x speed that you are going at before collision
* @param yspd the y speed that you are going at before collision
* @param zspd the z speed that you are going at before collision
* @param A the collision point on the triangle
* @param normal the normal of the triangle
*/
Vector3f Maths::calculatePlaneSpeed(float xspd, float yspd, float zspd, Vector3f* normal)
{
    Vector3f A(xspd, yspd, zspd);
    Vector3f Blue = projectOntoPlane(&A, normal);

    //Rotate normal along y axis 90 degrees
    float CDir = atan2f(-normal->z, -normal->x);
    CDir += (Maths::PI / 2);
    float Cx = cosf(CDir);
    float Cz = sinf(CDir);

    float CDist = sqrtf(normal->x*normal->x + normal->z*normal->z);
    float CPitch = ((Maths::PI / 2) + atan2f(-normal->y, CDist));

    float result[3] = { 0, 0, 0 }; //storage for the answer
    rotatePoint(result, 0, 0, 0, Cx, 0, Cz, Blue.x, Blue.y, Blue.z, -CPitch);

    return Vector3f(result[0], result[1], result[2]);
}

//Equation from https://sites.google.com/site/glennmurray/Home/rotation-matrices-and-formulas
//Point that axis goes through,
//direction of axis,
//point to rotate, 
//angle of rotation
void Maths::rotatePoint(float result[],
    float a, float b, float c,
    float u, float v, float w,
    float x, float y, float z,
    float theta)
{
    if (sqrtf(u*u + v*v + w*w) < 0.000000001f)
    {
        //std::fprintf(stdout, "Warning: trying to rotate by a very small axis [%f %f %f]\n", u, v, w);
        result[0] = x;
        result[1] = y;
        result[2] = z;
        return;
    }

    float u2 = u*u;
    float v2 = v*v;
    float w2 = w*w;
    float l2 = u2 + v2 + w2;
    float l = sqrtf(l2);

    float cosT = cosf(theta);
    float oneMinusCosT = 1 - cosT;
    float sinT = sinf(theta);

    result[0] = ((a*(v2 + w2) - u*(b*v + c*w - u*x - v*y - w*z)) * oneMinusCosT
        + l2*x*cosT
        + l*(-c*v + b*w - w*y + v*z)*sinT) / l2;

    result[1] = ((b*(u2 + w2) - v*(a*u + c*w - u*x - v*y - w*z)) * oneMinusCosT
        + l2*y*cosT
        + l*(c*u - a*w + w*x - u*z)*sinT) / l2;

    result[2] = ((c*(u2 + v2) - w*(a*u + b*v - u*x - v*y - w*z)) * oneMinusCosT
        + l2*z*cosT
        + l*(-b*u + a*v - v*x + u*y)*sinT) / l2;
}

//Point that axis goes through,
//direction of axis,
//point to rotate, 
//angle of rotation, in radians
Vector3f Maths::rotatePoint(
    Vector3f* pointToRotate,
    Vector3f* axisOfRotation,
    float theta)
{
    float result[3];
    Maths::rotatePoint(result, 0, 0, 0, 
        axisOfRotation->x, 
        axisOfRotation->y, 
        axisOfRotation->z, 
        pointToRotate->x, 
        pointToRotate->y, 
        pointToRotate->z, 
        theta);

    return Vector3f(result[0], result[1], result[2]);
}

Vector3f Maths::interpolateVector(Vector3f* A, Vector3f* B, float percent)
{
    Vector3f perpen = A->cross(B);
    float dotProduct = A->dot(B);
    float mag = A->length()*B->length();

    if (mag < 0.0000001f)
    {
        //std::fprintf(stdout, "Warning: Trying to interpolate between small vectors\n");
        return Vector3f(A);
    }

    if (dotProduct/mag > 0.99999f) //Vectors are extremely similar already, just return A
    {
        return Vector3f(A);
    }

    float angle = acos(dotProduct/mag);
    percent = fminf(1.0f, fmaxf(0.0f, percent));
    return Maths::rotatePoint(A, &perpen, angle*percent);
}

float Maths::angleBetweenVectors(Vector3f* A, Vector3f* B)
{
    float dotProduct = A->dot(B);
    float mag = A->length()*B->length();

    if (mag < 0.0000001f)
    {
        return 0;
    }

    if (dotProduct/mag > 0.99999f) //Vectors are extremely similar already, just return 0
    {
        return 0;
    }

    return acosf(dotProduct/mag);
}

Vector3f Maths::getCloserPoint(Vector3f* A, Vector3f* B, Vector3f* testPoint)
{
    Vector3f deltaA(A);
    deltaA = deltaA - testPoint;
    Vector3f deltaB(B);
    deltaB = deltaB - testPoint;
    float distA = deltaA.lengthSquared();
    float distB = deltaB.lengthSquared();
    if (distA < distB)
    {
        return Vector3f(A);
    }
    return Vector3f(B);
}

//https://stackoverflow.com/questions/11132681/what-is-a-formula-to-get-a-vector-perpendicular-to-another-vector
Vector3f Maths::calculatePerpendicular(Vector3f* vec)
{
    bool b0 = (vec->x <  vec->y) && (vec->x <  vec->z);
    bool b1 = (vec->y <= vec->x) && (vec->y <  vec->z);
    bool b2 = (vec->z <= vec->x) && (vec->z <= vec->y);
    Vector3f differentVec((float)(b0), (float)(b1), (float)(b2));
    differentVec.setLength(1);
    Vector3f perpen = vec->cross(&differentVec);
    perpen.setLength(1);
    return perpen;
}

Vector3f Maths::projectAlongLine(Vector3f* A, Vector3f* line)
{
    Vector3f master(A);
    //std::fprintf(stdout, "master = %f %f %f\n", master.x, master.y, master.z);
    Vector3f perpen1 = Maths::calculatePerpendicular(line);
    perpen1.normalize();
    //std::fprintf(stdout, "perpen1 = %f %f %f\n", perpen1.x, perpen1.y, perpen1.z);
    Vector3f perpen2 = perpen1.cross(line);
    perpen2.normalize();
    //std::fprintf(stdout, "perpen2 = %f %f %f\n", perpen2.x, perpen2.y, perpen2.z);
    master = Maths::projectOntoPlane(&master, &perpen1);
    //std::fprintf(stdout, "master = %f %f %f\n", master.x, master.y, master.z);
    master = Maths::projectOntoPlane(&master, &perpen2);
    //std::fprintf(stdout, "master = %f %f %f\n", master.x, master.y, master.z);
    return master;
}

/**
* @param initialVelocity
* @param surfaceNormal
* @param elasticity Scale of the resulting vector relative to the original velocity
*/
Vector3f Maths::bounceVector(Vector3f* initialVelocity, Vector3f* surfaceNormal, float elasticity)
{
    Vector3f twoNtimesVdotN(surfaceNormal);
    twoNtimesVdotN.scale(-2 * initialVelocity->dot(surfaceNormal));

    Vector3f Vnew = (twoNtimesVdotN + initialVelocity);
    Vnew.scale(elasticity);

    return Vnew;
}


//Projects vector A to be perpendicular to vector normal
Vector3f Maths::projectOntoPlane(Vector3f* A, Vector3f* normal)
{
    Vector3f B(0, 0, 0);
    Vector3f C(A);
    Vector3f N(normal->x, normal->y, normal->z);
    N.normalize();

    N.scale(C.dot(&N));
    B = C - N;

    return B;
}

/** Returns the point on a sphere that has the given angles from the center
* @param angH in radians
* @param angV in radians
* @param radius
* @return
*/
Vector3f Maths::spherePositionFromAngles(float angH, float angV, float radius)
{
    float y   = (radius*sinf(angV));
    float hpt = (radius*cosf(angV));

    float x = (hpt*cosf(angH));
    float z = (hpt*sinf(angH));

    return Vector3f(x, y, z);
}

Vector2f Maths::anglesFromDirection(Vector3f* dir)
{
    float yaw = atan2f(dir->z, dir->x);

    float hdist = sqrtf(dir->z*dir->z + dir->x*dir->x);
    float pitch = atan2f(dir->y, hdist);

    return Vector2f(yaw, pitch);
}

Vector3f Maths::randomPointOnSphere()
{
    float z   = Maths::nextUniform()*2 - 1;
    float lng = Maths::nextUniform()*2*Maths::PI;

    float radius = sqrtf(1-(z)*(z));

    float x = radius*cosf(lng);
    float y = radius*sinf(lng);

    return Vector3f(x, y, z);


    //Makes a 3D Bernoulli Lemniscate ???

    //float theta = 2 * M_PI * Maths::nextUniform();
    //float phi = acosf(1 - 2 * Maths::nextUniform());
    //float x = sinf(phi) * cosf(theta);
    //float y = sinf(phi) * sinf(theta);
    //float z = cosf(phi);

    //return spherePositionFromAngles(x, y, z);
}

float Maths::random()
{
    return (rand() % RAND_MAX) / ((float)(RAND_MAX));
}

float Maths::nextGaussian()
{
    return (*Maths::distributionNormal)(*Maths::generatorNormal);
}

float Maths::nextUniform()
{
    return (*Maths::distributionUniform)(*Maths::generatorUniform);
}

Vector4f Maths::calcPlaneValues(Vector3f* p1, Vector3f* p2, Vector3f* p3)
{
    Vector3f vec1(p1->x - p3->x, p1->y - p3->y, p1->z - p3->z);
    Vector3f vec2(p2->x - p3->x, p2->y - p3->y, p2->z - p3->z);

    Vector3f cross = vec1.cross(&vec2);

    float newD = cross.x*p3->x + cross.y*p3->y + cross.z*p3->z;

    return Vector4f(cross.x, cross.y, cross.z, -newD);
}

Vector4f Maths::calcPlaneValues(Vector3f* point, Vector3f* normal)
{
    Vector3f perp1 = Maths::calculatePerpendicular(normal);
    Vector3f perp2 = perp1.cross(normal);

    Vector3f p1(point);
    Vector3f p2(point);
    Vector3f p3(point);
    p2 = p2 + perp1;
    p3 = p3 + perp2;

    return Maths::calcPlaneValues(&p1, &p2, &p3);
}

Vector3f Maths::calcWorldSpaceDirectionVectorFromScreenSpaceCoords(float clickPosX, float clickPosY)
{
    int displayWidth;
    int displayHeight;
    glfwGetWindowSize(DisplayManager::getWindow(), &displayWidth, &displayHeight);
    float aspectRatio = ((float)displayWidth)/displayHeight;

    float normalizedX = (clickPosX-(displayWidth/2))/((displayWidth/2));
    float normalizedY = (clickPosY-(displayHeight/2))/((displayHeight/2));

    float frustrumLengthY = MasterRenderer::NEAR_PLANE*tanf(Maths::toRadians(MasterRenderer::getVFOV()/2));
    float frustrumLengthX = aspectRatio*frustrumLengthY;

    float cameraSpaceCoordX = normalizedX*frustrumLengthX;
    float cameraSpaceCoordY = -normalizedY*frustrumLengthY;
    float cameraSpaceCoordZ = -MasterRenderer::NEAR_PLANE;

    Vector3f cameraSpaceDirection(
            cameraSpaceCoordX,
            cameraSpaceCoordY,
            cameraSpaceCoordZ);
    cameraSpaceDirection.normalize();

    Vector3f xAxis(-1, 0, 0);
    Vector3f yAxis(0, -1, 0);

    Vector3f worldSpaceOffset = Maths::rotatePoint(&cameraSpaceDirection, &xAxis, Maths::toRadians(Global::gameCamera->pitch));

    return Maths::rotatePoint(&worldSpaceOffset, &yAxis, Maths::toRadians(Global::gameCamera->yaw));
}

//coordinates are 0,0 for middle of screen, -1, -1 for top left
Vector2f Maths::calcScreenCoordsOfWorldPoint(Vector3f* worldPoint)
{
    int displayWidth;
    int displayHeight;
    glfwGetWindowSize(DisplayManager::getWindow(), &displayWidth, &displayHeight);
    float aspectRatio = ((float)displayWidth)/displayHeight;
    float frustrumLengthY = 2*MasterRenderer::NEAR_PLANE*tanf(Maths::toRadians(MasterRenderer::getVFOV()/2));
    float frustrumLengthX = aspectRatio*frustrumLengthY;

    //first translate so that camera is in the origin
    Vector3f cameraSpaceCoord(worldPoint);
    cameraSpaceCoord = cameraSpaceCoord - Global::gameCamera->eye;

    //then, rotate so that things are back to if no cam rotation
    float yaw = Maths::toRadians(Global::gameCamera->yaw);
    float pitch = Maths::toRadians(Global::gameCamera->pitch);

    Vector3f xAxis(-1, 0, 0);
    Vector3f yAxis(0, -1, 0);

    cameraSpaceCoord = Maths::rotatePoint(&cameraSpaceCoord, &yAxis, -yaw);
    cameraSpaceCoord = Maths::rotatePoint(&cameraSpaceCoord, &xAxis, -pitch);

    //check if the point is not going to be visible
    if (cameraSpaceCoord.z > -MasterRenderer::NEAR_PLANE)
    {
        return Vector2f(-100, -100);
    }

    //now do trig to figure coordinates on the frustrum plane, and normalize
    float hangle = atan2f(cameraSpaceCoord.x, -cameraSpaceCoord.z);
    float xoff = (tanf(hangle)*MasterRenderer::NEAR_PLANE)/(frustrumLengthX/2);

    float vangle = atan2f(cameraSpaceCoord.y, -cameraSpaceCoord.z);
    float yoff = (tanf(vangle)*MasterRenderer::NEAR_PLANE)/(frustrumLengthY/2);

    return Vector2f(xoff, yoff);
}
