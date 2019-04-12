#include <glad/glad.h>

#include "../toolbox/vector.h"
#include "camera.h"
#include "../toolbox/maths.h"


Camera::Camera()
{
	//eye.set(-80, 50, 80);
    //yaw = 45;
    //pitch = 25;

	eye.set(0, 0, 0);
    yaw = 0;
    pitch = 0;
}

void Camera::refresh()
{
    /*
	Vector3f off;
	off = target-eye;

	off.normalize();
	off.scale(1.7f);
	fadePosition1.set(eye.x + off.x, eye.y + off.y, eye.z + off.z);

	off.normalize();
	off.scale(1.55f);
	fadePosition2.set(eye.x + off.x, eye.y + off.y, eye.z + off.z);

	extern float dt;
	vel = (eye-eyePrevious);
	vel.scale(1/dt);
	eyePrevious.set(&eye);
    */
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

Vector3f* Camera::getFadePosition1()
{
	return &fadePosition1;
}

Vector3f* Camera::getFadePosition2()
{
	return &fadePosition2;
}

//void Camera::setViewMatrixValues(Vector3f* newEye, Vector3f* newTarget, Vector3f* newUp)
//{
//	eye.set(newEye);
//	target.set(newTarget);
//	up.set(newUp);
//}
