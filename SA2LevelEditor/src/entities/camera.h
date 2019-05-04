#ifndef CAMERA_H
#define CAMERA_H

#include "../toolbox/vector.h"

class Camera
{
public:
	Vector3f eye;
    float yaw; //in degrees
    float pitch; //in degrees

	Camera();

    Camera(Camera* other);

    void reset();

    bool equals(Camera* other);

    Vector3f calcForward();

    Vector3f calcUp();

    Vector3f calcRight();
};
#endif
