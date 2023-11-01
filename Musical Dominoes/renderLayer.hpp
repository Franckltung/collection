#ifndef RENDER_LAYER_HPP
#define RENDER_LAYER_HPP

enum class RENDER_LAYER
{
	BG_LAYER = 0, // Background
	WORLD_LAYER, // Placed objects
	GRAB_LAYER, // Picked-up objects
	GUI_BG_LAYER,
	GUI_LAYER, // GUI
	GUI_TEXT_LAYER,
	GUI_DIALOG_LAYER, // GUI dialog windows
	GUI_DIALOG_TEXT_LAYER,
	count
};

struct RenderLayer
{
	RenderLayer(RENDER_LAYER layer) : layer(layer) {}
	RENDER_LAYER layer = RENDER_LAYER::WORLD_LAYER;
};
#endif
