#pragma once
// Handles reading and writing to files

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>

class fileIO
{
	public:
		// Reads and writes to files with each line as an entry in a vector
		// silent hides file info report and cont appends to end of file
		void writeToFile(const std::vector<std::string>& data, std::string fileName, bool silent, bool cont);
		void readFromFile(std::vector<std::string>& data, std::string fileName, bool silent = false);
};

