// 2023-Jul
// Timer class built to trigger something after a certain amount of time
#ifndef TIMER_HPP
#define TIMER_HPP

#include <functional>
#include "constants.hpp"

struct Timer
{
	// Duration in ticks
	Timer(int duration, std::function<void()> event) : counter(duration), event(event) {}

	int counter = 0;
	std::function<void()> event; // Event to trigger when timer runs out
};

#endif
