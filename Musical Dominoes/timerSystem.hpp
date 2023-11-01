// 2023-Jul
#ifndef TIMER_SYSTEM_HPP
#define TIMER_SYSTEM_HPP

#include "entt/entity/registry.hpp"

class TimerSystem
{
	public:
		void process(entt::registry& registry, int dTicks);
};

#endif
