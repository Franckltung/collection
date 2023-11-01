// Franck Limtung
#ifndef ENTITY_FACTORY_HPP
#define ENTITY_FACTORY_HPP

#include "entt/entity/registry.hpp"
#include "textureLoadingSystem.hpp"
#include "guiSystem.hpp"
#include "soundLoadingSystem.hpp"
#include "playerSystem.hpp"
#include "dominoSystem.hpp"

class EntityFactory
{
	public:
		entt::entity buildDomino(float x, float y, std::string headValue, std::string tailValue, std::shared_ptr<TextureLoadingSystem> loader,
				std::shared_ptr<SoundLoadingSystem> soundLoader, entt::registry& registry);
		entt::entity buildMenu(std::shared_ptr<bool> inMenu, std::shared_ptr<bool> newGame, std::shared_ptr<bool> quit,
				std::shared_ptr<DominoSystem> dominoSystem, std::shared_ptr<PlayerSystem> playerSystem,
				std::shared_ptr<SoundLoadingSystem> soundLoadingSystem, std::shared_ptr<GuiSystem> guiSystem,
				std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry);
		entt::entity buildSoundEffect(std::string filename, std::shared_ptr<SoundLoadingSystem> loader, entt::registry& registry);
		// buildText will use the x y position as the center of the text
		entt::entity buildText(std::string text, float x, float y, SDL_Color color, std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry);
		entt::entity buildButton(std::shared_ptr<TextureLoadingSystem> loader, entt::registry& registry, int x,
				int y, GUI_SCENE scene, RENDER_LAYER layer, std::string text);
};

#endif
