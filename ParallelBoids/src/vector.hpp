// Franck Limtung
// Simple 2d vector class which supports adding vectors together
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cmath>
#include "fastTrig.hpp"

struct Vector
{
	Vector(float mag, float direction) : magnitude(mag), direction(direction) {}
	float magnitude;
	float direction;
	float getXComp() const { return magnitude * std::cos(direction); }
	float getYComp() const { return magnitude * std::sin(direction); }

	Vector& operator+=(const Vector& second) {
		float firstX = magnitude * std::cos(direction);
		float firstY = magnitude * std::sin(direction);
		float secondX = second.magnitude * std::cos(second.direction);
		float secondY = second.magnitude * std::sin(second.direction);

		float newX = firstX + secondX;
		float newY = firstY + secondY;

		this->magnitude = std::sqrt(newX * newX + newY * newY);
		this->direction = FastArcTan2(newY, newX);
		return *this;
	}
};

#endif
