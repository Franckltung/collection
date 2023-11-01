#include "guiSystem.hpp"
#include "status.hpp"
#include "clickable.hpp"

void GuiSystem::loadScene(GUI_SCENE scene, entt::registry& registry) {
	auto slice = registry.view<GuiElement, Status>();
	for (auto iter : slice) {
		if (slice.get<GuiElement>(iter).type == scene) {
			slice.get<Status>(iter).active = true;
		}
	}
}

void GuiSystem::unloadGUI(entt::registry& registry) {
	auto slice = registry.view<GuiElement, Status>();
	for (auto iter : slice) {
		if (slice.get<GuiElement>(iter).temporary == true) {
			// Erase temporary entities
			registry.destroy(iter);
			continue;
		}

		slice.get<Status>(iter).active = false;
		// unclick all buttons so they are not active when the gui comes back
		if (registry.all_of<Clickable>(iter)) registry.get<Clickable>(iter).clicked = false;
	}
}
