#include "rotationVelocity.hpp"

void RotationVelocity::changeAngle(float value) {
	desiredAngle += value;
	while (desiredAngle > 180) desiredAngle -= 360;
	while (desiredAngle < -179) desiredAngle += 360;
}
