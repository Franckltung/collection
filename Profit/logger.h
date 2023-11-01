#pragma once
// Prints debug messages if enabled

#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include "fileIO.h"

class logger
{
public:
	logger(bool debugEnabled);

	void print(const std::string& message);

private:
	fileIO* writer;
	bool enabled;

	std::stringstream temp; // Temp holder for message
};
