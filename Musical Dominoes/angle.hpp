// Franck Limtung
// 2023-Oct
#ifndef ANGLE_HPP
#define ANGLE_HPP

struct Angle
{
	void changeAngle(float value); // Modify the angle by a value and handle overflows in either direction
	float angle = 0.0;

	template<class Archive>
	void serialize(Archive& archive) {
		archive(angle);
	}
};

#endif
