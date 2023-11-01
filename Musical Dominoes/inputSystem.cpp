#include <SDL2/SDL.h>
#include <cassert>
#include <iostream>
#include "inputSystem.hpp"
#include "position.hpp"
#include "circle.hpp"
#include "box.hpp"
#include "utility.hpp"
#include "clickable.hpp"
#include "domino.hpp"
#include "texture.hpp"

InputSystem::InputSystem(entt::registry& registry) {
	activeInputID = registry.create();
	registry.emplace<ActiveInput>(activeInputID);
}

bool InputSystem::process(entt::registry& registry, CameraSystem& cameraSystem, float zoomLevel) {
	ActiveInput& activeInput = registry.get<ActiveInput>(activeInputID);
	// Update mouse position
	auto& mousePos = activeInput.mousePosition;
	int x, y;
	SDL_GetMouseState(&x, &y);
	mousePos.x = x / zoomLevel;
	mousePos.y = y / zoomLevel;

	// Check keys
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) return true;
		else if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					activeInput.escKey.active = true;
					break;
				case SDLK_p:
					activeInput.pKey.active = true;
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					activeInput.escKey.active = false;
					break;
				case SDLK_p:
					activeInput.pKey.active = false;
			}
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			if (e.button.button == SDL_BUTTON_LEFT) activeInput.mouseLeft.active = true;
			else if (e.button.button == SDL_BUTTON_RIGHT) activeInput.mouseRight.active = true;
		}
		else if (e.type == SDL_MOUSEBUTTONUP) {
			if (e.button.button == SDL_BUTTON_LEFT) activeInput.mouseLeft.active = false;
			else if (e.button.button == SDL_BUTTON_RIGHT) activeInput.mouseRight.active = false;
		}
		else if (e.type == SDL_MOUSEWHEEL) {
			activeInput.mouseScrolling.active = true;
			activeInput.scrollValue = e.wheel.preciseY;
		}
		else {
			activeInput.mouseScrolling.active = false;
			activeInput.scrollValue = 0;
		}
	}

	// Reset unclicked because it only lasts one tick
	auto slice = registry.view<Clickable>();
	for (auto iter : slice) {
		auto& clickable = slice.get<Clickable>(iter);
		if (clickable.unclicked == true) slice.get<Clickable>(iter).unclicked = false;
	}

	// Check clickables - only happens when you just clicked, not if you are holding down left
	if (!activeInput.holdingMouseLeft.active) {
		activeInput.clickedButton = false; // Set to false initially
		auto slice = registry.view<Position, Clickable>();
		for (auto iter : slice) {
			if (registry.all_of<Status>(iter) && registry.get<Status>(iter).active == false) continue; // Skip inactive
			auto& clickable = registry.get<Clickable>(iter);
			Position& mousePos = activeInput.mousePosition;
			Position cameraPos = registry.get<Position>(cameraSystem.getCameraID());
			// Handle floating objects because they are unaffected by the camera
			if (registry.all_of<std::shared_ptr<Texture>>(iter) && registry.get<std::shared_ptr<Texture>>(iter)->floating == true) {
				cameraPos.x = 0;
				cameraPos.y = 0;
			}
			assert(registry.all_of<Circle>(iter) || registry.all_of<Box>(iter));

			if (activeInput.mouseLeft.active) {
				if (registry.all_of<Circle>(iter)) {
					auto position = Utility::getCentered(registry.get<Position>(iter), registry.get<Circle>(iter));
					float distance = std::pow(position.x - mousePos.x - cameraPos.x, 2) + std::pow(position.y - mousePos.y - cameraPos.y, 2);
					if (distance < std::pow(registry.get<Circle>(iter).radius, 2)) {
						clickable.clicked = true;
						clickable.unclicked = false;
						activeInput.clickedButton = true;
					}
					else if (clickable.clicked == true) {
						// Deactivate
						clickable.clicked = false;
						clickable.unclicked = true;
					}
				}
				else if (registry.all_of<Box>(iter)) {
					auto& box = registry.get<Box>(iter);
					auto position = registry.get<Position>(iter);
					if (mousePos.x > position.x - cameraPos.x && mousePos.x < position.x - cameraPos.x + box.width &&
							mousePos.y > position.y - cameraPos.y && mousePos.y < position.y - cameraPos.y + box.height) {
						clickable.clicked = true;
						clickable.unclicked = false;
						activeInput.clickedButton = true;
					}
					else if (clickable.clicked == true) {
						clickable.clicked = false;
						clickable.unclicked = true;
					}
				}
			}
			else {
				if (clickable.clicked == true) {
					// Deactivate
					clickable.clicked = false;
					clickable.unclicked = true;
				}
			}
		}
	}

	// Set holding *after* performing actions so it applies on the next tick
	if (activeInput.mouseLeft.active == true) activeInput.holdingMouseLeft.active = true;
	else activeInput.holdingMouseLeft.active = false;

	// Not quitting
	return false;
}
