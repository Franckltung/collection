#ifndef GUI_ELEMENT_HPP
#define GUI_ELEMENT_HPP

enum class GUI_SCENE
{
	MENU,
	GAME
};

struct GuiElement
{
	GuiElement(GUI_SCENE scene, bool temporary = false) : type(scene), temporary(temporary) {}
	GUI_SCENE type;
	bool temporary; // Will it be deleted when it's unloaded
};

#endif
