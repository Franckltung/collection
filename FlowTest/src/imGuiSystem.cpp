#include <cassert>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "imGuiSystem.hpp"

ImGuiSystem::ImGuiSystem() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// Setup SDL
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
	window = SDL_CreateWindow("Control Panel", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	assert(window != NULL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	assert(renderer != NULL);

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Set to BG color

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	SDL_GetWindowSize(window, &windowWidth, &windowHeight);
}

void ImGuiSystem::process() {
	SDL_RenderClear(renderer);

	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);

	if (ImGui::Begin("ControlPanel", 0, window_flags)) {
		ImGui::SliderInt("Boids", &state.boidNumber, 0, 16384);
		ImGui::SliderFloat("Boid Max Speed", &state.boidMaxSpeed, 0.0, 50.0);
		ImGui::SliderFloat("Boid Min Speed", &state.boidMinSpeed, 0.0, 50.0);
		ImGui::SliderFloat("Boid Vision Range", &state.boidVisionRange, 0.0, 128.0);
		ImGui::SliderFloat("Boid Protection Range", &state.boidProtectionRange, 0.0, 128.0);
		ImGui::SliderFloat("Forward Impulse Strength", &state.impulseForwardStr, 0.0, 10.0);
		ImGui::SliderFloat("Near Bounds Impulse Strength", &state.impulseNearBoundsStr, 0.0, 10.0);
		ImGui::SliderFloat("Separation Impulse Strength", &state.impulseSeparationStr, 0.0, 10.0);
		ImGui::SliderFloat("Alignment Impulse Strength", &state.impulseAlignStr, 0.0, 1.0);
		ImGui::SliderFloat("Cohesion Impulse Strength", &state.impulseCohesionStr, 0.0, 1.0);
		ImGui::SliderFloat("Drag", &state.drag, 0.0, 1.0);

		if (ImGui::Button("Rebuild World")) rebuildMenu = true;

		if (rebuildMenu) ImGui::OpenPopup("Rebuild Popup");
		if (ImGui::BeginPopup("Rebuild Popup")) {
			ImGui::SliderInt("Cell Number", &state.newCellNum, 8, 256);
			ImGui::SliderInt("Cell Size", &state.newCellSize, 8, 256);
			ImGui::SliderInt("Edge Margin", &state.newEdgeMargin, 0, state.newCellNum / 2);
			state.rebuildWorld |= ImGui::Button("Ok");
			if (state.rebuildWorld || ImGui::Button("Cancel")) {
				rebuildMenu = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

	SDL_RenderPresent(renderer);
}

void ImGuiSystem::free() {
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = NULL;
	window = NULL;
}
