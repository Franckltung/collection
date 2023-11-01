#include <cassert>
#include "utility.hpp"
#include "box.hpp"
#include "circle.hpp"
#include "status.hpp"

Position Utility::getCentered(entt::entity entity, entt::registry& registry) {
	assert(registry.all_of<Position>(entity));
	assert(registry.all_of<Circle>(entity) != registry.all_of<Box>(entity)); // XOR; entity must have only circle or only box collider

	auto& position = registry.get<Position>(entity);
	if (registry.all_of<Circle>(entity)) return getCentered(position, registry.get<Circle>(entity));
	else return getCentered(position, registry.get<Box>(entity));
}

Position Utility::getCentered(const Position& position, const Box& box) {
	Position positionCentered;
	positionCentered.x = position.x + (box.width / 2);
	positionCentered.y = position.y + (box.height / 2);

	return positionCentered;
}

Position Utility::getCentered(const Position& position, const Circle& circle) {
	Position positionCentered;
	positionCentered.x = position.x + circle.radius;
	positionCentered.y = position.y + circle.radius;

	return positionCentered;
}

float Utility::calculateAngle(const Position& subject, const Position& target) {
	return std::atan2(target.y - subject.y, target.x - subject.x) * (180.0/M_PI);
}

int Utility::getRandom(int min, int max) {
	std::uniform_int_distribution<int> dis(min, max);
	return dis(seed);
}

float Utility::getRandom(float min, float max) {
	std::uniform_real_distribution<float> dis(min, max);
	return dis(seed);
}

void Utility::seedGenerator() {
	seed.seed(std::random_device{}());
}

bool Utility::epsilonEqual(float first, float second, float epsilon) {
	float abs = std::abs(first - second);
	if (abs < epsilon) return true;
	else return false;
}

bool Utility::getStatus(entt::entity ent, entt::registry& registry) {
	if (!registry.all_of<Status>(ent)) return true; // If it doesn't have a status it's always active

	return registry.get<Status>(ent).active;
}
