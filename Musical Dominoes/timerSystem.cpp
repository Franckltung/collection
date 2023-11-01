#include "timerSystem.hpp"
#include "timer.hpp"
#include "status.hpp"

void TimerSystem::process(entt::registry& registry, int dTicks) {
	if (dTicks == 0) return;

	auto slice = registry.view<Timer>();
	for (auto iter : slice) {
		auto& timer = slice.get<Timer>(iter);
		if (registry.all_of<Status>(iter) && registry.get<Status>(iter).active == false) continue; // Ignore disabled timers
		timer.counter -= dTicks;
		if (timer.counter <= 0) {
			timer.event();
			if (registry.all_of<Status>(iter)) registry.get<Status>(iter).active = false;
		}
	}
}
