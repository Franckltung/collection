#include "player.hpp"
#include "status.hpp"
#include "domino.hpp"

void Player::changeStatus(bool setActive, entt::registry& registry) {
	// Iterate arrays from the back to allow deletion without messing up the iterator
	for (int i = dominoes.size() - 1; i >= 0; i--) {
		auto& domino = registry.get<Domino>(dominoes[i]);
		if (domino.placed == true) {
			dominoes.erase(dominoes.begin() + i);
			continue;
		}

		auto& active = registry.get<Status>(dominoes[i]);
		if (setActive) active.active = true;
		else active.active = false;
	}

	orderDominoes(registry);
}

void Player::orderDominoes(entt::registry& registry) {
	for (unsigned int i = 0; i < dominoes.size(); i++) {
		auto& domino = registry.get<Domino>(dominoes[i]);
		if (domino.placed == true) continue; // Skip already placed dominoes
		domino.startingX = carouselPosition.x + (i * Constants::DOMINO_WIDTH) % (Constants::DOMINO_WIDTH * 4);
		domino.startingY = carouselPosition.y + (i / 4) * Constants::DOMINO_HEIGHT;
		auto& position = registry.get<Position>(dominoes[i]);
		position.x = domino.startingX;
		position.y = domino.startingY;
	}
}

void Player::removePlacedDominoes(entt::registry& registry) {
	for (int i = dominoes.size() - 1; i >= 0; i--) {
		if (registry.get<Domino>(dominoes[i]).placed == true) {
			dominoes.erase(dominoes.begin() + i);
		}
	}
}
