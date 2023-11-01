// Franck Limtung
// 2023-Oct
// TODO: Fine grained parallism. We should try to pass each individual boid from node to node and see if
// it runs faster
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <chrono>
#include <random>
#include <thread>
#include "oneapi/tbb.h"
#include "world.hpp"
#include "boidProcessor.hpp"
#include "simpleRenderer.hpp"
#include "timer.hpp"
#include "imGuiSystem.hpp"
#include "imgui/imgui_impl_sdl.h"

int main (int argc, char* argv[]) {
	bool doParallel = false;
	if (argc > 1) {
		if (strcmp(argv[1], "--doParallel") == 0) doParallel = true;
	}

	std::mt19937_64 seedy(std::random_device{}());

	// Set up GUI
	ImGuiSystem guiSystem;
	GUIState& guiState = guiSystem.getState();

	std::uniform_real_distribution<float> posDistrib(0, guiState.newCellNum * guiState.newCellSize); // We use a real distribution because it is [minVal,maxValue), not closed
	std::uniform_real_distribution<float> directionDistrib(0, 2 * M_PI);
	std::uniform_real_distribution<float> speedDistrib(guiState.boidMinSpeed, guiState.boidMaxSpeed);
	tbb::flow::graph graph;

	World world(guiState.newCellSize, guiState.newCellNum, guiState.newCellNum, guiState.boidVisionRange, guiState.newEdgeMargin);
	std::vector<Boid*> boids;
	for (int i = 0; i < guiState.boidNumber; i++) {
		boids.push_back(new Boid(posDistrib(seedy), posDistrib(seedy), speedDistrib(seedy), directionDistrib(seedy), guiState));
	}

	BoidProcessor boidProcessor(world, guiState);

	// Set up screen
	int virtualWindowWidth = guiState.newCellNum * guiState.newCellSize; // Scale based on size and number of cells
	SimpleRenderer renderer("FlowTest", 640, 640, 640.0 / virtualWindowWidth);

	std::vector<RenderRect> renderShapes;
	for (Boid* boid : boids) {
		renderShapes.push_back(RenderRect(boid->x, boid->y, 8, 8, true, 255, 255, 255));
	}

	// Setup nodes
	tbb::flow::broadcast_node<tbb::flow::continue_msg> startNode(graph);
	tbb::flow::continue_node<tbb::flow::continue_msg> renderClearNode(graph, [&](tbb::flow::continue_msg) {
		renderer.renderClear();
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> updateRenderShapesNode(graph, [&](tbb::flow::continue_msg) {
		for (unsigned int i = 0; i < boids.size(); i++) {
			renderShapes[i].x = boids[i]->x;
			renderShapes[i].y = boids[i]->y;
		}
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> populateCellsNode(graph, [&](tbb::flow::continue_msg) {
		for (Boid* boid : boids) {
			Cell* currCell = world.getCell(boid->x / world.getCellSize(), boid->y / world.getCellSize());
			currCell->boids.push_back(boid);
		}
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> drawRectsNode(graph, [&](tbb::flow::continue_msg) {
		renderer.drawRects(renderShapes);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> clearCellsNode(graph, [&](tbb::flow::continue_msg) {
		world.clearCells();
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processVisionNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processVisionParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processBoundsNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processBoundsParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processNearBoundsNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processNearBoundsParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processSeparationNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processSeparationParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processAlignmentNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processAlignmentParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processCohesionNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processCohesionParallel(boids);
	});
	tbb::flow::continue_node<tbb::flow::continue_msg> processMovementNode(graph, [&](tbb::flow::continue_msg) {
		boidProcessor.processMovementParallel(boids);
	});

	tbb::flow::make_edge(startNode, renderClearNode);
	tbb::flow::make_edge(startNode, clearCellsNode);
	tbb::flow::make_edge(startNode, processNearBoundsNode);
	tbb::flow::make_edge(clearCellsNode, populateCellsNode);
	tbb::flow::make_edge(populateCellsNode, processVisionNode);
	tbb::flow::make_edge(processVisionNode, processSeparationNode);
	tbb::flow::make_edge(processVisionNode, processAlignmentNode);
	tbb::flow::make_edge(processVisionNode, processCohesionNode);
	tbb::flow::make_edge(processSeparationNode, processMovementNode);
	tbb::flow::make_edge(processAlignmentNode, processMovementNode);
	tbb::flow::make_edge(processCohesionNode, processMovementNode);
	tbb::flow::make_edge(processNearBoundsNode, processMovementNode);
	tbb::flow::make_edge(processMovementNode, processBoundsNode);
	tbb::flow::make_edge(processBoundsNode, updateRenderShapesNode);
	tbb::flow::make_edge(updateRenderShapesNode, drawRectsNode);
	tbb::flow::make_edge(renderClearNode, drawRectsNode);

	// Setup timers
	TimerSystem timerSystem;
	int epochNumber = 0;
	int maxEpochNumber = -1; // maximum number of epochs; if less than 0 it will run forever
	timerSystem.startTimer("Total Timer");

	bool quit = false;
	SDL_Event e;
	while (!quit && epochNumber != maxEpochNumber) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) quit = true;
			ImGui_ImplSDL2_ProcessEvent(&e); // Process GUI events
		}

		world.setVisionRange(guiState.boidVisionRange);
		// Handle changing number of boids
		while (boids.size() < guiState.boidNumber) {
			boids.push_back(new Boid(posDistrib(seedy), posDistrib(seedy), speedDistrib(seedy), directionDistrib(seedy), guiState));
			renderShapes.push_back(RenderRect(boids.back()->x, boids.back()->y, 8, 8, true, 255, 255, 255));
		}
		while (boids.size() > guiState.boidNumber) {
			delete boids.back();
			boids.pop_back();
			renderShapes.pop_back();
		}
		// Handle changing world size
		if (guiState.rebuildWorld) {
			guiState.rebuildWorld = false;
			renderer.setZoom(640.0 / (guiState.newCellNum * guiState.newCellSize));
			world.rebuildWorld(guiState.newCellSize, guiState.newCellNum, guiState.newCellNum, guiState.newEdgeMargin);
			boidProcessor.processBounds(boids);

			posDistrib = std::uniform_real_distribution<float>(0, guiState.newCellNum * guiState.newCellSize);
			speedDistrib = std::uniform_real_distribution<float>(guiState.boidMinSpeed, guiState.boidMaxSpeed);
		}

		// PARALLEL
		if (doParallel == true) {
			timerSystem.resetTimer("Parallel Timer");

			startNode.try_put(tbb::flow::continue_msg());
			graph.wait_for_all();

			timerSystem.stopTimer("Parallel Timer");

			std::cout << "Parallel time: " << timerSystem.readTimer("Parallel Timer") / 1000.0 << " ms\n";
			epochNumber++;

			guiSystem.process();
			renderer.renderPresent();
			continue;
		}

		// SERIAL

		// Clear cells
		world.clearCells();

		timerSystem.resetTimer("Populate Serial");
		// Populate cells
		for (Boid* boid : boids) {
			Cell* currCell = world.getCell(boid->x / world.getCellSize(), boid->y / world.getCellSize());
			currCell->boids.push_back(boid);
		}
		timerSystem.stopTimer("Populate Serial");

		timerSystem.resetTimer("Rules Serial");
		// Apply rules
		boidProcessor.process(boids);
		timerSystem.stopTimer("Rules Serial");

		//world.printWorld();
		timerSystem.resetTimer("Render Serial");
		for (unsigned int i = 0; i < boids.size(); i++) {
			renderShapes[i].x = boids[i]->x;
			renderShapes[i].y = boids[i]->y;
		}
		renderer.renderClear();
		renderer.drawRects(renderShapes);
		guiSystem.process(); // Draw and render GUI
		timerSystem.stopTimer("Render Serial");

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));

		//std::cout << "Pos(x, y): " << boids[0]->x << " " << boids[0]->y << " Vector(m, d): " << boids[0]->vec.magnitude << " " << boids[0]->vec.direction << " Nearby: " << boids[0]->nearbyBoids.size() << "\n";
		//std::cin.ignore();
		std::cout << "Serial time:\n";
		std::cout << "Populate: " << timerSystem.readTimer("Populate Serial") / 1000.0 << " ms | ";
		std::cout << "Rules: " << timerSystem.readTimer("Rules Serial") / 1000.0 << " ms | ";
		std::cout << "Render: " << timerSystem.readTimer("Render Serial") / 1000.0 << " ms\n";

		renderer.renderPresent();
		epochNumber++;
	}

	std::cout << "Epochs: " << epochNumber << " Total Time: " << timerSystem.readTimer("Total Timer") / 1000.0 << " ms\n";

	return 0;
}
