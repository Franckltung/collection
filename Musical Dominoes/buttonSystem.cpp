#include <iostream>
#include "buttonSystem.hpp"
#include "button.hpp"
#include "status.hpp"
#include "clickable.hpp"
#include "sprite.hpp"

void ButtonSystem::process(entt::registry& registry) {
	auto slice = registry.view<Button, Status, Sprite, Clickable>();
	for (auto iter : slice) {
		if (!slice.get<Status>(iter).active) continue;
		if (slice.get<Clickable>(iter).clicked == true) {
			slice.get<Button>(iter).event();
			registry.get<Status>(clickEffect).active = true; // Play click sound
			slice.get<Sprite>(iter).currSprite = 1;
		}
		else {
			slice.get<Sprite>(iter).currSprite = 0;
		}
	}
}
