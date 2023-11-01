#include "angle.hpp"

void Angle::changeAngle(float value) {
	angle += value;
	while (angle > 180) angle -= 360;
	while (angle < -179) angle += 360;
}
