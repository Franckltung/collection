#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "entityFactory.hpp"
#include "domino.hpp"
#include "position.hpp"
#include "box.hpp"
#include "angle.hpp"
#include "status.hpp"
#include "utility.hpp"
#include "sprite.hpp"
#include "clickable.hpp"
#include "rotationVelocity.hpp"
#include "constants.hpp"
#include "guiElement.hpp"
#include "button.hpp"
#include "renderLayer.hpp"

entt::entity EntityFactory::buildDomino(float x, float y, std::string headValue, std::string tailValue, std::shared_ptr<TextureLoadingSystem> loader,
		std::shared_ptr<SoundLoadingSystem> soundLoader, entt::registry& registry) {
	entt::entity domino = registry.create();
	auto& dominoComp = registry.emplace<Domino>(domino, headValue, tailValue);
	dominoComp.startingX = x;
	dominoComp.startingY = y;
	registry.emplace<Position>(domino, x, y);
	registry.emplace<Box>(domino, Constants::DOMINO_WIDTH, Constants::DOMINO_HEIGHT); // width and height should be swapped when the domino is rotated
	auto& angle = registry.emplace<Angle>(domino);
	angle.angle = 0;
	auto& rotationVelocity = registry.emplace<RotationVelocity>(domino);
	rotationVelocity.desiredAngle = angle.angle;
	registry.emplace<Status>(domino, true);
	registry.emplace<Clickable>(domino);

	// Load surfaces
	int headTextureNum = Utility::getRandom(0, 3);
	SDL_Surface* surfaceHead = IMG_Load(("resources/domino_" + headValue + "_" +
			std::to_string(headTextureNum) + ".png").c_str());
	assert(surfaceHead != NULL);
	SDL_Surface* surfaceTail;
	// If it's a double domino, I think it looks best if it always has identical icons
	if (headValue == tailValue) {
		//surfaceTail = surfaceHead;
		surfaceTail = IMG_Load(("resources/domino_" + headValue + "_" +
				std::to_string(headTextureNum) + ".png").c_str());
	} else {
		surfaceTail = IMG_Load(("resources/domino_" + tailValue + "_" +
				std::to_string(Utility::getRandom(0, 3)) + ".png").c_str());
	}
	assert(surfaceTail != NULL);
	SDL_Surface* surfaceBase = IMG_Load("resources/domino_base_sided.png");
	assert(surfaceBase != NULL);

	// Create texture using combination of three different surfaces
	SDL_Rect headRect = {0, 0, 470, 450};
	SDL_Rect tailRect = {530, 0, 470, 450};
	SDL_BlitSurface(surfaceHead, NULL, surfaceBase, &headRect);
	SDL_BlitSurface(surfaceTail, NULL, surfaceBase, &tailRect);
	auto texture = registry.emplace<std::shared_ptr<Texture>>(domino, std::make_shared<Texture>());
	texture->texture = SDL_CreateTextureFromSurface(loader->getRenderer(), surfaceBase);
	assert(texture->texture != NULL);
	texture->width = Constants::DOMINO_WIDTH;
	texture->height = Constants::DOMINO_HEIGHT;
	texture->initialized = true;
	texture->floating = true;

	SDL_FreeSurface(surfaceHead);
	SDL_FreeSurface(surfaceTail);
	SDL_FreeSurface(surfaceBase);

	auto& sprite = registry.emplace<Sprite>(domino);
	sprite.sprites = {{0, 0, 1000, 450, 0}, {0, 451, 1000, 450, 0}}; // Front and back side
	registry.emplace<RenderLayer>(domino, RENDER_LAYER::WORLD_LAYER);

	// Sound effects
	dominoComp.pickUpEffect = buildSoundEffect("resources/pickUpSound.ogg", soundLoader, registry);
	dominoComp.placeEffect = buildSoundEffect("resources/placeSound.ogg", soundLoader, registry);

	return domino;
}

