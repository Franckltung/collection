// Controls a set of dominoes
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>
#include "entt/entity/registry.hpp"
#include "position.hpp"
#include "constants.hpp"

class Player
{
	public:
		void setActive(entt::registry& registry) { changeStatus(true, registry); }
		void setInactive(entt::registry& registry) { changeStatus(false, registry); }
		void addDomino(entt::entity domino) { dominoes.push_back(domino); };
		void orderDominoes(entt::registry& registry); // Put all owned dominoes in carousel
		void removePlacedDominoes(entt::registry& registry); // Remove all placed dominoes from hand
		std::vector<entt::entity>& getDominoes() { return dominoes; }
	private:
		void changeStatus(bool setActive, entt::registry& registry);

		std::vector<entt::entity> dominoes;
		Position carouselPosition = {50, 25}; // Starting position of the carousel
};

#endif
