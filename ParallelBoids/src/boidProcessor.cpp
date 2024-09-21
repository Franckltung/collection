#include <cmath>
#include <iostream>
#include "oneapi/tbb.h"
#include "boidProcessor.hpp"

void BoidProcessor::process(std::vector<Boid*>& boids) {
	processVision(boids);
	processNearBounds(boids);
	processSeparation(boids);
	processAlignment(boids);
	processCohesion(boids);
	processMovement(boids);
	processBounds(boids); // Note: must happen after movement, or we break the world boundary!
}

void BoidProcessor::processVision(std::vector<Boid*>& boids) {
	// Get all cells within 3x3 grid around boid
	for (Boid* boid : boids) {
		Cell* currCell = world.getCell(boid->x / world.getCellSize(), boid->y / world.getCellSize());
		boid->nearbyBoids = world.getNearBoids(currCell->row, currCell->col, boid->x, boid->y);
	}
}

void BoidProcessor::processVisionParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			Cell* currCell = world.getCell(boids[i]->x / world.getCellSize(), boids[i]->y / world.getCellSize());
			boids[i]->nearbyBoids = world.getNearBoids(currCell->row, currCell->col, boids[i]->x, boids[i]->y);
		}
	});
}

void BoidProcessor::processBounds(std::vector<Boid*>& boids) {
	// Just stop at an edge, the near bounds rule will eventually turn it around
	for (Boid* boid : boids) {
		if (boid->x < 0) boid->x = 0;
		else if (boid->x >= world.getColNum() * world.getCellSize()) boid->x = world.getRowNum() * world.getCellSize() - 1;
		if (boid->y < 0) boid->y = 0;
		else if (boid->y >= world.getRowNum() * world.getCellSize()) boid->y = world.getColNum() * world.getCellSize() - 1;
	}
}

void BoidProcessor::processBoundsParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			Boid* boid = boids[i];
			if (boid->x < 0) boid->x = 0;
			else if (boid->x >= world.getColNum() * world.getCellSize()) boid->x = world.getRowNum() * world.getCellSize() - 1;
			if (boid->y < 0) boid->y = 0;
			else if (boid->y >= world.getRowNum() * world.getCellSize()) boid->y = world.getColNum() * world.getCellSize() - 1;
		}
	});
}

void BoidProcessor::processNearBounds(std::vector<Boid*>& boids) {
	int edge = world.getEdgeMargin() * world.getCellSize();
	// Just stop at an edge, the near bounds rule will eventually turn it around
	for (Boid* boid : boids) {
		if (boid->x < 0 + edge) boid->vec += Vector(guiState.impulseNearBoundsStr, 0);
		else if (boid->x >= world.getColNum() * world.getCellSize() - edge) boid->vec += Vector(guiState.impulseNearBoundsStr, M_PI);
		if (boid->y < 0 + edge) boid->vec += Vector(guiState.impulseNearBoundsStr, M_PI / 2.0);
		else if (boid->y >= world.getRowNum() * world.getCellSize() - edge) boid->vec += Vector(guiState.impulseNearBoundsStr, 3.0 * M_PI / 2.0);
	}
}

void BoidProcessor::processNearBoundsParallel(std::vector<Boid*>& boids) {
	int edge = world.getEdgeMargin() * world.getCellSize();

	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			Boid* boid = boids[i];
			if (boid->x < 0 + edge) boid->vec += Vector(guiState.impulseNearBoundsStr, 0);
			else if (boid->x >= world.getColNum() * world.getCellSize() - edge) boid->vec += Vector(guiState.impulseNearBoundsStr, M_PI);
			if (boid->y < 0 + edge) boid->vec += Vector(guiState.impulseNearBoundsStr, M_PI / 2.0);
			else if (boid->y >= world.getRowNum() * world.getCellSize() - edge) boid->vec += Vector(guiState.impulseNearBoundsStr, 3.0 * M_PI / 2.0);
		}
	});
}

void BoidProcessor::processSeparation(std::vector<Boid*>& boids) {
	for (Boid* boid : boids) {
		std::vector<Boid*>& neighbours = boid->nearbyBoids;
		for (Boid* neighbour : neighbours) {
			// Skip self
			if (neighbour == boid) continue;

			// Get distance and separate if needed
			float distanceX = neighbour->x - boid->x;
			float distanceY = neighbour->y - boid->y;
			if (std::pow(distanceX, 2) + std::pow(distanceY, 2) < std::pow(guiState.boidProtectionRange, 2)) {
				boid->vec += Vector(guiState.impulseSeparationStr, FastArcTan2(distanceY, distanceX) + M_PI);
			}
		}
	}
}

