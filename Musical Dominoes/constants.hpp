#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

namespace Constants
{
	constexpr int WINDOW_WIDTH = 1152;
	constexpr int WINDOW_HEIGHT = 648;
	constexpr int WINDOW_WIDTH_ZOOM = WINDOW_WIDTH / 0.88;
	constexpr int WINDOW_HEIGHT_ZOOM = WINDOW_HEIGHT / 0.88;
	constexpr double TICK_TIME = 16.667;
	constexpr float SNAP_DISTANCE = 120; // Distance to snap to head and tail domino
	constexpr bool REPLAY_WITH_DOUBLE = true; // Does the player get to play again if the domino was a double
	constexpr int DOMINO_WIDTH = 100;
	constexpr int DOMINO_HEIGHT = 45;
	constexpr float VOLUME = 1;
};

#endif
