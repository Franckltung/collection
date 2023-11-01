#include "JSONHandler.h"

JSONHandler::JSONHandler(logger& log) {
	debug = &log;
}

std::string JSONHandler::craftLookup(const std::string& JSONResponse) {
	rapidjson::Document d;
	d.Parse(JSONResponse.c_str());
	
	// Read all result keys into a string
	std::string converted;

	rapidjson::Value& result = d["result"];
	if (result.Size() == 0) {
		return "No Result";
	}
	// Only search up to 10 results
	if (result.Size() > 10) {
		for (unsigned int i = 0; i < 10; i++) {
			converted += result[i].GetString();
			converted += ","; // Comma between each key
		}
	}
	else {
		for (unsigned int i = 0; i < result.Size(); i++) {
			converted += result[i].GetString();
			converted += ","; // Comma between each key
		}
	}
	converted.pop_back(); // Remove last comma
	// Add query id
	converted += "?query=";
	converted += d["id"].GetString();

	debug->print("Created lookup string: " + converted);

	return converted;
}

std::string JSONHandler::craftRequest(std::string name, std::string type, unsigned int corrupted) {
	// Basic query - searches with min price of 1c, might not create accurate price
	std::string baseMessage = R"({"query":{"status":{"option":"online"},"name":"The Beast Fur Shawl","type":"Vaal Regalia","stats":[{"type":"and","filters":[]}],"filters":{"misc_filters":{"disabled":false,"filters":{"corrupted":{"option":"false"}}},"trade_filters":{"filters":{"price":{"min":1}}}}},"sort":{"price":"asc"}})";
	rapidjson::StringBuffer buffer;

	rapidjson::Document doc;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Parse(baseMessage.c_str());

	// Set name and type
	// Delete member if specified blank
	if (name == "") {
		doc["query"].RemoveMember("name");
	}
	else {
		doc["query"]["name"].SetString(name.c_str(), doc.GetAllocator());
	}
	if (type == "") {
		doc["query"].RemoveMember("type");
	}
	else {
		doc["query"]["type"].SetString(type.c_str(), doc.GetAllocator());
	}

	if (corrupted == 1) {
		std::string temp = "true";
		doc["query"]["filters"]["misc_filters"]["filters"]["corrupted"]["option"].SetString(temp.c_str(), doc.GetAllocator());
	}

	doc.Accept(writer);

	debug->print("Created request:" + std::string(buffer.GetString()));

	return buffer.GetString();
}

std::vector<CardItem> JSONHandler::loadCardItems(std::string dir) {	
	std::ifstream reader;
	std::string line;
	std::string doc;
	std::vector<CardItem> CardItems{};

	reader.open(dir);
	if (!reader) {
		debug->print("Error; failed to open CardItems file");
		std::cerr << "Error; failed to open CardItems file\n";
	}
	else {
		while (std::getline(reader, line)) {
			doc += line;
		}

		rapidjson::Document d;
		d.Parse(doc.c_str());

		rapidjson::Value& CardItemPairs = d["CardItemPairs"];
		
		for (unsigned int i = 0; i < CardItemPairs.Size(); i++) {
			CardItem item;
			item.cardName = CardItemPairs[i]["Card"].GetString();
			item.itemName = CardItemPairs[i]["Item"].GetString();
			item.cardNum = CardItemPairs[i]["Stack"].GetUint();
			item.cardCorrupted = CardItemPairs[i]["Corrupted"].GetUint();
			item.itemType = CardItemPairs[i]["ItemType"].GetString();
			
			CardItems.push_back(item);
		}

	}

	return CardItems;
}

std::vector<Card> JSONHandler::readCards(const std::string& json, unsigned int chaosToExa) {
	std::vector<Card> cards;

	rapidjson::Document d;
	d.Parse(json.c_str());

	rapidjson::Value& result = d["result"];

	for (unsigned int i = 0; i < result.Size(); i++) {
		// Check if result is not null
		if (result[i].IsNull()) {
			debug->print("Note; a card was skipped because it was null.");
			std::cout << "Note; a card was skipped because it was null.\n";
			continue;
		}

		// Ignore cards with price not listed in chaos or exalt
		if ((std::string(result[i]["listing"]["price"]["currency"].GetString()) != "exa") && (std::string(result[i]["listing"]["price"]["currency"].GetString()) != "chaos")) {
			continue; // Ignore card
		}
		Card card;
		card.name = result[i]["item"]["name"].GetString();
		if (std::string(result[i]["listing"]["price"]["currency"].GetString()) == "exa") {
			card.priceC = result[i]["listing"]["price"]["amount"].GetDouble()*chaosToExa; // Convert exa to chaos
		}
		else {
			card.priceC = result[i]["listing"]["price"]["amount"].GetDouble();
		}
		cards.push_back(card);
	}

	if (cards.size() == 0) {
		std::cout << "Error; no cards were read, probably not intended!\n";
	}

	return cards;
}

std::vector<Item> JSONHandler::readItems(const std::string& json, unsigned int chaosToExa) {
	std::vector<Item> items;

	rapidjson::Document d;
	d.Parse(json.c_str());

	rapidjson::Value& result = d["result"];

	for (unsigned int i = 0; i < result.Size(); i++) {
		// Check if result is not null
		if (result[i].IsNull()) {
			debug->print("Note; An item was skipped because it was null.");
			std::cout << "Note; An item was skipped because it was null.\n";
			continue;
		}
		// Ignore items with price not listed in chaos or exalt
		if ((std::string(result[i]["listing"]["price"]["currency"].GetString()) != "exa") && std::string(result[i]["listing"]["price"]["currency"].GetString()) != "chaos") {
			continue; // Ignore item
		}
		Item item;
		item.name = result[i]["item"]["name"].GetString();
		if (std::string(result[i]["listing"]["price"]["currency"].GetString()) == "exa") {
			item.priceC = result[i]["listing"]["price"]["amount"].GetDouble()*chaosToExa; // Convert exa to chaos
		}
		else {
			item.priceC = result[i]["listing"]["price"]["amount"].GetDouble();
		}
		items.push_back(item);
	}

	if (items.size() == 0) {
		debug->print("Warning; no items were read, probably not intended!");
		std::cout << "Warning; no items were read, probably not intended!\n";
	}

	if (items.size() < 1) {
		debug->print("Warning; returned item array from json with 0 items.");
		std::cout << "Warning; returned item array from json with 0 items.\n";
	}

	return items;
}