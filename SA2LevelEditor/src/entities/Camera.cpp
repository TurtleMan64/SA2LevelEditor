#include <glad/glad.h>

#include "../toolbox/vector.h"
#include "camera.h"
#include "../toolbox/maths.h"


Camera::Camera()
{
	eye.set(-80, 50, 80);
    yaw = 45;
    pitch = 25;

	//eye.set(0, 0, 0);
    //yaw = 0;
    //pitch = 0;
}

Camera::Camera(Camera* other)
{
    eye.set(&other->eye);
    pitch = other->pitch;
    yaw = other->yaw;
}

void Camera::reset()
{
    eye.set(-80, 50, 80);
    yaw = 45;
    pitch = 25;
}

bool Camera::equals(Camera* other)
{
    return ((eye.x == other->eye.x) &&
            (eye.y == other->eye.y) &&
            (eye.z == other->eye.z) &&
            (pitch == other->pitch) &&
            (yaw   == other->yaw));
}

Vector3f Camera::calcForward()
{
    Vector3f forward(
		-cosf(Maths::toRadians(yaw + 90))*((cosf(Maths::toRadians(pitch)))),
		 sinf(Maths::toRadians(pitch + 180)),
		-sinf(Maths::toRadians(yaw + 90))*((cosf(Maths::toRadians(pitch)))));
    forward.normalize();
    return forward;
}

Vector3f Camera::calcUp()
{
    Vector3f up(
		-cosf(Maths::toRadians(yaw + 90))*((cosf(Maths::toRadians(pitch + 90)))),
		 sinf(Maths::toRadians(pitch + 180 + 90)),
		-sinf(Maths::toRadians(yaw + 90))*((cosf(Maths::toRadians(pitch + 90)))));
    up.normalize();
    return up;
}

Vector3f Camera::calcRight()
{
    Vector3f forward = calcForward();
    Vector3f up = calcUp();
    return up.cross(&forward);
}
