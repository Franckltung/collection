#include "soundLoadingSystem.hpp"

std::shared_ptr<Sound> SoundLoadingSystem::getSound(std::string filename) {
	auto soundLookup = soundMap.find(filename);
	if (soundLookup != soundMap.end()) {
		return soundLookup->second;
	}
	else {
		std::shared_ptr<Sound> sound = std::make_shared<Sound>();
		sound->soundChunk = Mix_LoadWAV(filename.c_str()); // Can load ogg
		assert(sound->soundChunk != nullptr);
		sound->initialized = true;

		soundMap.insert({filename, sound});
		return sound;
	}
}
