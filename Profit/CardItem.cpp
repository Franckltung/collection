#include <cmath>
#include "CardItem.h"

// TODO: Sorting is not the fastest way to find the median
void CardItem::compress(std::vector<Card> cardType, std::vector<Item> itemType) {
	// Calculate median average price
	std::sort(cardType.begin(), cardType.end(), [](const Card& lhs, const Card& rhs) {
		return lhs.priceC < rhs.priceC;
	});
	std::sort(itemType.begin(), itemType.end(), [](const Item& lhs, const Item& rhs) {
		return lhs.priceC < rhs.priceC;
	});

	if (cardType.size() % 2 == 0)
	{
		cardPriceC = (cardType[cardType.size() / 2 - 1].priceC + cardType[cardType.size() / 2].priceC / 2);
	}
	else
	{
		cardPriceC = (cardType[cardType.size() / 2].priceC);
	}

	if (itemType.size() % 2 == 0)
	{
		itemPriceC = (itemType[itemType.size() / 2 - 1].priceC + itemType[itemType.size() / 2].priceC / 2);
	}
	else
	{
		itemPriceC = (itemType[itemType.size() / 2].priceC);
	}

	// Profit is per card
	std::cout << "Item name: " << itemName << " Item price: " << itemPriceC << " Card price: " << cardPriceC << "\n";
	profit = (itemPriceC - cardPriceC * cardNum)/cardNum;
}

// TODO: Write to report file
void Util::revealProfit(std::vector<CardItem> cardItems) {
	fileIO writer;
	std::stringstream stream;
	for (unsigned int i = 0; i < cardItems.size(); i++) {
		std::string itemName = cardItems[i].itemName;
		if (cardItems[i].cardCorrupted) {
			itemName += "(Corrupted)";
		}
		std::cout << std::fixed << std::left;
		std::cout << std::setw(123) << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
		std::cout << std::setw(30) << std::left << "Card Name:" << std::setw(1) << "|" << std::setw(60) << "Item Name:" << std::setw(1) << "|" << std::setw(30) << "Profit in chaos(Per card):" << std::endl;
		std::cout << std::setw(30) << std::left << cardItems[i].cardName << std::setw(1) << "|" << std::setw(60) << itemName << std::setw(1) << "|" << std::setw(30) << cardItems[i].profit << std::endl;

		stream << std::fixed << std::left;
		stream << std::setw(123) << "---------------------------------------------------------------------------------------------------------------------------" << std::endl;
		stream << std::setw(30) << std::left << "Card Name:" << std::setw(1) << "|" << std::setw(60) << "Item Name:" << std::setw(1) << "|" << std::setw(30) << "Profit in chaos(Per card):" << std::endl;
		stream << std::setw(30) << std::left << cardItems[i].cardName << std::setw(1) << "|" << std::setw(60) << itemName << std::setw(1) << "|" << std::setw(30) << cardItems[i].profit << std::endl;
	}
		writer.writeToFile({ stream.str() }, outputFile, true, false);
		std::cout << "Wrote table to file \"" << outputFile << "\".\n";
}
