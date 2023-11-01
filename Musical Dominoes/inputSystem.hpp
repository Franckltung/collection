// 2022-Oct
// Only run after initializing SDL!
#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "entt/entity/registry.hpp"
#include "position.hpp"
#include "cameraSystem.hpp"
#include "status.hpp"

struct ActiveInput
{
	Position mousePosition;
	Status escKey;
	Status pKey;
	Status mouseLeft;
	Status mouseRight;
	Status holdingMouseLeft; // Are we holding down the left mouse button
	Status clickedButton; // Did we click a button? This disables player movement
	Status mouseScrolling;
	float scrollValue = 0;
};

class InputSystem
{
	public:
		InputSystem(entt::registry& registry);
		bool process(entt::registry& registry, CameraSystem& cameraSystem, float zoomLevel);

		entt::entity activeInputID;
};

#endif