void BoidProcessor::processSeparationParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			std::vector<Boid*>& neighbours = boids[i]->nearbyBoids;
			for (Boid* neighbour : neighbours) {
				if (neighbour == boids[i]) continue;

				float distanceX = neighbour->x - boids[i]->x;
				float distanceY = neighbour->y - boids[i]->y;
				if (std::pow(distanceX, 2) + std::pow(distanceY, 2) < std::pow(guiState.boidProtectionRange, 2)) {
					boids[i]->vec += Vector(guiState.impulseSeparationStr, FastArcTan2(distanceY, distanceX) + M_PI);
				}
			}
		}
	});
}

void BoidProcessor::processAlignment(std::vector<Boid*>& boids) {
	// Try to match vector of neighbours
	for (Boid* boid : boids) {
		std::vector<Boid*> neighbours = boid->nearbyBoids;
		if (neighbours.size() <= 1) continue; // Skip if we don't have any neighbours

		Vector averageDirection(0, 0);
		for (Boid* neighbour : neighbours) {
			if (neighbour == boid) continue;
			averageDirection += Vector(1, neighbour->vec.direction);
		}

		boid->vec += Vector(guiState.impulseAlignStr, averageDirection.direction);
	}
}

void BoidProcessor::processAlignmentParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			std::vector<Boid*> neighbours = boids[i]->nearbyBoids;
			if (neighbours.size() <= 1) continue;

			Vector averageDirection(0, 0);
			for (Boid* neighbour : neighbours) {
				if (neighbour == boids[i]) continue;
				averageDirection += Vector(1, neighbour->vec.direction);
			}

			boids[i]->vec += Vector(guiState.impulseAlignStr, averageDirection.direction);
		}
	});
}

void BoidProcessor::processCohesion(std::vector<Boid*>& boids) {
	// Try to fly towards center of flock
	for (Boid* boid : boids) {
		std::vector<Boid*> neighbours = boid->nearbyBoids;
		if (neighbours.size() <= 1) continue; // Skip if we don't have any neighbours

		float averageX = 0;
		float averageY = 0;
		for (Boid* neighbour : neighbours) {
			averageX += neighbour->x;
			averageY += neighbour->y;
		}
		averageX /= neighbours.size();
		averageY /= neighbours.size();

		boid->vec += Vector(guiState.impulseCohesionStr, FastArcTan2(averageY - boid->y, averageX - boid->x));
	}
}

void BoidProcessor::processCohesionParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			std::vector<Boid*> neighbours = boids[i]->nearbyBoids;
			if (neighbours.size() <= 1) continue;

			float averageX = 0;
			float averageY = 0;
			for (Boid* neighbour : neighbours) {
				averageX += neighbour->x;
				averageY += neighbour->y;
			}
			averageX /= neighbours.size();
			averageY /= neighbours.size();

			boids[i]->vec += Vector(guiState.impulseCohesionStr, FastArcTan2(averageY - boids[i]->y, averageX - boids[i]->x));
		}
	});
}

void BoidProcessor::processMovement(std::vector<Boid*>& boids) {
	for (Boid* boid : boids) {
		Vector& vec = boid->vec;
		// Apply max speed constraint; this shoudn't change direction at all
		if (vec.magnitude > guiState.boidMaxSpeed) vec.magnitude = guiState.boidMaxSpeed;
		boid->x += vec.getXComp();
		boid->y += vec.getYComp();

		// Apply drag
		vec.magnitude *= guiState.drag;
		// Apply forward motion
		if (vec.magnitude < guiState.boidMinSpeed) vec.magnitude += guiState.impulseForwardStr;
	}
}

void BoidProcessor::processMovementParallel(std::vector<Boid*>& boids) {
	tbb::parallel_for(tbb::blocked_range<size_t>{0, boids.size()}, [&](const tbb::blocked_range<size_t>& r) {
		for (size_t i = r.begin(); i < r.end(); i++) {
			Vector& vec = boids[i]->vec;
			if (vec.magnitude > guiState.boidMaxSpeed) vec.magnitude = guiState.boidMaxSpeed;
			boids[i]->x += vec.getXComp();
			boids[i]->y += vec.getYComp();

			vec.magnitude *= guiState.drag;
			if (vec.magnitude < guiState.boidMinSpeed) vec.magnitude += guiState.impulseForwardStr;
		}
	});
}
