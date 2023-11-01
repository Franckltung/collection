// 2022-Oct
#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "entt/entity/registry.hpp"

struct Renderable
{
	SDL_Texture* texture;
	SDL_Rect spriteQuad;
	SDL_Rect renderQuad;
	double angle = 0.0;
	SDL_Point center; // Center to use for rotation. Will be based on the box / circle of entity instead of the texture

};

class RenderSystem
{
	public:
		RenderSystem(std::string windowTitle, int windowWidth, int windowHeight, entt::entity activeInputID, entt::entity cameraPosition);
		~RenderSystem() { free(); }
		void free();

		void process(entt::registry& registry);

		void setBackground(entt::entity background) { backgroundTexture = background; }
		SDL_Renderer* getRenderer() { return renderer; }
		SDL_Window* getWindow() { return window; }

		void debugSetRenderClickables(bool enable) { renderClickables = enable; }

		bool isInitialized() const { return initialized; } // TODO: NOT REALLY WORKING RIGHT NOW BUT IT MIGHT NOT BE IMPORTANT
	private:
		entt::entity cameraPosition = entt::null;
		entt::entity activeInputID = entt::null;
		entt::entity backgroundTexture = entt::null;
		std::vector<std::vector<Renderable>> layers;// Z layers of textures

		SDL_Renderer* renderer;
		SDL_Window* window;

		bool renderClickables = false;
		bool initialized = false; // Did SDL2 initialize correctly?
};

#endif
