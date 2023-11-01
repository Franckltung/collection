#include "logger.h"


logger::logger(bool debugEnabled)
{
	enabled = debugEnabled;
	if (enabled) {
		std::string msg = "DEBUG ENABLED";
		print(msg);
	}
}

void logger::print(const std::string& message) {
	if (enabled == true) {
		// Get timestamp
		auto cur = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(cur);

		temp.str(""); // Clear
		temp << std::ctime(&time) << "DEBUG: " << message;
		std::cout << temp.str() << "\n";
		writer->writeToFile({ temp.str() }, "log.txt", true, true);
	}
}
