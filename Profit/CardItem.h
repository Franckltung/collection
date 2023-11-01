#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "fileIO.h"

const std::string outputFile = "result.txt";
struct Card {
	std::string name;
	double priceC; // Price in chaos
};

struct Item {
	std::string name;
	double priceC; // Price in chaos
};

// The final data object, with card stats, item stats, and profit-per-card
struct CardItem {
	// Compress a set of cards and items into a single cardItem, and calculate the profit
	void compress(std::vector<Card> cardType, std::vector<Item> itemType);

	std::string cardName;
	std::string itemName;
	std::string itemType;
	unsigned int cardNum;
	double cardPriceC; // Average
	double itemPriceC; // Average

	unsigned int cardCorrupted;

	double profit;
};

class Util
{
	public:
		void revealProfit(std::vector<CardItem> cardItems); // Print info
};
