#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>

struct Button
{
	std::function<void()> event;
};

#endif
