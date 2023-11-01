#include "timer.hpp"

void TimerSystem::startTimer(std::string name) {
	Timer& timer = getTimer(name);
	if (!timer.stopped) return;
	timer.stopped = false;
	timer.start = std::chrono::high_resolution_clock::now() - (timer.stop - timer.start); // Offset clock by current stored duration
}

void TimerSystem::stopTimer(std::string name) {
	Timer& timer = getTimer(name);
	timer.stopped = true;
	timer.stop = std::chrono::high_resolution_clock::now();
}

void TimerSystem::resetTimer(std::string name) {
	Timer& timer = getTimer(name);
	timer.stopped = false;
	timer.start = std::chrono::high_resolution_clock::now();
}

int TimerSystem::readTimer(std::string name) {
	Timer& timer = getTimer(name);
	if (!timer.stopped) return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer.start).count();
	else return std::chrono::duration_cast<std::chrono::microseconds>(timer.stop - timer.start).count();
}

Timer& TimerSystem::getTimer(std::string name) {
	auto search = timers.find(name);
	if (search != timers.end()) {
		return search->second;
	}
	// Else
	return timers.insert({name, Timer()}).first->second;
}
