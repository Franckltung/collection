// Franck Limtung
#ifndef BOID_PROCESSOR_HPP
#define BOID_PROCESSOR_HPP

#include "world.hpp"
#include "boid.hpp"

class BoidProcessor
{
	public:
		BoidProcessor(World& world, GUIState& guiState) : world(world), guiState(guiState) {}

		void process(std::vector<Boid*>& boids);
		void processVision(std::vector<Boid*>& boids);
		void processBounds(std::vector<Boid*>& boids); // Prevent escaping world
		void processNearBounds(std::vector<Boid*>& boids); // Avoid escaping world
		void processSeparation(std::vector<Boid*>& boids);
		void processAlignment(std::vector<Boid*>& boids);
		void processCohesion(std::vector<Boid*>& boids);
		void processMovement(std::vector<Boid*>& boids);

		void processVisionParallel(std::vector<Boid*>& boids);
		void processBoundsParallel(std::vector<Boid*>& boids);
		void processNearBoundsParallel(std::vector<Boid*>& boids);
		void processSeparationParallel(std::vector<Boid*>& boids);
		void processAlignmentParallel(std::vector<Boid*>& boids);
		void processCohesionParallel(std::vector<Boid*>& boids);
		void processMovementParallel(std::vector<Boid*>& boids);
	private:
		World& world;
		GUIState& guiState;
};

#endif