entt::entity EntityFactory::buildMenu(std::shared_ptr<bool> inMenu, std::shared_ptr<bool> newGame, std::shared_ptr<bool> quit,
		std::shared_ptr<DominoSystem> dominoSystem, std::shared_ptr<PlayerSystem> playerSystem,
		std::shared_ptr<SoundLoadingSystem> soundLoadingSystem, std::shared_ptr<GuiSystem> guiSystem,
		std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry) {
	entt::entity background = registry.create();
	registry.emplace<std::shared_ptr<Texture>>(background, loader->getTexture("resources/menuBG.png", true));
	registry.emplace<RenderLayer>(background, RENDER_LAYER::GUI_BG_LAYER);
	registry.emplace<Position>(background);
	registry.emplace<Status>(background);
	registry.emplace<GuiElement>(background, GUI_SCENE::MENU);

	entt::entity playButton = buildButton(loader, registry, Constants::WINDOW_WIDTH / 2.0 - 100,
				Constants::WINDOW_HEIGHT / 4.0 + 90, GUI_SCENE::MENU, RENDER_LAYER::GUI_LAYER, "New Game");
	auto& button = registry.get<Button>(playButton);
	// We have to use weak pointers because some guy said that lambda functions break shared ptrs
	std::weak_ptr<bool> inMenuPtr = inMenu;
	std::weak_ptr<GuiSystem> guiSystemPtr = guiSystem;
	std::weak_ptr<PlayerSystem> playerSystemPtr = playerSystem;
	std::weak_ptr<DominoSystem> dominoSystemPtr = dominoSystem;
	std::weak_ptr<TextureLoadingSystem> loaderPtr = loader;
	std::weak_ptr<SoundLoadingSystem> soundLoaderPtr = soundLoadingSystem;
	std::weak_ptr<bool> newGamePtr = newGame;
	button.event = [this, newGamePtr, guiSystemPtr, playerSystemPtr, dominoSystemPtr, loaderPtr, soundLoaderPtr,
			&registry, inMenuPtr] {
		std::shared_ptr<GuiSystem> guiSystem = guiSystemPtr.lock();
		std::shared_ptr<bool> inMenu = inMenuPtr.lock();
		std::shared_ptr<PlayerSystem> playerSystem = playerSystemPtr.lock();
		std::shared_ptr<TextureLoadingSystem> loader = loaderPtr.lock();
		std::shared_ptr<SoundLoadingSystem> soundLoader = soundLoaderPtr.lock();
		std::shared_ptr<DominoSystem> dominoSystem = dominoSystemPtr.lock();
		std::shared_ptr<bool> newGame = newGamePtr.lock();

		// Exit menu
		guiSystem->unloadGUI(registry);
		guiSystem->loadScene(GUI_SCENE::GAME, registry);
		*inMenu = false;

		// Start new game
		playerSystem->deletePlayers();
		std::shared_ptr<Player> player1 = playerSystem->addPlayer();
		std::shared_ptr<Player> player2 = playerSystem->addPlayer();

		// Delete old dominoes if they exist
		auto slice = registry.view<Domino>();
		for (auto iter : slice) {
			registry.destroy(iter);
		}

		// Create new dominoes
		dominoSystem->getBoneYard().getDominoes().clear();
		dominoSystem->getBoneYard().setCarouselPosition(0, Constants::WINDOW_HEIGHT - Constants::DOMINO_HEIGHT * 2, registry);

		// Select domino values from grab bag by eliminating options
		std::vector<std::string> values = {"quarter", "half", "whole", "eight", "wholehalf", "halfquarter", "quartereight", "eightsixteen"};
		while (values.size() > 6) {
			values.erase(values.begin() + Utility::getRandom(0, values.size() - 1));
		}
		// always have zero
		values.push_back("0");

		// Double six set
		std::vector<entt::entity> dominoes;
		for (unsigned int i = 0; i < values.size(); i++) {
			for (unsigned int i2 = 0; i2 <= i; i2++) {
				dominoes.push_back(this->buildDomino(0, 0, values[i], values[i2], loader, soundLoader, registry));
			}
		}
		assert(dominoes.size() == 28);

		// Randomize dominoes
		for (int i = dominoes.size() - 1; i >= 0; i--) {
			entt::entity temp = dominoes[i];
			int randomIndex = Utility::getRandom(0, i);
			dominoes[i] = dominoes[randomIndex];
			dominoes[randomIndex] = temp;
		}
		for (int i = 0; i < 7; i++) {
			player1->addDomino(dominoes.back());
			dominoes.erase(dominoes.begin() + dominoes.size() - 1);
		}
		for (int i = 0; i < 7; i++) {
			player2->addDomino(dominoes.back());
			dominoes.erase(dominoes.begin() + dominoes.size() - 1);
		}
		// Set starting domino
		auto startingDomino = dominoes[dominoes.size() - 1];
		registry.get<Position>(startingDomino) = {Constants::WINDOW_WIDTH / 2 - 50, Constants::WINDOW_HEIGHT / 2};
		registry.get<Domino>(startingDomino).direction = DOMINO_DIRECTION::WEST;
		registry.get<Domino>(startingDomino).startingX = Constants::WINDOW_WIDTH / 2 - 50;
		registry.get<Domino>(startingDomino).startingY = Constants::WINDOW_HEIGHT / 2.0;
		registry.get<Angle>(startingDomino).angle = 0;
		registry.get<RotationVelocity>(startingDomino).desiredAngle = 0;
		registry.get<Box>(startingDomino) = {100, 45};
		registry.get<std::shared_ptr<Texture>>(startingDomino)->floating = false;
		registry.remove<Clickable>(startingDomino);
		dominoes.erase(dominoes.begin() + dominoes.size() - 1);

		dominoSystem->resetBoard(startingDomino, registry);

		// Add left-over to boneyard
		for (auto domino : dominoes) {
			dominoSystem->getBoneYard().addDomino(domino, registry);
		}

		player1->orderDominoes(registry);
		player2->orderDominoes(registry);

		playerSystem->setActivePlayer(0, registry);
		*newGame = true;

		// Handle strange case of player needing boneyard at the start
		if (!dominoSystem->getBoard().checkDominoes(playerSystem->getActivePlayer()->getDominoes(), registry)) {
			dominoSystem->getBoneYard().setActive(registry);
		}
	};

	entt::entity quitButtonID = buildButton(loader, registry, Constants::WINDOW_WIDTH / 2.0 - 100,
				Constants::WINDOW_HEIGHT / 4.0 + 200, GUI_SCENE::MENU, RENDER_LAYER::GUI_LAYER, "Quit");
	auto& quitButton = registry.get<Button>(quitButtonID);
	std::weak_ptr<bool> quitPtr = quit;
	quitButton.event = [quitPtr] {
		std::shared_ptr<bool> quit = quitPtr.lock();
		*quit = true;
	};

	// Title
	entt::entity titleID = registry.create();
	registry.emplace<std::shared_ptr<Texture>>(titleID, loader->getTexture("resources/titleNew.png", true, {620, 400}));
	registry.emplace<RenderLayer>(titleID, RENDER_LAYER::GUI_LAYER);
	registry.emplace<Status>(titleID);
	registry.emplace<GuiElement>(titleID, GUI_SCENE::MENU);
	registry.emplace<Position>(titleID, Constants::WINDOW_WIDTH / 2.0 - 310, -50);

	return background;
}

