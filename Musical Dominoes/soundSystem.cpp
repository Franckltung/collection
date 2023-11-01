#include <memory>
#include "soundSystem.hpp"
#include "sound.hpp"
#include "status.hpp"

void SoundSystem::process(entt::registry& registry) {
	auto slice = registry.view<std::shared_ptr<Sound>, Status>();

	for (auto iter : slice) {
		auto sound = slice.get<std::shared_ptr<Sound>>(iter);
		auto& status = slice.get<Status>(iter);

		if (status.active == false) continue;
		assert(sound->initialized == true);

		Mix_PlayChannel(-1, sound->soundChunk, false);
		status.active = false;
	}
}
