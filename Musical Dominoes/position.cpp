#include "position.hpp"

std::ostream& operator<<(std::ostream& os, const Position& position) {
	os << "(" << position.x << ", " << position.y << ")";
	return os;
}

Position Position::operator+(const Position& right) {
	return {x + right.x, y + right.y};
}
