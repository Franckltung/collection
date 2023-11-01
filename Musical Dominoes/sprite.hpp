// 2023-May
// Sprite holds a link to a texture and chooses what section of that texture to render

#include <vector>
#include "entt/entity/registry.hpp"

struct SpriteClip
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int lifespan = 0; // In ticks
};

struct Sprite
{
	std::vector<SpriteClip> sprites;
	bool active = false; // Should we keep changing sprites?
	int currSprite = 0;
	int tickCount = 0; // Ticks counted so far. Set to zero whenever the sprite changes
};
