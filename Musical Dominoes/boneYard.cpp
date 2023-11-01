#include <iostream>
#include "boneYard.hpp"
#include "status.hpp"
#include "sprite.hpp"
#include "constants.hpp"
#include "renderLayer.hpp"

void BoneYard::addDomino(entt::entity domino, entt::registry& registry) {
	dominoes.push_back(domino);

	auto& dominoComp = registry.get<Domino>(domino);
	dominoComp.inBoneYard = true;
	dominoComp.startingX = carouselPosition.x + carouselXOffset;
	dominoComp.startingY = carouselPosition.y;
	carouselXOffset += Constants::DOMINO_WIDTH;
	// Calculate multirow boneyard
	if (carouselXOffset % (Constants::DOMINO_WIDTH * carouselLength) == 0) {
		carouselPosition.y += Constants::DOMINO_HEIGHT;
		carouselXOffset = 0;
	}
	auto& position = registry.get<Position>(domino);
	position.x = dominoComp.startingX;
	position.y = dominoComp.startingY;

	registry.get<RenderLayer>(domino).layer = RENDER_LAYER::GRAB_LAYER;

	auto& sprite = registry.get<Sprite>(domino);
	sprite.currSprite = 1; // Set sprite to back side
	auto& status = registry.get<Status>(domino);
	status.active = false;
}

void BoneYard::changeStatus(bool setActive, entt::registry& registry) {
	// Iterate arrays from the back to allow deletion without messing up the iterator
	for (int i = dominoes.size() - 1; i >= 0; i--) {
		auto& domino = registry.get<Domino>(dominoes[i]);
		if (domino.inBoneYard == false) {
			dominoes.erase(dominoes.begin() + i);
			continue;
		}

		auto& active = registry.get<Status>(dominoes[i]);
		if (setActive) active.active = true;
		else active.active = false;
	}

	is_active = setActive;
}

void BoneYard::setCarouselPosition(float xPos, float yPos, entt::registry& registry, int length) {
	carouselPosition = {xPos, yPos};
	carouselXOffset = 0;
	carouselLength = length;

	for (auto dominoID : dominoes) {
		auto& dominoComp = registry.get<Domino>(dominoID);
		dominoComp.startingX = carouselPosition.x + carouselXOffset;
		dominoComp.startingY = carouselPosition.y;
		carouselXOffset += Constants::DOMINO_WIDTH;
		// Calculate multirow boneyard
		if (carouselXOffset % (Constants::DOMINO_WIDTH * carouselLength) == 0) {
			carouselPosition.y += Constants::DOMINO_HEIGHT;
			carouselXOffset = 0;
		}
		auto& position = registry.get<Position>(dominoID);
		position.x = dominoComp.startingX;
		position.y = dominoComp.startingY;
	}
}

void BoneYard::removeGrabbedDominoes(entt::registry& registry) {
	for (int i = dominoes.size() - 1; i >= 0; i--) {
		if (registry.get<Domino>(dominoes[i]).inBoneYard == false) {
			dominoes.erase(dominoes.begin() + i);
		}
	}
}
