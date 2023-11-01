// Franck Limtung
// 2023-Aug
#ifndef DOMINO_SYSTEM_HPP
#define DOMINO_SYSTEM_HPP

#include <iostream>
#include <unordered_map>
#include "entt/entity/registry.hpp"
#include "inputSystem.hpp"
#include "dominoBoard.hpp"
#include "playerSystem.hpp"
#include "boneYard.hpp"
#include "sound.hpp"
#include "soundLoadingSystem.hpp"

class DominoSystem
{
	public:
		// Domino types is a list of all sound files matching this pattern: resources/*DOMINO TYPE*_effect.ogg TODO CHANGE TO MP3?
		DominoSystem(entt::entity startingDomino, int boneYardX, int boneYardY, entt::registry& registry)
				: board(startingDomino, registry), boneYard(boneYardX, boneYardY) {}
		bool process(entt::registry& registry, InputSystem& inputSystem, std::shared_ptr<PlayerSystem> playerSystem, float zoomLevel); // Returns true if domino was placed
		DominoBoard& getBoard() { return board; }
		BoneYard& getBoneYard() { return boneYard; }
		void resetBoard(entt::entity startingDomino, entt::registry& registry) { board.resetBoard(startingDomino, registry); }
	private:
		// Calculate offset required for the attachment point for the end of the domino line
		Position calculateDominoOffset(Position& currPosition, DOMINO_DIRECTION lineDirection);
		DominoBoard board;
		BoneYard boneYard;
		std::unordered_map<std::string, entt::entity> dominoSounds; // Custom sounds played by each domino
};

#endif
