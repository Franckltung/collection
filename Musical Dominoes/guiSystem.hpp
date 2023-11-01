#ifndef GUI_SYSTEM_HPP
#define GUI_SYSTEM_HPP

#include "entt/entity/registry.hpp"
#include "guiElement.hpp"

class GuiSystem
{
	public:
		void loadScene(GUI_SCENE scene, entt::registry& registry);
		void unloadGUI(entt::registry& registry);
};

#endif
