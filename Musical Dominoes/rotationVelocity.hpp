#ifndef ROTATION_VELOCITY_HPP
#define ROTATION_VELOCITY_HPP

struct RotationVelocity
{
	float speed = 7;
	float desiredAngle = 0;
	void changeAngle(float value); // Modify the angle by a value and handle overflows in either direction
};

#endif
