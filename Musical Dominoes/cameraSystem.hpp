// 2022-Oct
#ifndef CAMERA_SYSTEM_HPP
#define CAMERA_SYSTEM_HPP

#include "entt/entity/registry.hpp"
#include "position.hpp"

class CameraSystem
{
	public:
		CameraSystem(int cameraWidth, int cameraHeight, entt::registry& registry) : cameraWidth(cameraWidth), cameraHeight(cameraHeight) {
			assert(cameraWidth > 0 && cameraHeight > 0);
			cameraID = registry.create();
			registry.emplace<Position>(cameraID);
		}
		void process(entt::registry& registry);

		void attach(entt::entity entity) { playerID = entity; } // Attach camera to entity
		entt::entity getCameraID() { return cameraID; }

		void setCameraSize(int cameraWidth, int cameraHeight);
	private:
		entt::entity playerID = entt::null;
		entt::entity cameraID;
		int cameraWidth, cameraHeight;
};

#endif
