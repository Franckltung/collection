// Franck Limtung
// 2023-Jul
// This class stores the logical representation of the board
#ifndef DOMINO_BOARD_HPP
#define DOMINO_BOARD_HPP

#include <list>
#include "entt/entity/registry.hpp"
#include "domino.hpp"

class DominoBoard
{
	public:
		DominoBoard(entt::entity startingDomino, entt::registry& registryy);

		void resetBoard(entt::entity startingDomino, entt::registry& registry);

		// Return domino value only if it succeeds in connecting to the stem domino
		bool addDominoToHead(entt::entity domino, entt::registry& registry, std::string& matchedValue);
		bool addDominoToTail(entt::entity domino, entt::registry& registry, std::string& matchedValue);
		entt::entity getHead() { return dominoChain.front(); }
		entt::entity getTail() { return dominoChain.back(); }

		// Get next predefined position and update iterator
		void getNextHeadPosition(float& posX, float& posY, float& changeAngle);
		void getNextTailPosition(float& posX, float& posY, float& changeAngle);

		// Che current domino hand to see if he needs to go to the boneyard. Return true if there is a solution
		bool checkDominoes(std::vector<entt::entity> dominoes, entt::registry& registry);

		DOMINO_DIRECTION getHeadDirection() { return headDirection; }
		DOMINO_DIRECTION getTailDirection() { return tailDirection; }

		int getHeadLength() { return (positions.size() / 2.0 - 1) - positionsHeadIter; }
		int getTailLength() { return positionsTailIter - (positions.size() / 2.0 - 1); }
	private:
		std::string getEndValue(Domino& currDomino, bool tail = false); // Get the value the player domino has to match to add his domino to the chain

		std::list<entt::entity> dominoChain;
		std::vector<std::tuple<float, float, DOMINO_DIRECTION>> positions; // Stores calculated positions for dominoes to be placed in, and possible rotations
		int positionsHeadIter;
		int positionsTailIter;
		// Current directions of the head and tail, these determine the logic of the placement function
		DOMINO_DIRECTION headDirection = DOMINO_DIRECTION::WEST;
		DOMINO_DIRECTION tailDirection = DOMINO_DIRECTION::EAST;
};

#endif
