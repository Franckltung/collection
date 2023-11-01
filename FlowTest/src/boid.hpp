// Franck Limtung
#ifndef BOID_HPP
#define BOID_HPP
#include <vector>
#include "vector.hpp"
#include "imGuiSystem.hpp"

struct Boid
{
	Boid(float x, float y, float speed, float direction, const GUIState& guiState) : x(x), y(y), vec(speed, direction), maxSpeed(guiState.boidMaxSpeed) {}
	float x = 0;
	float y = 0;
	Vector vec;
	float maxSpeed = 0; // Top speed
	std::vector<Boid*> nearbyBoids; // Boids within a grid length of this boid
};

#endif
