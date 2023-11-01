#pragma once

#include <sstream>
#include <fstream>

#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "logger.h"
#include "CardItem.h"

// Handles the JSON data used in this program

const std::string fetchURL = "www.pathofexile.com/api/trade/fetch/";

class JSONHandler {
	public:
		JSONHandler(logger& log);

		std::vector<CardItem> loadCardItems(std::string dir);

		// Input a search result
		std::vector<Card> readCards(const std::string& json, unsigned int chaosToExa);
		std::vector<Item> readItems(const std::string& json, unsigned int chaosToExa);

		// Craft search request
		std::string craftRequest(std::string name, std::string type, unsigned int corrupted = 0);

		// Craft search lookup after sending request
		std::string craftLookup(const std::string& JSONResponse);
	private:
		logger* debug;
};
