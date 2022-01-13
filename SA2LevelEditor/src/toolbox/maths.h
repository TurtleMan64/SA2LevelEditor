#ifndef MATHS_H
#define MATHS_H

class Vector3f;
class Vector2f;
class Camera;

#include <random>
#include <chrono>
#include "matrix.h"

class Maths
{
private:
    static std::mt19937* generatorUniform;
    static std::uniform_real_distribution<float>* distributionUniform;

    static std::default_random_engine* generatorNormal;
    static std::normal_distribution<float>* distributionNormal;

public:
    static const float PI;

    static float toRadians(float deg);

    static float bamsToRad(int bams);

    static float bamsToRad(signed short bams);

    static float toDegrees(float rad);

    static float bamsToDeg(int bams);

    static int radToBams(float rad);

    static int degToBams(float deg);

    //Blank Matrix4f should have already been created. This function does not allocate any memory to matrix.
    //static void createTransformationMatrix(Matrix4f* result, Vector3f* translation, float rX, float rY, float rZ, float scale);

    //static void createTransformationMatrix(Matrix4f* result, Vector3f* translation, float rX, float rY, float rZ, float sX, float sY, float sZ);

    //For 2D GUI Images
    static void createTransformationMatrix(Matrix4f* result, Vector2f* translation, float roation, Vector2f* scale);

    static void createTransformationMatrixYXZ(Matrix4f* result, Vector3f* translation, int rX, int rY, int rZ, float sX, float sY, float sZ);

    static void createTransformationMatrixZXY(Matrix4f* result, Vector3f* translation, int rX, int rY, int rZ, float sX, float sY, float sZ);

    static void createTransformationMatrixXZY(Matrix4f* result, Vector3f* translation, int rX, int rY, int rZ, float sX, float sY, float sZ);

    static void createTransformationMatrixZY(Matrix4f* result, Vector3f* translation, int rY, int rZ, float sX, float sY, float sZ);

    static void createTransformationMatrixXY(Matrix4f* result, Vector3f* translation, int rX, int rY, float sX, float sY, float sZ);

    static void createViewMatrix(Matrix4f* result, Camera* cam);

    //Returns the difference between the two angles
    //ang1 and ang2 should be in degrees
    static float compareTwoAngles(float ang1, float ang2);

    //assumes the size of int and float are 32 bits
    static int sign(float value);

    //calculates the number of base 10 decimal digits in the number. 
    // examples: numDigits(38)    = 2  
    //           numDigits(68932) = 5
    static int numDigits(int number);

    //
    static float approach(float initialValue, float terminalValue, float approachConstant, float timePassed);

    //result needs to be array of 3 doubles
    //theta is in radians
    static void rotatePoint(float result[],
        float a, float b, float c,
        float u, float v, float w,
        float x, float y, float z,
        float theta);

    //Point that axis goes through,
    //direction of axis,
    //point to rotate, 
    //angle of rotation, in radians
    static Vector3f rotatePoint(
        Vector3f* pointToRotate,
        Vector3f* axisOfRotation,
        float theta);

    //Given two vectors, linear rotate from the A to B by percent and return that new vector.
    //If the two vectors are too small or are too similar already, a copy of A is retured.
    static Vector3f interpolateVector(Vector3f* A, Vector3f* B, float percent);

    //calculates the angle in radians between two vectors
    static float angleBetweenVectors(Vector3f* A, Vector3f* B);

    //given two points A and B, returns which one is closer to a point Test
    static Vector3f getCloserPoint(Vector3f* A, Vector3f* B, Vector3f* testPoint);

    //returns a new velocity vector that has undergone continous drag over a period of time. 
    //drag should be a negative number to actually drag the velocity vector
    static Vector3f applyDrag(Vector3f* velocity, float drag, float deltaTime);

    //returns a new velocity that has undergone continous drag over a period of time. 
    //drag should be a negative number to actually drag the velocity
    static float applyDrag(float velocity, float drag, float deltaTime);

    //angle in radians
    static Vector3f mapInputs3(float angle, float mag, Vector3f* VecC);

    //in radians
    static Vector3f mapCamera(float yaw, float pitch, float mag, Vector3f* VecC);


    static Vector3f bounceVector(Vector3f* initialVelocity, Vector3f* surfaceNormal, float elasticity);

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
    static Vector3f calculatePlaneSpeed(float xspd, float yspd, float zspd, Vector3f* normal);

    /** Returns the point on a sphere that has the given angles from the center
    * @param angH in radians
    * @param angV in radians
    * @param radius
    * @return
    */
    static Vector3f spherePositionFromAngles(float angH, float angV, float radius);

    //gives the yaw and pitch (in radians) of a direction vector
    static Vector2f anglesFromDirection(Vector3f* direction);

    //Generates a uniformly distributed random position on a sphere of radius 1
    static Vector3f randomPointOnSphere();

    static Vector3f projectOntoPlane(Vector3f* A, Vector3f* normal);

    //projects a vector along a line
    static Vector3f projectAlongLine(Vector3f* A, Vector3f* line);

    //calculates an arbitrary vector that is perpendicular to the given vector vec
    static Vector3f calculatePerpendicular(Vector3f* vec);

    //returns uniform random float >= 0 and < 1
    static float random();

    //normal distribution mean = 0, std dev = 1
    static float nextGaussian();

    //returns uniform random float >= 0 and < 1
    static float nextUniform();

    //Creates a view matrix that is centered at the eye position, and looking at focus
    static Matrix4f lookAt(Vector3f* eye, Vector3f* focus, Vector3f* up);

    //Given two vectors that are perpencicular to each other, calculate
    // a third vector that is perpendicular to both
    static Vector3f calcThirdAxis(Vector3f* A, Vector3f* B);

    //Calculate the A B C and D values for a plane from a normal and a point
    static Vector4f calcPlaneValues(Vector3f* point, Vector3f* normal);

    //Calculate the A B C and D values for a plane from 3 points
    static Vector4f calcPlaneValues(Vector3f* p1, Vector3f* p2, Vector3f* p3);

    //Given a basis {v1, v2, v3}, computes the coordinate vector
    // of a point a in the new basis
    static Vector3f coordinatesRelativeToBasis(Vector3f* v1, Vector3f* v2, Vector3f* v3, Vector3f* a);

    //When you click on the screen, you are given the x and y position of the click,
    // in screen space. This function will take those coordinates and convert them into
    // a 3D vector in world space that represents a line that goes directly from the
    // camera's eye to the place you clicked. This can then be used to do things
    // like place an object where the click happend, or do a collision check.
    static Vector3f calcWorldSpaceDirectionVectorFromScreenSpaceCoords(float clickPosX, float clickPosY);

    static Vector2f calcScreenCoordsOfWorldPoint(Vector3f* worldPoint);
};

#endif
