// Franck Limtung
// 2023-Jul
// This struct stores the SDL sound and frees it when it's deleted
// The sound entity should have a status to control its playback
#ifndef SOUND_HPP
#define SOUND_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

struct Sound
{
	~Sound() { Mix_FreeChunk(soundChunk); }

	Mix_Chunk* soundChunk = nullptr;
	std::string filename;
	bool initialized = false;;
};

#endif
