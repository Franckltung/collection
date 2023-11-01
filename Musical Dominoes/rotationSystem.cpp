#include "rotationSystem.hpp"
#include "angle.hpp"
#include "rotationVelocity.hpp"
#include "utility.hpp"

void RotationSystem::process(entt::registry& registry, int dTicks) {
	if (dTicks == 0) return;

	auto slice = registry.view<Angle, RotationVelocity>();
	for (auto iter : slice) {
		Angle& angle = slice.get<Angle>(iter);
		RotationVelocity& vel = slice.get<RotationVelocity>(iter);
		int dTicksCopy = dTicks;
		while (angle.angle != vel.desiredAngle && dTicksCopy > 0) {
			angle.changeAngle(vel.speed);
			dTicksCopy--;
			// Angle will be set when it's close enough to the desired angle so we don't overshoot
			if (Utility::epsilonEqual(angle.angle, vel.desiredAngle, vel.speed * 2)) {
				angle.angle = vel.desiredAngle;
			}
		}
	}
}