entt::entity EntityFactory::buildSoundEffect(std::string filename, std::shared_ptr<SoundLoadingSystem> loader, entt::registry& registry) {
	entt::entity soundID = registry.create();
	registry.emplace<std::shared_ptr<Sound>>(soundID, loader->getSound(filename));
	registry.emplace<Status>(soundID);

	return soundID;
}

entt::entity EntityFactory::buildText(std::string text, float x, float y, SDL_Color color, std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry) {
	entt::entity textID = registry.create();
	registry.emplace<Status>(textID);

	int textWidth = 0;
	int textHeight = 0;
	registry.emplace<std::shared_ptr<Texture>>(textID, loader->getTextTexture(text, color, false));
	// Get center
	TTF_SizeUTF8(loader->getFont(), text.c_str(), &textWidth, &textHeight);
	registry.emplace<Position>(textID, x - textWidth / 2.0, y - textHeight / 2.0);
	return textID;
}

entt::entity EntityFactory::buildButton(std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry, int x,
		int y, GUI_SCENE scene, RENDER_LAYER layer, std::string text) {
	entt::entity button = registry.create();
	registry.emplace<std::shared_ptr<Texture>>(button, loader->getTexture("resources/button.png", true, {200, 90}));
	registry.emplace<RenderLayer>(button, layer);
	registry.emplace<Status>(button);
	registry.emplace<GuiElement>(button, scene);
	registry.emplace<Position>(button, x, y);
	registry.emplace<Box>(button, 200, 90);
	auto& sprite = registry.emplace<Sprite>(button);
	sprite.sprites = {{0, 0, 1006, 454, 0}, {0, 455, 1006, 451, 0}};
	registry.emplace<Clickable>(button);
	registry.emplace<Button>(button); // User has to retrieve this themselves and make it do something

	entt::entity playButtonText = buildText(text, x + 100, y + 45, {255, 255, 255, 255}, loader, registry);
	registry.emplace<GuiElement>(playButtonText, scene);
	registry.emplace<RenderLayer>(playButtonText, RENDER_LAYER::GUI_TEXT_LAYER);

	return button;
}
