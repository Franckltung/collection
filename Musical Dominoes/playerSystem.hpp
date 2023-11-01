// 2023-Jul
#ifndef PLAYER_SYSTEM_HPP
#define PLAYER_SYSTEM_HPP

#include <memory>
#include "player.hpp"

class PlayerSystem
{
	public:
		std::shared_ptr<Player> addPlayer();
		std::shared_ptr<Player> getPlayer(unsigned int playerNum) { return players[playerNum]; }

		void changePlayer(entt::registry& registry); // Set active player to next player
		void setActivePlayer(unsigned int playerNum, entt::registry& registry);
		std::shared_ptr<Player> getActivePlayer();
		unsigned int getActivePlayerID() { return currentPlayerNum; }

		void deletePlayers(); // Remove all players, so we can start a new game probably
	private:
		std::vector<std::shared_ptr<Player>> players;
		int currentPlayerNum = 0;
};

#endif
