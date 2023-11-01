// Franck Limtung
// 2023-Aug
// Just a simple, unambitious domino game with a musical twist
// RESUME GAME?
// MUSIC ?
// PLAY NOTE OF END DOMINO WHEN IT'S PLACED?
#include <chrono>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <thread>
#include <iostream>
#include "entt/entity/registry.hpp"
#include "textureLoadingSystem.hpp"
#include "entityFactory.hpp"
#include "utility.hpp"
#include "cameraSystem.hpp"
#include "renderSystem.hpp"
#include "inputSystem.hpp"
#include "dominoSystem.hpp"
#include "constants.hpp"
#include "angle.hpp"
#include "domino.hpp"
#include "clickable.hpp"
#include "rotationSystem.hpp"
#include "rotationVelocity.hpp"
#include "player.hpp"
#include "playerSystem.hpp"
#include "guiSystem.hpp"
#include "buttonSystem.hpp"
#include "soundSystem.hpp"
#include "soundLoadingSystem.hpp"
#include "timerSystem.hpp"
#include "timer.hpp"

int main(int argc, char* argv[]) {
	entt::registry registry;

	Utility::seedGenerator();

	// Setup input
	InputSystem inputSystem(registry);

	// Setup renderer
	CameraSystem cameraSystem(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, registry);
	RenderSystem renderSystem("Musical Dominoes", Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, inputSystem.activeInputID, cameraSystem.getCameraID());

	// Setup sound
	std::shared_ptr<SoundLoadingSystem> soundLoadingSystem = std::make_shared<SoundLoadingSystem>(renderSystem);
	SoundSystem soundSystem(renderSystem);

	std::shared_ptr<TextureLoadingSystem> textureLoadingSystem = std::make_shared<TextureLoadingSystem>(renderSystem.getRenderer(), "Sansation_Regular.ttf");

	// Setup BG
	auto bgID = registry.create();
	auto& bgTexture = registry.emplace<std::shared_ptr<Texture>>(bgID);
	bgTexture = textureLoadingSystem->getTexture("resources/background.png", false);
	registry.emplace<RenderLayer>(bgID, RENDER_LAYER::BG_LAYER);
	renderSystem.setBackground(bgID);

	std::shared_ptr<PlayerSystem> playerSystem = std::make_shared<PlayerSystem>();

	EntityFactory entityFactory;

	auto startingDomino = entityFactory.buildDomino(Constants::WINDOW_WIDTH / 2 - 50, Constants::WINDOW_HEIGHT / 2, "half", "whole", textureLoadingSystem, soundLoadingSystem, registry);
	registry.get<Domino>(startingDomino).direction = DOMINO_DIRECTION::WEST;
	registry.get<Angle>(startingDomino).angle = 0;
	registry.get<RotationVelocity>(startingDomino).desiredAngle = 0;
	registry.get<Box>(startingDomino) = {100, 45};
	registry.get<std::shared_ptr<Texture>>(startingDomino)->floating = false;
	registry.remove<Clickable>(startingDomino);

	// Setup centered camera
	entt::entity cameraPos = registry.create();
	registry.emplace<Position>(cameraPos, Constants::WINDOW_WIDTH / 2.0, Constants::WINDOW_HEIGHT / 2.0);
	registry.emplace<Box>(cameraPos, 1, 1);
	cameraSystem.attach(cameraPos);
	float zoomLevel = 1;
	float gameZoomLevel = 1; // Zoom level desired when not in menu

	std::shared_ptr<DominoSystem> dominoSystem = std::make_shared<DominoSystem>(startingDomino,
			0, Constants::WINDOW_HEIGHT - Constants::DOMINO_HEIGHT * 2, registry);
	RotationSystem rotationSystem;

	TimerSystem timerSystem;

	// Setup GUI
	std::shared_ptr<bool> quit = std::make_shared<bool>(false);
	std::shared_ptr<bool> inMenu = std::make_shared<bool>(true);
	std::shared_ptr<bool> newGame = std::make_shared<bool>(false); // Flag to notify if a new game was created
	std::shared_ptr<GuiSystem> guiSystem = std::make_shared<GuiSystem>();
	entityFactory.buildMenu(inMenu, newGame, quit, dominoSystem, playerSystem, soundLoadingSystem, guiSystem, textureLoadingSystem, registry);
	ButtonSystem buttonSystem(entityFactory.buildSoundEffect("resources/click.mp3", soundLoadingSystem, registry));

	entt::entity player1Text = entityFactory.buildText("Player 1's turn", Constants::WINDOW_WIDTH / 2.0, 20, {255, 0, 0, 255}, textureLoadingSystem, registry);
	entt::entity player2Text = entityFactory.buildText("Player 2's turn", Constants::WINDOW_WIDTH / 2.0, 20, {0, 0, 255, 255}, textureLoadingSystem, registry);
	registry.emplace<RenderLayer>(player1Text, RENDER_LAYER::GUI_TEXT_LAYER);
	registry.emplace<RenderLayer>(player2Text, RENDER_LAYER::GUI_TEXT_LAYER);
	registry.emplace<GuiElement>(player1Text, GUI_SCENE::GAME);
	registry.emplace<GuiElement>(player2Text, GUI_SCENE::GAME);

	// Load some sounds
	entt::entity success_bell = entityFactory.buildSoundEffect("resources/success_bell.mp3", soundLoadingSystem, registry);
	entt::entity errorSound = entityFactory.buildSoundEffect("resources/error.mp3", soundLoadingSystem, registry);

	guiSystem->loadScene(GUI_SCENE::MENU, registry);

	// Prevent button presses from being triggered every tick
	bool pKeyRepeat = false;
	// Stop processing dominoes if a player won
	bool playerWon = false;

	// Arguments
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "--renderClickable") renderSystem.debugSetRenderClickables(true);
	}

	// Game loop
	double dt = 0;
	double dtLeftover = 0;
	int dTicks = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
	while (!*quit) {
		// Input
		*quit = inputSystem.process(registry, cameraSystem, zoomLevel);
		auto& activeInput = registry.get<ActiveInput>(inputSystem.activeInputID);
		if (activeInput.pKey.active == true || activeInput.escKey.active == true) {
			// Skip action if it already occurred without releasing the key
			if (pKeyRepeat == false) {
				pKeyRepeat = true;

				if (*inMenu == false) {
					*inMenu = true;
					guiSystem->unloadGUI(registry);
					guiSystem->loadScene(GUI_SCENE::MENU, registry);
				}
				else {
					*inMenu = false;
					guiSystem->unloadGUI(registry);
					guiSystem->loadScene(GUI_SCENE::GAME, registry);
				}
			}
		}
		else pKeyRepeat = false;

		buttonSystem.process(registry);

		// Set zoom
		/*
		if (activeInput.mouseScrolling.active == true && !*inMenu) {
			gameZoomLevel += activeInput.scrollValue * dTicks * 0.10;
			// Zoom range is 0.7 to 1.5
			if (gameZoomLevel < 0.7) gameZoomLevel = 0.7;
			else if (gameZoomLevel > 1.5) gameZoomLevel = 1.5;
			activeInput.mouseScrolling.active = false; // Scrolling happens in notches
		}
		*/
		// New game was requested by the menu - we have to reset the zoom
		if (*newGame == true) {
			*newGame = false;
			playerWon = false;
			gameZoomLevel = 1;
		}

		// Logic
		if (*inMenu == true) {
			// Reset zoom
			if (!Utility::epsilonEqual(zoomLevel, 1, 0.01)) {
				// Set zoom back to default
				zoomLevel = 1;
				cameraSystem.setCameraSize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
				SDL_RenderSetScale(renderSystem.getRenderer(), zoomLevel, zoomLevel);
			}
		}
		else {
			if (zoomLevel != gameZoomLevel) {
				zoomLevel = gameZoomLevel;
				cameraSystem.setCameraSize(Constants::WINDOW_WIDTH / zoomLevel, Constants::WINDOW_HEIGHT / zoomLevel);
				SDL_RenderSetScale(renderSystem.getRenderer(), zoomLevel, zoomLevel);
			}

			if (!playerWon && dominoSystem->process(registry, inputSystem, playerSystem, zoomLevel) == true) {
				// A domino was placed, we have to check certain things
				if (!Utility::epsilonEqual(gameZoomLevel, 0.88, 0.05) && (dominoSystem->getBoard().getHeadLength() > 11 || dominoSystem->getBoard().getTailLength() > 11)) {
					gameZoomLevel = 0.88;
					// Change boneyard position so it stays at the bottom of the screen
					dominoSystem->getBoneYard().setCarouselPosition(0, Constants::WINDOW_HEIGHT_ZOOM - Constants::DOMINO_HEIGHT * 2.0, registry, 16);
				}
				for (int i = 0; i < 2; i++) {
					if (playerSystem->getPlayer(i)->getDominoes().size() == 0) {
						// Player won
						registry.get<Status>(success_bell).active = true;
						entt::entity winText = entityFactory.buildText("Player " + std::to_string(i + 1) +
								" won! Press p to go back to the menu", Constants::WINDOW_WIDTH / 2.0, Constants::WINDOW_HEIGHT / 4.0,
								{255, 255, 255, 255}, textureLoadingSystem, registry);
						registry.emplace<GuiElement>(winText, GUI_SCENE::GAME, true);
						registry.get<Status>(winText).active = true;
						playerWon = true;
						break;
					}
				}
				if (!playerWon && !dominoSystem->getBoard().checkDominoes(playerSystem->getActivePlayer()->getDominoes(), registry)) {
					if (dominoSystem->getBoneYard().getDominoes().size() > 0) {
						// Player needs to get more dominoes
						dominoSystem->getBoneYard().setActive(registry);
					}
					else {
						// Check if both players are blocked
						std::shared_ptr<Player> nextPlayer = playerSystem->getPlayer((playerSystem->getActivePlayerID() + 1) % 2); // Get next player
						if (!dominoSystem->getBoard().checkDominoes(nextPlayer->getDominoes(), registry)) {
							// Both are blocked, just end the game TODO: SCORE?
							entt::entity loseText = entityFactory.buildText("Both players are blocked. Game over! Press p to go back to the menu",
									Constants::WINDOW_WIDTH / 2.0, Constants::WINDOW_HEIGHT / 4.0, {255, 255, 255, 255},
									textureLoadingSystem, registry);
							registry.emplace<GuiElement>(loseText, GUI_SCENE::GAME, true);
							registry.get<Status>(loseText).active = true;
							registry.get<Status>(errorSound).active = true;
							playerWon = true; // Not really, but it works for proper handling
						}
						else {
							// One player blocked
							entt::entity blockText = entityFactory.buildText("Player " + std::to_string(playerSystem->getActivePlayerID() + 1) +
									" blocked, skipping turn", Constants::WINDOW_WIDTH / 2.0, Constants::WINDOW_HEIGHT / 4.0,
									{255, 255, 255, 255}, textureLoadingSystem, registry);
							registry.emplace<GuiElement>(blockText, GUI_SCENE::GAME);
							registry.get<Status>(blockText).active = true;
							registry.get<Status>(errorSound).active = true;

							std::weak_ptr<PlayerSystem> playerSystemPtr = playerSystem; // Look in entityFactory for the reason why we need to do this
							registry.emplace<Timer>(blockText, 180, [&registry, blockText, playerSystemPtr]{
									std::shared_ptr<PlayerSystem> playerSystem = playerSystemPtr.lock();
								playerSystem->changePlayer(registry);
								registry.destroy(blockText);
							});
						}
					}
				}
				else if (dominoSystem->getBoneYard().isActive()) {
					// Player doesn't need boneyard any more
					dominoSystem->getBoneYard().setInactive(registry);
				}
			}

			// Current player text
			if (playerSystem->getActivePlayerID() == 0) {
				registry.get<Status>(player1Text).active = true;
				registry.get<Status>(player2Text).active = false;
			}
			else {
				registry.get<Status>(player2Text).active = true;
				registry.get<Status>(player1Text).active = false;
			}

			rotationSystem.process(registry, dTicks);
			timerSystem.process(registry, dTicks);
		}

		// Sound
		soundSystem.process(registry);

		// Camera
		cameraSystem.process(registry);

		// Render
		SDL_RenderClear(renderSystem.getRenderer());
		renderSystem.process(registry);
		SDL_RenderPresent(renderSystem.getRenderer());

		// Frametime calculation
		dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		// Cap fps to 60
		if (dt < Constants::TICK_TIME) {
			std::this_thread::sleep_for(std::chrono::microseconds((int)(Constants::TICK_TIME * 1000.0) - (int)(dt * 1000.0)));
			dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count();
		}
		startTime = std::chrono::high_resolution_clock::now();

		// Calculate ticks
		dt += dtLeftover;
		dTicks = 0;
		while (dt > Constants::TICK_TIME) {
			dTicks++;
			dt -= Constants::TICK_TIME;
		}
		// Left over
		dtLeftover = dt;
	}

	return 0;
}
