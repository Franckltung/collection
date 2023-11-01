#include "texture.hpp"

void Texture::free() {
	SDL_DestroyTexture(texture);
	texture = NULL;
}
