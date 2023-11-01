// Franck Limtung
// A simple renderer that I can hopefully reuse in multiple projects
#ifndef SIMPLE_RENDERER_HPP
#define SIMPLE_RENDERER_HPP

#include <string>
#include <cassert>
#include <vector>
#include <tuple>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

struct RenderPoint
{
	RenderPoint(int x, int y, short red, short green, short blue) : x(x), y(y), red(red), green(green), blue(blue) {}
	int x = 0;
	int y = 0;
	short red;
	short green;
	short blue;
};
struct RenderRect : public RenderPoint
{
	RenderRect(int x, int y, int width, int height, bool fill, short red, short green, short blue) : RenderPoint(x, y, red, green, blue), width(width), height(height), fill(fill) {}
	int width;
	int height;
	bool fill;
};
struct RenderCircle : public RenderPoint
{
	RenderCircle(int x, int y, int radius, bool fill, short red, short green, short blue) : RenderPoint(x, y, red, green, blue), radius(radius), fill(fill) {}
	int radius;
	bool fill;
};
struct RenderImage
{
	RenderImage(std::string filename, double angle) : filename(filename), angle(angle) {}
	std::string filename;
	double angle;
};

class SimpleRenderer
{
	public:
		SimpleRenderer(std::string windowTitle, int windowWidth, int windowHeight, float windowZoom) :
			windowTitle(windowTitle), windowWidth(windowWidth), windowHeight(windowHeight), windowZoom(windowZoom) {
			// Setup SDL
			assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
			assert(SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"));
			window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
			assert(window != NULL);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			assert(renderer != NULL);

			assert(SDL_RenderSetIntegerScale(renderer, SDL_TRUE) >= 0);
			SDL_SetRenderDrawColor(renderer, std::get<0>(backgroundColor), std::get<1>(backgroundColor), std::get<2>(backgroundColor), 0xFF); // Set to BG color
			SDL_RenderSetScale(renderer, windowZoom, windowZoom);
		}
		~SimpleRenderer() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			renderer = NULL;
			window = NULL;
			SDL_Quit();
		}

		void drawPoints(const std::vector<RenderPoint>& points);
		void drawRects(const std::vector<RenderRect>& rects);
		void drawCircles(const std::vector<RenderCircle>& circles);
		// void drawImages(const std::vector<RenderImage>& images); TODO

		void renderClear() { SDL_RenderClear(renderer); }
		void renderPresent() { SDL_RenderPresent(renderer); }

		SDL_Renderer* getRenderer() { return renderer; }
		SDL_Window* getWindow() { return window; }

		void setZoom(float windowZoom) { SDL_RenderSetScale(renderer, windowZoom, windowZoom); }
	private:
		SDL_Renderer* renderer;
		SDL_Window* window;
		std::string windowTitle;
		int windowWidth;
		int windowHeight;
		float windowZoom;
		int shapeBorderWidth = 6; // Thickness of edge of shape without fill
		std::tuple<int, int, int> backgroundColor{0, 0, 0};
};

#endif
