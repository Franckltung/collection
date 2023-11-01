#include <cassert>
#include <iostream>
#include <SDL2/SDL_image.h>
#include "renderSystem.hpp"
#include "position.hpp"
#include "utility.hpp"
#include "texture.hpp"
#include "sprite.hpp"
#include "angle.hpp"
#include "circle.hpp"
#include "box.hpp"
#include "clickable.hpp"
#include "inputSystem.hpp"
#include "constants.hpp"
#include "renderLayer.hpp"

RenderSystem::RenderSystem(std::string windowTitle, int windowWidth, int windowHeight, entt::entity activeInputID,
		entt::entity cameraPosition) {
	assert(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) >= 0); // Initialize
	assert(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2")); // Set anisotropic filtering
	window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
	assert(window != NULL);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	assert(renderer != NULL);
	//SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Set draw color to black

	// Set up PNG loading
	assert(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG);

	// Set up text
	assert(TTF_Init() != -1);

	assert(SDL_RenderSetIntegerScale(renderer, SDL_TRUE) >= 0);

	//SDL_ShowCursor(false);

	this->activeInputID = activeInputID;
	this->cameraPosition = cameraPosition;

	for (int i = 0; i < static_cast<int>(RENDER_LAYER::count); i++) {
		layers.push_back(std::vector<Renderable>());
	}

	initialized = true;
}

void RenderSystem::process(entt::registry& registry) {
	assert(cameraPosition != entt::null);
	auto& cameraPos = registry.get<Position>(cameraPosition);

	// Render background if it exists
	if (backgroundTexture != entt::null) {
		assert(registry.all_of<std::shared_ptr<Texture>>(backgroundTexture));
		auto& BGTexture = registry.get<std::shared_ptr<Texture>>(backgroundTexture);

		SDL_Rect BGQuad = {static_cast<int>(0 - Constants::WINDOW_WIDTH / 2.0 - cameraPos.x), static_cast<int>(0 - Constants::WINDOW_HEIGHT / 2.0 - cameraPos.y), BGTexture->width, BGTexture->height};
		SDL_RenderCopy(renderer, BGTexture->texture, NULL, &BGQuad);
	}

	// Empty layer arrays
	for (auto& iter : layers) iter.clear();

	// Prepare layers
	auto slice = registry.view<Position, std::shared_ptr<Texture>>();
	for (auto iter : slice) {
		if (Utility::getStatus(iter, registry) == false) continue;

		auto& texture = registry.get<std::shared_ptr<Texture>>(iter);
		assert(texture->initialized == true);
		assert(texture->texture != NULL);
		assert(texture->width > 0 && texture->height > 0);
		if (registry.all_of<RenderLayer>(iter)) assert(registry.get<RenderLayer>(iter).layer < RENDER_LAYER::count);
		auto& position = slice.get<Position>(iter);
		SpriteClip clip;
		if (registry.all_of<Sprite>(iter)) {
			auto& sprite = registry.get<Sprite>(iter);
			clip = sprite.sprites[sprite.currSprite];
		}
		else clip = {0, 0, texture->width, texture->height, 0};
		// Get layer if it exists, otherwise default to highest layer
		int renderLayer;
		if (registry.all_of<RenderLayer>(iter)) renderLayer = static_cast<int>(registry.get<RenderLayer>(iter).layer);
		else renderLayer = static_cast<int>(RENDER_LAYER::count) - 1;
		layers[renderLayer].push_back(Renderable());
		Renderable& renderable = layers[renderLayer].back();
		renderable.texture = texture->texture;
		if (texture->floating) {
			renderable.renderQuad = {(int)position.x, (int)position.y, texture->width, texture->height};
		}
		else {
			renderable.renderQuad = {(int)(position.x - cameraPos.x), (int)(position.y - cameraPos.y), texture->width, texture->height};
		}
		renderable.spriteQuad = {clip.x, clip.y, clip.width, clip.height};
		if (registry.all_of<Angle>(iter)) renderable.angle = registry.get<Angle>(iter).angle;

		// Calculate center of rotation
		renderable.center.x = renderable.renderQuad.w / 2.0;
		renderable.center.y = renderable.renderQuad.h / 2.0;
	}

	// Render layers
	for (unsigned int i = 0; i < layers.size(); i++) {
		for (unsigned int j = 0; j < layers[i].size(); j++) {
			auto& renderable = layers[i][j];
			SDL_RenderCopyEx(renderer, renderable.texture, &renderable.spriteQuad, &renderable.renderQuad, renderable.angle, &renderable.center, SDL_FLIP_NONE);
		}
	}

	/* Render mouse
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF); // White

	auto& mousePosition = registry.get<ActiveInput>(activeInputID).mousePosition;
	SDL_Rect mouseRect{(int)mousePosition.x, (int)mousePosition.y, 10, 10};
	SDL_RenderFillRect(renderer, &mouseRect);

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black the default
	// */

	if (renderClickables) {
		// Render Clickables
		auto sliceClickables = registry.view<Clickable, Position>();
		for (auto iter : sliceClickables) {
			assert(registry.all_of<Box>(iter) || registry.all_of<Circle>(iter));
			Clickable& clickable = registry.get<Clickable>(iter);
			if (Utility::getStatus(iter, registry) == false) {
				SDL_SetRenderDrawColor(renderer, 0x80, 0x80, 0x80, 0x80); // Gray transparent
			}
			else if (!clickable.clicked) SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF); // Red
			else SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF); // Green

			if (registry.all_of<Circle>(iter)) {
				auto& circle = registry.get<Circle>(iter);
				auto position = Utility::getCentered(registry.get<Position>(iter), circle);
				// Simple circle algorithm
				for (int w = 0; w < circle.radius * 2; w++) {
					for (int h = 0; h < circle.radius * 2; h++) {
						int dw = circle.radius - w;
						int dh = circle.radius - h;
						if (dw * dw + dh * dh <= circle.radius * circle.radius) {
							if (registry.all_of<std::shared_ptr<Texture>>(iter) && registry.get<std::shared_ptr<Texture>>(iter)->floating == true) {
								SDL_RenderDrawPoint(renderer, position.x + dw, position.y + dh);
							}
							else SDL_RenderDrawPoint(renderer, position.x - cameraPos.x + dw, position.y - cameraPos.y + dh);
						}
					}
				}
			}
			else if (registry.all_of<Box>(iter)) {
				auto& box = registry.get<Box>(iter);
				auto position = registry.get<Position>(iter);
				SDL_Rect clickableRect;
				if (registry.all_of<std::shared_ptr<Texture>>(iter) && registry.get<std::shared_ptr<Texture>>(iter)->floating == true) {
					clickableRect = {(int)(position.x), (int)(position.y), (int)box.width, (int)box.height};
				}
				else clickableRect = {(int)(position.x - cameraPos.x), (int)(position.y - cameraPos.y), (int)box.width, (int)box.height};
				SDL_RenderFillRect(renderer, &clickableRect);
			}
		}

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black the default
	}
}

void RenderSystem::free() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = NULL;
	window = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
