#include "playerSystem.hpp"

std::shared_ptr<Player> PlayerSystem::addPlayer() {
	players.push_back(std::make_shared<Player>());
	return players.back();
}

void PlayerSystem::changePlayer(entt::registry& registry) {
	if (players.size() <= 0) return; // This is probably not a situation entered into intentionally
	players[currentPlayerNum]->setInactive(registry);
	currentPlayerNum = (currentPlayerNum + 1) % players.size();
	players[currentPlayerNum]->setActive(registry);
}

void PlayerSystem::setActivePlayer(unsigned int playerNum, entt::registry& registry) {
	assert(playerNum < players.size());

	for (unsigned int i = 0; i < players.size(); i++) {
		if (i == playerNum) players[i]->setActive(registry);
		else players[i]->setInactive(registry);
	}
}

std::shared_ptr<Player> PlayerSystem::getActivePlayer() {
	if (players.size() < 1) return nullptr;
	// Else
	return players[currentPlayerNum];
}

void PlayerSystem::deletePlayers() {
	currentPlayerNum = 0;
	players.clear();
}
