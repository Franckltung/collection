// Franck Limtung
// 2023-Jul
#ifndef TEXTURE_LOADING_SYSTEM_HPP
#define TEXTURE_LOADING_SYSTEM_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <SDL2/SDL_ttf.h>
#include "entt/entity/registry.hpp"
#include "texture.hpp"
#include "renderLayer.hpp"
#include "box.hpp"

class TextureLoadingSystem
{
	public:
		TextureLoadingSystem(SDL_Renderer* renderer, std::string fontFilename) : renderer(renderer) {
			GLFont = TTF_OpenFont(fontFilename.c_str(), 32);
			assert(GLFont != NULL);
		}
		~TextureLoadingSystem() {
			TTF_CloseFont(GLFont);
		}

		std::shared_ptr<Texture> getTexture(std::string filename, bool floating, Box size = {0, 0}); // Get or load a single texture based on file path
		std::shared_ptr<Texture> getTextTexture(std::string text, SDL_Color color = {255, 255, 255, 255}, bool floating = true); // Get texture for a text
		SDL_Renderer* getRenderer() { return renderer; }
		TTF_Font* getFont() { return GLFont; }
	private:
		std::unordered_map<std::string, std::shared_ptr<Texture>> textureMap; // textures should be automatically destroyed by RAII
		SDL_Renderer* renderer;
		TTF_Font* GLFont;
};

#endif
