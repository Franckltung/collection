#include <SDL2/SDL_image.h>
#include "textureLoadingSystem.hpp"

std::shared_ptr<Texture> TextureLoadingSystem::getTexture(std::string filename, bool floating, Box size) {
	auto textureLookup = textureMap.find(filename);
	if (textureLookup != textureMap.end()) {
		return textureLookup->second;
	}
	else {
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		SDL_Texture* newTexture = NULL;
		SDL_Surface* loadedSurface = NULL;

		loadedSurface = IMG_Load(filename.c_str());
		assert(loadedSurface != NULL);
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		assert(newTexture != NULL);

		texture->texture = newTexture;
		// Unless specified, use the image's size to determine the textures size
		if (size.width <= 0 || size.height <= 0) {
			texture->width = loadedSurface->w;
			texture->height = loadedSurface->h;
		}
		else {
			texture->width = size.width;
			texture->height = size.height;
		}
		texture->filename = filename;
		texture->floating = floating;
		texture->initialized = true;

		SDL_FreeSurface(loadedSurface);

		textureMap.insert({filename, texture});
		return texture;
	}
}

std::shared_ptr<Texture> TextureLoadingSystem::getTextTexture(std::string text, SDL_Color color, bool floating) {
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = NULL;

	loadedSurface = TTF_RenderUTF8_Blended(GLFont, text.c_str(), color);
	assert(loadedSurface != NULL);
	newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	assert(newTexture != NULL);

	texture->texture = newTexture;
	texture->width = loadedSurface->w;
	texture->height = loadedSurface->h;
	texture->filename = "TEXT";
	texture->floating = floating;
	texture->initialized = true;

	SDL_FreeSurface(loadedSurface);

	return texture;
}
