#include <iostream>
#include "dominoBoard.hpp"
#include "domino.hpp"
#include "constants.hpp"

DominoBoard::DominoBoard(entt::entity startingDomino, entt::registry& registry) {
	resetBoard(startingDomino, registry);
}

void DominoBoard::resetBoard(entt::entity startingDomino, entt::registry& registry) {
	dominoChain.clear();

	// Generate domino positions based on the starting domino's position
	dominoChain.push_back(startingDomino);
	positions.resize(64);
	Domino& startDomino = registry.get<Domino>(startingDomino);
	positions[31] = {startDomino.startingX, startDomino.startingY, DOMINO_DIRECTION::EAST};

	int i = 30;
	std::tuple<float, float, DOMINO_DIRECTION> prevPosition = positions[i + 1];
	while (i > 25) {
		prevPosition = positions[i + 1];
		positions[i] = {std::get<0>(prevPosition) - Constants::DOMINO_WIDTH, std::get<1>(prevPosition), DOMINO_DIRECTION::WEST};
		i--;
	}
	// Corner
	prevPosition = positions[i + 1];
	positions[i] = {std::get<0>(prevPosition) - Constants::DOMINO_WIDTH / 4.0 - 2, std::get<1>(prevPosition) + 3.0 * Constants::DOMINO_WIDTH / 4.0 - 2, DOMINO_DIRECTION::SOUTH};
	i--;
	prevPosition = positions[i + 1];
	positions[i] = {std::get<0>(prevPosition), std::get<1>(prevPosition) + Constants::DOMINO_WIDTH, DOMINO_DIRECTION::SOUTH};
	i--;
	prevPosition = positions[i + 1];
	positions[i] = {std::get<0>(prevPosition) + Constants::DOMINO_WIDTH / 4.0 + 2, std::get<1>(prevPosition) + 3.0 * Constants::DOMINO_WIDTH / 4.0 - 3, DOMINO_DIRECTION::EAST};
	i--; // 22
	while (i > 11) {
		prevPosition = positions[i + 1];
		positions[i] = {std::get<0>(prevPosition) + Constants::DOMINO_WIDTH, std::get<1>(prevPosition), DOMINO_DIRECTION::EAST};
		i--;
	}
	prevPosition = positions[i + 1];
	positions[i] = {std::get<0>(prevPosition) + Constants::DOMINO_WIDTH / 4.0 + 2, std::get<1>(prevPosition) - 3.0 * Constants::DOMINO_WIDTH / 4.0 + 3, DOMINO_DIRECTION::NORTH};
	i--;
	while (i >= 0) {
		prevPosition = positions[i + 1];
		positions[i] = {std::get<0>(prevPosition), std::get<1>(prevPosition) - Constants::DOMINO_WIDTH, DOMINO_DIRECTION::NORTH};
		i--;
	}

	// Tail
	i = 32;
	while (i < 37) {
		prevPosition = positions[i - 1];
		positions[i] = {std::get<0>(prevPosition) + Constants::DOMINO_WIDTH, std::get<1>(prevPosition), DOMINO_DIRECTION::EAST};
		i++;
	}
	// Corner
	prevPosition = positions[i - 1];
	positions[i] = {std::get<0>(prevPosition) + Constants::DOMINO_WIDTH / 4.0 + 2, std::get<1>(prevPosition) - 3.0 * Constants::DOMINO_WIDTH / 4.0 + 3, DOMINO_DIRECTION::NORTH};
	i++;
	prevPosition = positions[i - 1];
	positions[i] = {std::get<0>(prevPosition), std::get<1>(prevPosition) - Constants::DOMINO_WIDTH, DOMINO_DIRECTION::NORTH};
	i++;
	prevPosition = positions[i - 1];
	positions[i] = {std::get<0>(prevPosition) - Constants::DOMINO_WIDTH / 4.0 - 2, std::get<1>(prevPosition) - 3.0 * Constants::DOMINO_WIDTH / 4.0 + 2, DOMINO_DIRECTION::WEST};
	i++;
	while (i < (int)positions.size() - 13) {
		prevPosition = positions[i - 1];
		positions[i] = {std::get<0>(prevPosition) - Constants::DOMINO_WIDTH, std::get<1>(prevPosition), DOMINO_DIRECTION::WEST};
		i++;
	}
	prevPosition = positions[i - 1];
	positions[i] = {std::get<0>(prevPosition) - Constants::DOMINO_WIDTH / 4.0 - 2, std::get<1>(prevPosition) + 3.0 * Constants::DOMINO_WIDTH / 4.0 - 2, DOMINO_DIRECTION::SOUTH};
	i++;
	while (i <= (int)positions.size() - 1) {
		prevPosition = positions[i - 1];
		positions[i] = {std::get<0>(prevPosition), std::get<1>(prevPosition) + Constants::DOMINO_WIDTH, DOMINO_DIRECTION::SOUTH};
		i++;
	}

	positionsHeadIter = 30;
	positionsTailIter = 32;
	headDirection = DOMINO_DIRECTION::WEST;
	tailDirection = DOMINO_DIRECTION::EAST;
}

