// Franck Limtung
#ifndef IMGUI_SYSTEM_HPP
#define IMGUI_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <string>

struct GUIState
{
	int boidNumber = 128;
	float boidMaxSpeed = 10.0;
	float boidMinSpeed = 3.0;
	float boidVisionRange = 64.0;
	float boidProtectionRange = 8.0;
	int newCellSize = 32;
	int newCellNum = 48;
	int newEdgeMargin = 2;
	float impulseForwardStr = 3.0;
	float impulseNearBoundsStr = 1.0;
	float impulseSeparationStr = 0.3;
	float impulseAlignStr = 0.1;
	float impulseCohesionStr = 0.05;
	float drag = 0.96;
	bool rebuildWorld = false;
};

class ImGuiSystem
{
	public:
		ImGuiSystem();
		~ImGuiSystem() { free(); }
		void free();

		void process();

		GUIState& getState() { return state; }
		SDL_Window* getWindow() { return window; }
	private:
		SDL_Renderer* renderer; // GUI gets a separate renderer so it works properly with scaling
		SDL_Window* window;
		GUIState state;
		int windowWidth = 0;
		int windowHeight = 0;
		bool rebuildMenu = false;
};

#endif
