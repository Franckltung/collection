// 2023-Jun
#ifndef ROTATION_SYSTEM_HPP
#define ROTATION_SYSTEM_HPP

#include "entt/entity/registry.hpp"

class RotationSystem
{
	public:
		void process(entt::registry& registry, int dTicks);
};

#endif
