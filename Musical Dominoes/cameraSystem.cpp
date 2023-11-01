#include <cassert>
#include "cameraSystem.hpp"
#include "utility.hpp"

void CameraSystem::process(entt::registry& registry) {
	assert(registry.all_of<Position>(playerID));
	auto& cameraPos = registry.get<Position>(cameraID);
	Position playerPos;
	if (playerID != entt::null) {
		assert(registry.all_of<Position>(playerID));
		playerPos = Utility::getCentered(playerID, registry);
	}
	cameraPos.x = playerPos.x - (cameraWidth / 2);
	cameraPos.y = playerPos.y - (cameraHeight / 2);
}

void CameraSystem::setCameraSize(int cameraWidth, int cameraHeight) {
	this->cameraWidth = cameraWidth;
	this->cameraHeight = cameraHeight;
}
