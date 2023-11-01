#ifndef BUTTON_SYSTEM_HPP
#define BUTTON_SYSTEM_HPP

#include "entt/entity/registry.hpp"

class ButtonSystem
{
	public:
		ButtonSystem(entt::entity clickEffect) : clickEffect(clickEffect) {}

		void process(entt::registry& registry);
	private:
		entt::entity clickEffect;
};

#endif
