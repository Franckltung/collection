// Franck Limtung
// 2023-Jul

#include "entt/entity/registry.hpp"
#include "renderSystem.hpp"

class SoundSystem
{
	public:
		SoundSystem(const RenderSystem& renderSystem) {
			// Render system should be initialized before using this
			assert(renderSystem.isInitialized());
		}
		void process(entt::registry& registry);
};
