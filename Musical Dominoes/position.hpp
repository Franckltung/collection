// 2022-Oct
#ifndef POSITION_HPP
#define POSITION_HPP

#include <ostream>

struct Position
{
	Position(float x, float y) : x(x), y(y) {}
	Position() : x(0), y(0) {}
	float x, y;

	friend std::ostream& operator<<(std::ostream& os, const Position& position);
	Position operator+(const Position& right);

	template<class Archive>
	void serialize(Archive& archive) {
		archive(x, y);
	}
};

std::ostream& operator<<(std::ostream& os, const Position& position);

#endif
