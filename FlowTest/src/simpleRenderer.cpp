#include "simpleRenderer.hpp"

void SimpleRenderer::drawPoints(const std::vector<RenderPoint>& points) {
	for (const RenderPoint& point : points) {
		SDL_SetRenderDrawColor(renderer, point.red, point.green, point.blue, 0xFF);
		SDL_RenderDrawPoint(renderer, point.x, point.y);
	}

	SDL_SetRenderDrawColor(renderer, std::get<0>(backgroundColor), std::get<1>(backgroundColor), std::get<2>(backgroundColor), 0xFF); // Set to BG color
}

void SimpleRenderer::drawRects(const std::vector<RenderRect>& rects) {
	for (const RenderRect& rect : rects) {
		SDL_SetRenderDrawColor(renderer, rect.red, rect.green, rect.blue, 0xFF);
		SDL_Rect rectSDL = {rect.x - rect.width / 2, rect.y - rect.height / 2, rect.width, rect.height};
		SDL_RenderFillRect(renderer, &rectSDL);
	}

	SDL_SetRenderDrawColor(renderer, std::get<0>(backgroundColor), std::get<1>(backgroundColor), std::get<2>(backgroundColor), 0xFF);
}

void SimpleRenderer::drawCircles(const std::vector<RenderCircle>& circles) {
	for (const RenderCircle& circle : circles) {
		SDL_SetRenderDrawColor(renderer, circle.red, circle.green, circle.blue, 0xFF);
		// Simple circle algorithm
		for (int w = 0; w < circle.radius * 2; w++) {
			for (int h = 0; h < circle.radius * 2; h++) {
				int dw = circle.radius - w;
				int dh = circle.radius - h;
				// Fill and not fill have different rules
				if (circle.fill && dw * dw + dh * dh <= circle.radius * circle.radius) {
					SDL_RenderDrawPoint(renderer, circle.x + dw, circle.y + dh);
				}
				else if (!circle.fill && std::sqrt(std::abs((dw * dw + dh * dh) - circle.radius * circle.radius)) < shapeBorderWidth) {
					SDL_RenderDrawPoint(renderer, circle.x + dw, circle.y + dh);
				}
			}
		}
	}

	SDL_SetRenderDrawColor(renderer, std::get<0>(backgroundColor), std::get<1>(backgroundColor), std::get<2>(backgroundColor), 0xFF); // Set to BG color
}
