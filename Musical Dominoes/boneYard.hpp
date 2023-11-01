// Stores and distributes game dominoes
#ifndef BONE_YARD_HPP
#define BONE_YARD_HPP

#include <vector>
#include "entt/entity/registry.hpp"
#include "domino.hpp"
#include "position.hpp"

class BoneYard
{
	public:
		BoneYard(int xPos, int yPos) : carouselPosition(xPos, yPos) {}
		void addDomino(entt::entity domino, entt::registry& registry); // Adds domino to list and also makes it invisible and set inBoneYard
		void setActive(entt::registry& registry) { changeStatus(true, registry); }
		void setInactive(entt::registry& registry) { changeStatus(false, registry); }
		bool isActive() { return is_active; }
		void removeGrabbedDominoes(entt::registry& registry); // Remove all grabbed dominoes from boneyard

		void setCarouselPosition(float xPos, float yPos, entt::registry& registry, int length = 11);

		std::vector<entt::entity>& getDominoes() { return dominoes; }
	private:
		void changeStatus(bool setActive, entt::registry& registry);

		std::vector<entt::entity> dominoes;
		Position carouselPosition; // Starting position of the carousel
		int carouselXOffset = 0;
		int carouselLength = 11;
		bool is_active = false;
};

#endif
