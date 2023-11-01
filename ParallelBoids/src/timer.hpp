// Franck Limtung
// Simple timer class to measure execution time of things
#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <string>
#include <unordered_map>

struct Timer
{
	Timer() {
		start = std::chrono::high_resolution_clock::now();
		stop = start;
	}
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop;
	bool stopped = false; // Reading the timer will read stop - start if true, otherwise it will take the current time
};

class TimerSystem
{
	public:
		void startTimer(std::string name);
		void stopTimer(std::string name);
		void resetTimer(std::string name);
		int readTimer(std::string name); // Read in us
	private:
		std::unordered_map<std::string, Timer> timers;
		Timer& getTimer(std::string name);
};

#endif
