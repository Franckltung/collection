#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <random>
#include "entt/entity/registry.hpp"
#include "position.hpp"
#include "box.hpp"
#include "circle.hpp"

namespace Utility
{
	Position getCentered(entt::entity entity, entt::registry& registry); // Entity should have a position and a box or circle component
	Position getCentered(const Position& position, const Box& box);
	Position getCentered(const Position& position, const Circle& circle);
	float calculateAngle(const Position& subject, const Position& target);

	int getRandom(int min, int max);
	float getRandom(float min, float max);
	void seedGenerator(); // Seed random generator before using random functions

	bool epsilonEqual(float first, float second, float epsilon); // Are the two values approximately equal?

	bool getStatus(entt::entity ent, entt::registry& registry);

	namespace
	{
		std::mt19937_64 seed(1337);
	}
};

#endif
