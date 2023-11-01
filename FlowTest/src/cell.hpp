// Franck Limtung
#ifndef CELL_HPP
#define CELL_HPP

#include <vector>
#include "boid.hpp"

struct Cell
{
	Cell(int row, int col) : row(row), col(col) {}
	std::vector<Boid*> boids;
	int row, col;
};

#endif
