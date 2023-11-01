#include <iomanip>
#include "fileIO.h"

void fileIO::writeToFile(const std::vector<std::string>& data, std::string fileName, bool silent, bool cont) {
	std::ofstream fileO;
	if (cont) {
		fileO.open(fileName, std::ios::app);
	}
	else {
		fileO.open(fileName);
	}
	if (!fileO) {
		std::cerr << "Failed to open " << std::quoted(fileName) << " for writing.\n";
	}
	else {
		for (unsigned int i = 0; i < data.size(); i++) {
			fileO << data.at(i) << "\n";
		}

		// Only show if requested
		if (!silent) {
			std::cout << "Wrote file " << std::quoted(fileName) << "(" << data.size() << ").\n";
		}
	}
	fileO.close();
}

void fileIO::readFromFile(std::vector<std::string>& data, std::string fileName, bool silent) {
	std::ifstream fileI;
	fileI.open(fileName);
	if (!fileI) {
		std::cerr << "Failed to open " << std::quoted(fileName) << " for reading.\n";
	}
	else {
		unsigned int count = 0;
		std::string line;
		while (std::getline(fileI, line)) {
			count++;
			data.push_back(line);
		}
		if (!silent) std::cout << "Read file " << std::quoted(fileName) << "(" << count << ").\n";
	}
	fileI.close();
}
