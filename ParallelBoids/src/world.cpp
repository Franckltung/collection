#include <iostream>
#include "oneapi/tbb.h"
#include "world.hpp"

std::vector<Boid*> World::getNearBoids(int row, int col, float x, float y) {
	// Look at all cells in 3x3 grid around boid and add them to list
	std::vector<Boid*> nearbys;
	nearbys.reserve(32);

	for (int i = row - 1; i < row + 2; i++) {
		if (i < 0 || i > rowMax) continue;
		for (int i2 = col - 1; i2 < col + 2; i2++) {
			if (i2 < 0 || i2 > colMax) continue;
			// Else
			Cell* cell = world[i][i2];
			for (Boid* boid : cell->boids) {
				if (std::pow(boid->x - x, 2) + std::pow(boid->y - y, 2) < std::pow(visionRange, 2)) {
					nearbys.push_back(boid);
				}
			}
		}
	}

	return nearbys; // Should move the constructed vector, not copy
}

void World::clearCells() {
	for (int i = 0; i < world.size(); i++) {
		for (int i2 = 0; i2 < world[i].size(); i2++) {
			world[i][i2]->boids.clear();
		}
	}
}

void World::printWorld() {
	for (int i = 0; i < world.size(); i++) {
		for (int i2 = 0; i2 < world[i].size(); i2++) {
			if (world[i][i2]->boids.size() > 0) std::cout << "B ";
			else std::cout << ". ";
		}
		std::cout << "\n";
	}
	std::cout << "\033[2J\033[1;1H";
}

void World::rebuildWorld(int cellSize, int row, int col, int edgeMargin) {
	this->cellSize = cellSize;
	rowMax = row - 1;
	colMax = col - 1;
	this->edgeMargin = edgeMargin;

	// Delete old cells
	for (int i = world.size() - 1; i >= 0; i--) {
		for (int j = world[i].size() - 1; j >= 0; j--) {
			delete world[i][j];
		}
	}

	world.clear();
	// Create new cells
	for (int i = 0; i < row; i++) {
		world.push_back(std::vector<Cell*>());
		for (int j = 0; j < col; j++) {
			world.back().push_back(new Cell(i, j));
		}
	}
}
