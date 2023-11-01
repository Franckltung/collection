// Franck Limtung
// The world consists of a matrix of cells. Boids only check their occupied cell and nearby ones
#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include "cell.hpp"
#include "boid.hpp"

class World
{
	public:
		World(int cellSize, int row, int col, int visionRange, int edgeMargin) : cellSize(cellSize), rowMax(row - 1), colMax(col - 1), visionRange(visionRange), edgeMargin(edgeMargin) {
			for (int i = 0; i < row; i++) {
				world.push_back(std::vector<Cell*>());
				for (int i2 = 0; i2 < col; i2++) {
					world.back().push_back(new Cell(i, i2));
				}
			}
		}
		std::vector<Boid*> getNearBoids(int row, int col, float x, float y);
		Cell* getCell(int row, int col) { return world.at(row).at(col); }
		int getCellSize() { return cellSize; }
		int getRowNum() { return rowMax + 1; }
		int getColNum() { return colMax + 1; }
		int getVisionRange() { return visionRange; }
		void setVisionRange(int visionRange) { this->visionRange = visionRange; }
		int getEdgeMargin() { return edgeMargin; }

		void clearCells(); // Remove all boids; for now I think it will be fine to do this every epoch
		void printWorld(); // Simple map visualizer before I implement SDL

		void rebuildWorld(int cellSize, int row, int col, int edgeMargin);
	private:
		std::vector<std::vector<Cell*>> world;
		int cellSize = 0;
		int rowMax = 0;
		int colMax = 0;
		int visionRange = 0; // Vision range of boids
		int edgeMargin = 0; // Distance when boids will bounce off walls in CELLS
};

#endif
