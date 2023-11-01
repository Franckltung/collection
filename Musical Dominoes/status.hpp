// Franck Limtung
// 2023-May
// Determines if the entity is enabled or disabled
#ifndef STATUS_HPP
#define STATUS_HPP

struct Status
{
	Status(bool active = false) : active(active) {}
	bool active;
};

#endif
