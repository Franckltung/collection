#ifndef DOMINO_HPP
#define DOMINO_HPP

#include "entt/entity/registry.hpp"

enum class DOMINO_DIRECTION
{
	NORTH,
	EAST,
	SOUTH,
	WEST
};

struct Domino
{
	Domino(std::string headValue, std::string tailValue) : headValue(headValue), tailValue(tailValue) {}
	std::string headValue = ""; // Pointing towards direction
	std::string tailValue = "";
	DOMINO_DIRECTION direction = DOMINO_DIRECTION::WEST;
	// Starting positions to return to after a failed dragging
	float startingX = 0;
	float startingY = 0;
	bool placed = false; // Set active after the domino is placed so it can be removed from the player's hand
	bool inBoneYard = false; // Dominoes in the bone yard have different behavior
	entt::entity pickUpEffect;
	entt::entity placeEffect;
};

#endif
