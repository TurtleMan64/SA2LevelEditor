#ifndef CAMERA_H
#define CAMERA_H

#include "../toolbox/vector.h"

class Camera
{
public:
	Vector3f fadePosition1;
	Vector3f fadePosition2;

	Vector3f eye;
    float yaw; //in degrees
    float pitch; //in degrees

	Camera();

	//Call this ONCE per frame, it calculates velocity and other things
	void refresh();

    void reset();

	Vector3f* getFadePosition1();

	Vector3f* getFadePosition2();

    Vector3f calcForward();

    Vector3f calcUp();

    Vector3f calcRight();

	//void setViewMatrixValues(Vector3f* newEye, Vector3f* newTarget, Vector3f* newUp);
};
#endif
