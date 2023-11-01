// Franck Limtung
// 2023-Jul
// Almost identical to the texture loading system
#ifndef SOUND_LOADING_SYSTEM_HPP
#define SOUND_LOADING_SYSTEM_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include "entt/entity/registry.hpp"
#include "sound.hpp"
#include "constants.hpp"
#include "renderSystem.hpp"

class SoundLoadingSystem
{
	public:
		SoundLoadingSystem(const RenderSystem& renderSystem) {
			assert(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0);
			assert(renderSystem.isInitialized());
			Mix_Volume(-1, MIX_MAX_VOLUME * Constants::VOLUME);
			Mix_AllocateChannels(200);
		}
		~SoundLoadingSystem() {
			Mix_Quit();
		}
		std::shared_ptr<Sound> getSound(std::string filename);
	private:
		std::unordered_map<std::string, std::shared_ptr<Sound>> soundMap;
};

#endif
