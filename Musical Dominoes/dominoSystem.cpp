#include <iostream>
#include "dominoSystem.hpp"
#include "domino.hpp"
#include "clickable.hpp"
#include "box.hpp"
#include "position.hpp"
#include "utility.hpp"
#include "constants.hpp"
#include "angle.hpp"
#include "texture.hpp"
#include "rotationVelocity.hpp"
#include "playerSystem.hpp"
#include "renderLayer.hpp"
#include "sprite.hpp"

bool DominoSystem::process(entt::registry& registry, InputSystem& inputSystem, std::shared_ptr<PlayerSystem> playerSystem, float zoomLevel) {
	bool dominoWasPlaced = false; // Returns true if a player placed something so we can perform checks in the main function
	auto slice = registry.view<Sprite, Domino, Clickable, Box, Position, Angle, std::shared_ptr<Texture>, RotationVelocity, Status, RenderLayer>();
	for (auto iter : slice) {
		if (slice.get<Status>(iter).active == false) continue;

		auto& clickable = slice.get<Clickable>(iter);
		if (clickable.clicked == true) {
			auto& domino = slice.get<Domino>(iter);
			// Handle bone yard
			if (domino.inBoneYard == true) {
				domino.inBoneYard = false;
				playerSystem->getActivePlayer()->addDomino(iter);
				playerSystem->getActivePlayer()->orderDominoes(registry);
				auto& sprite = registry.get<Sprite>(iter);
				sprite.currSprite = 0;
				clickable.clicked = false;
				registry.get<Status>(domino.pickUpEffect).active = true;
				registry.get<RenderLayer>(iter).layer = RENDER_LAYER::WORLD_LAYER;

				boneYard.removeGrabbedDominoes(registry);
				return true;
			}

			auto& position = slice.get<Position>(iter);
			auto& activeInput = registry.get<ActiveInput>(inputSystem.activeInputID);
			const auto& mousePosition = activeInput.mousePosition;
			auto& box = slice.get<Box>(iter);
			// It's a little complicated to figure out what this does, but it works
			position.x = mousePosition.x - (Constants::WINDOW_WIDTH / zoomLevel - Constants::WINDOW_WIDTH) / 2.0 - Constants::DOMINO_WIDTH / 2.0;
			position.y = mousePosition.y - (Constants::WINDOW_HEIGHT / zoomLevel - Constants::WINDOW_HEIGHT) / 2.0 - Constants::DOMINO_HEIGHT / 2.0;

			auto texture = slice.get<std::shared_ptr<Texture>>(iter);
			texture->floating = false;
			auto& renderLayer = slice.get<RenderLayer>(iter);
			renderLayer.layer = RENDER_LAYER::GRAB_LAYER;

			// Handle rotation
			if (activeInput.mouseRight.active) {
				auto& angleChange = slice.get<RotationVelocity>(iter);
				angleChange.desiredAngle += 90;
				if (angleChange.desiredAngle >= 180) angleChange.desiredAngle -= 360;

				activeInput.mouseRight.active = false; // TODO: Might not be good to change activeInput from outside
				// Change direction and box shape when rotating
				float temp = box.width;
				box.width = box.height;
				box.height = temp;

				switch (domino.direction) {
					case DOMINO_DIRECTION::NORTH:
						domino.direction = DOMINO_DIRECTION::EAST;
						//angle.angle = 180;
						break;
					case DOMINO_DIRECTION::EAST:
						domino.direction = DOMINO_DIRECTION::SOUTH;
						//angle.angle = -90;
						break;
					case DOMINO_DIRECTION::SOUTH:
						domino.direction = DOMINO_DIRECTION::WEST;
						//angle.angle = 0;
						break;
					case DOMINO_DIRECTION::WEST:
						domino.direction = DOMINO_DIRECTION::NORTH;
						//angle.angle = 90;
				}
			}
		}
		else if (clickable.unclicked == true) {
			Position headPositionOffset = Utility::getCentered(board.getHead(), registry);
			Position tailPositionOffset = Utility::getCentered(board.getTail(), registry);
			Position& position = slice.get<Position>(iter);
			Position positionCen = Utility::getCentered(iter, registry);
			auto& domino = slice.get<Domino>(iter);

			// Offset head and tail positions based on current domino chain orientation
			headPositionOffset = calculateDominoOffset(headPositionOffset, board.getHeadDirection());
			tailPositionOffset = calculateDominoOffset(tailPositionOffset, board.getTailDirection());

			// Set texture back to world because it's not being grabbed any more
			auto& renderLayer = slice.get<RenderLayer>(iter);
			renderLayer.layer = RENDER_LAYER::WORLD_LAYER;

			// Stores matched value so we can create the right sound
			std::string matchedValue;

			if ((std::pow(positionCen.x - headPositionOffset.x, 2) +
					std::pow(positionCen.y - headPositionOffset.y, 2) < std::pow(Constants::SNAP_DISTANCE, 2)) &&
					board.addDominoToHead(iter, registry, matchedValue)) {
				float angle = 0;
				board.getNextHeadPosition(domino.startingX, domino.startingY, angle);
				if (angle == -90) {
					auto& angleChange = slice.get<RotationVelocity>(iter);
					angleChange.changeAngle(angle);

					// Rotate domino appropriately
					int index = static_cast<int>(domino.direction) - 1;
					if (index == -1) index = 3;
					domino.direction = static_cast<DOMINO_DIRECTION>(index);
				}
				if (angle == 90) {
					auto& angleChange = slice.get<RotationVelocity>(iter);
					angleChange.changeAngle(angle);

					int index = static_cast<int>(domino.direction) + 1;
					if (index == 4) index = 0;
					domino.direction = static_cast<DOMINO_DIRECTION>(index);
				}

				// Micro position adjustments for upside down dominoes TODO: Can we remove the need for this
				if (domino.direction == DOMINO_DIRECTION::EAST) domino.startingY += 1;
				else if (domino.direction == DOMINO_DIRECTION::NORTH) domino.startingX += 1;

				registry.get<Status>(domino.placeEffect).active = true;

				domino.placed = true;
				playerSystem->getActivePlayer()->removePlacedDominoes(registry);
				dominoWasPlaced = true;

				if (domino.headValue != domino.tailValue || Constants::REPLAY_WITH_DOUBLE == false) playerSystem->changePlayer(registry); // Change player based on rules
				registry.remove<Clickable>(iter); // Make it impossible to move
			}
			else if ((std::pow(positionCen.x - tailPositionOffset.x, 2) +
					std::pow(positionCen.y - tailPositionOffset.y, 2) < std::pow(Constants::SNAP_DISTANCE, 2)) &&
					board.addDominoToTail(iter, registry, matchedValue)) {
				float angle = 0;
				board.getNextTailPosition(domino.startingX, domino.startingY, angle);
				if (angle == -90) {
					auto& angleChange = slice.get<RotationVelocity>(iter);
					angleChange.changeAngle(angle);

					// Rotate domino appropriately
					int index = static_cast<int>(domino.direction) - 1;
					if (index == -1) index = 3;
					domino.direction = static_cast<DOMINO_DIRECTION>(index);
				}
				if (angle == 90) {
					auto& angleChange = slice.get<RotationVelocity>(iter);
					angleChange.changeAngle(angle);

					int index = static_cast<int>(domino.direction) + 1;
					if (index == 4) index = 0;
					domino.direction = static_cast<DOMINO_DIRECTION>(index);
				}

				if (domino.direction == DOMINO_DIRECTION::EAST) domino.startingY += 1;
				else if (domino.direction == DOMINO_DIRECTION::NORTH) domino.startingX += 1;

				registry.get<Status>(domino.placeEffect).active = true;
				domino.placed = true;
				playerSystem->getActivePlayer()->removePlacedDominoes(registry);
				dominoWasPlaced = true;

				if (domino.headValue != domino.tailValue || Constants::REPLAY_WITH_DOUBLE == false) playerSystem->changePlayer(registry);

				registry.remove<Clickable>(iter);
			}
			else {
				// Go back to player hand and rotate back to WEST
				domino.direction = DOMINO_DIRECTION::WEST;
				slice.get<Angle>(iter).angle = 0;
				slice.get<RotationVelocity>(iter).desiredAngle = slice.get<Angle>(iter).angle;
				auto& box = slice.get<Box>(iter);
				box.width = Constants::DOMINO_WIDTH;
				box.height = Constants::DOMINO_HEIGHT;
				auto texture = slice.get<std::shared_ptr<Texture>>(iter);
				texture->floating = true;
			}

			// Go to whatever position is the new starting position
			position.x = domino.startingX;
			position.y = domino.startingY;
		}
	}

	return dominoWasPlaced;
}

Position DominoSystem::calculateDominoOffset(Position& currPosition, DOMINO_DIRECTION lineDirection) {
	Position offsetPosition = currPosition;

	switch (lineDirection) {
		case DOMINO_DIRECTION::NORTH:
			offsetPosition.y -= Constants::DOMINO_WIDTH;
			break;
		case DOMINO_DIRECTION::EAST:
			offsetPosition.x += Constants::DOMINO_WIDTH;
			break;
		case DOMINO_DIRECTION::WEST:
			offsetPosition.x -= Constants::DOMINO_WIDTH;
			break;
		case DOMINO_DIRECTION::SOUTH:
			offsetPosition.y += Constants::DOMINO_WIDTH;
	}

	return offsetPosition;
}
