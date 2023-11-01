// Franck Limtung
// 2023-May
// This struct stores the SDL texture and frees it when it's deleted
// Textures should be shared between objects using shared ptrs, the RenderLayer and SpriteClip will differentiate entities
#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <string>

struct Texture
{
	~Texture() { SDL_DestroyTexture(texture); }
	void free();

	SDL_Texture* texture = NULL;
	std::string filename;
	int width = -1; // Set a custom width and height to scale the size of the rendered texture
	int height = -1;
	bool initialized = false;
	bool floating = false;
};

#endif