bool DominoBoard::addDominoToHead(entt::entity domino, entt::registry& registry, std::string& matchedValue) {
	Domino& currDomino = registry.get<Domino>(dominoChain.front());
	Domino& newDomino = registry.get<Domino>(domino);

	std::string headValue = getEndValue(currDomino);
	DOMINO_DIRECTION lineDirection = headDirection; // Direction required to match with the head value

	if (newDomino.headValue == headValue && newDomino.direction == static_cast<DOMINO_DIRECTION>((static_cast<int>(lineDirection) + 2) % 4)) {
		dominoChain.push_front(domino);
		matchedValue = newDomino.headValue;
		return true;
	}
	else if (newDomino.tailValue == headValue && newDomino.direction == lineDirection) {
		dominoChain.push_front(domino);
		matchedValue = newDomino.tailValue;
		return true;
	}

	matchedValue = "";
	return false;
}

bool DominoBoard::addDominoToTail(entt::entity domino, entt::registry& registry, std::string& matchedValue) {
	Domino& currDomino = registry.get<Domino>(dominoChain.back());
	Domino& newDomino = registry.get<Domino>(domino);

	std::string headValue = getEndValue(currDomino, true);
	DOMINO_DIRECTION lineDirection = tailDirection; // Direction required to match with the head value

	if (newDomino.headValue == headValue && newDomino.direction == static_cast<DOMINO_DIRECTION>((static_cast<int>(lineDirection) + 2) % 4)) {
		dominoChain.push_back(domino);
		matchedValue = newDomino.headValue;
		return true;
	}
	else if (newDomino.tailValue == headValue && newDomino.direction == lineDirection) {
		dominoChain.push_back(domino);
		matchedValue = newDomino.tailValue;
		return true;
	}

	matchedValue = "";
	return false;
}

void DominoBoard::getNextHeadPosition(float& posX, float& posY, float& changeAngle) {
	assert(positionsHeadIter >= 0);
	std::tuple<float, float, DOMINO_DIRECTION> position = positions[positionsHeadIter];
	posX = std::get<0>(position);
	posY = std::get<1>(position);
	if (std::get<2>(position) != headDirection) {
		// Direction was changed
		changeAngle = -90;
		headDirection = std::get<2>(position);
	}

	positionsHeadIter--;
	assert(positionsHeadIter >= 0);
}

void DominoBoard::getNextTailPosition(float& posX, float& posY, float& changeAngle) {
	assert(positionsTailIter < (int)positions.size());
	std::tuple<float, float, DOMINO_DIRECTION> position = positions[positionsTailIter];
	posX = std::get<0>(position);
	posY = std::get<1>(position);
	if (std::get<2>(position) != tailDirection) {
		changeAngle = -90;
		tailDirection = std::get<2>(position);
	}

	positionsTailIter++;
	assert(positionsTailIter < (int)positions.size());
}

std::string DominoBoard::getEndValue(Domino& currDomino, bool tail) {
	DOMINO_DIRECTION lineDirection = tail ? tailDirection : headDirection; // Direction required to match with the head value
	switch (currDomino.direction) {
		case DOMINO_DIRECTION::NORTH:
			if (lineDirection == DOMINO_DIRECTION::NORTH) return currDomino.headValue;
			else if (lineDirection == DOMINO_DIRECTION::SOUTH) return currDomino.tailValue;
			else assert("Invalid direction." && false);
			break;
		case DOMINO_DIRECTION::EAST:
			if (lineDirection == DOMINO_DIRECTION::EAST) return currDomino.headValue;
			else if (lineDirection == DOMINO_DIRECTION::WEST) return currDomino.tailValue;
			else assert("Invalid direction." && false);
			break;
		case DOMINO_DIRECTION::SOUTH:
			if (lineDirection == DOMINO_DIRECTION::NORTH) return currDomino.tailValue;
			else if (lineDirection == DOMINO_DIRECTION::SOUTH) return currDomino.headValue;
			else assert("Invalid direction." && false);
			break;
		case DOMINO_DIRECTION::WEST:
			if (lineDirection == DOMINO_DIRECTION::EAST) return currDomino.tailValue;
			else if (lineDirection == DOMINO_DIRECTION::WEST) return currDomino.headValue;
			else assert("Invalid direction." && false);
	}

	return ""; // BIG error
}

bool DominoBoard::checkDominoes(std::vector<entt::entity> dominoes, entt::registry& registry) {
	Domino& head = registry.get<Domino>(dominoChain.front());
	Domino& tail = registry.get<Domino>(dominoChain.back());

	// Check every player domino to see if it could match with either the head or the tail
	for (entt::entity dominoID : dominoes) {
		Domino& currDomino = registry.get<Domino>(dominoID);
		if (currDomino.placed == true) continue; // Skip placed dominoes
		std::string headValue = getEndValue(head);
		if (currDomino.headValue == headValue || currDomino.tailValue == headValue) return true;
		std::string tailValue = getEndValue(tail, true);
		if (currDomino.headValue == tailValue || currDomino.tailValue == tailValue) return true;
	}

	return false; // No match was found
}
