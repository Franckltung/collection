// Franck Limtung 2019

#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <thread>
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "JSONHandler.h"
#include "network.h"
#include "logger.h"
#include "CardItem.h"

int main() {
	std::string inputS;
	std::string searchTarget = "/api/trade/search/"; // Website dir string
	std::string fetchTarget = "/api/trade/fetch/";
	unsigned int sleepTimeMs = 1000; // Time to sleep between searches

	unsigned int chaosToExa;

	while (true) {
		std::cout << "Which league: ";
		std::cin >> inputS;
		searchTarget += inputS;
		std::cout << "What is the current chaos : exalt ratio?: ";
		std::cin >> chaosToExa;

		try {
			logger debug(false);
			debug.print("STARTED PROGRAM");

			JSONHandler JSONUtil(debug);
			network networkHandler(debug);
			Util util;

			std::vector<Card> cards;
			std::vector<Item> items;

			std::vector<CardItem> cardItems = JSONUtil.loadCardItems("CardItems.json");
			for (unsigned int i = 0; i < cardItems.size(); i++) {
				std::cout << "Reading card " << std::quoted(cardItems[i].cardName) << ", linked with item " << std::quoted(cardItems[i].itemName) << ".\n";
				cards.clear();
				items.clear();
				// Query card
				std::string queryCard = JSONUtil.craftRequest("", cardItems[i].cardName);
				//std::cout << "Query: " << queryCard << "\n";

				std::string responseCard = networkHandler.send(queryCard, searchTarget, http::verb::post);
				//std::cout << "Response: " << responseCard << "\n";
				std::string convertedCard = JSONUtil.craftLookup(responseCard);
				//std::cout << "Converted: " << convertedCard << "\n";
				std::string resultsCard = networkHandler.send("", fetchTarget + convertedCard, http::verb::get);

				// Add cards
				cards = JSONUtil.readCards(resultsCard, chaosToExa);

				// Query Item
				std::string queryItem = JSONUtil.craftRequest(cardItems[i].itemName, cardItems[i].itemType, cardItems[i].cardCorrupted);
				std::string responseItem = networkHandler.send(queryItem, searchTarget, http::verb::post);
				std::string convertedItem = JSONUtil.craftLookup(responseItem);
				std::string resultsItem = networkHandler.send("", fetchTarget + convertedItem, http::verb::get);
				// Add items
				items = JSONUtil.readItems(resultsItem, chaosToExa);

				// Compress into cardItem
				cardItems[i].compress(cards, items);

				// Wait 3 seconds, so the poe server doesn't ban me
				std::cout << "Sleeping for " << sleepTimeMs << " milliseconds\n";

				std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMs));
			}

			// Sort from largest to smallest profit
			std::sort(cardItems.begin(), cardItems.end(), [](const CardItem& lhs, const CardItem& rhs) {
				return lhs.profit > rhs.profit;
			});

			util.revealProfit(cardItems); // Print info
		}
		catch (std::exception const& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return 1;
		}
		std::cin.clear();
		std::cin.ignore(10000, '\n');
		std::cout << "Press Enter To Continue...";
		std::cin.get();
	}
	return 0;
}
