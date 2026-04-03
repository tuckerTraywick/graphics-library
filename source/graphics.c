#include <stdio.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics.h"
#include "backend.h"

#define abs(x) (((x) < 0.0f) ? (-x) : (x))

#define round(x) (((x) < 0.0f) ? (int32_t)(x - 0.5f) : (int32_t)(x + 0.5f))

#define min(a, b) (((a) <= (b)) ? (a) : (b))

#define max(a, b) (((a) >= (b)) ? (a) : (b))

struct window {
	char *name;
	bool is_open;
	struct backend_window *backend_window;
};

struct surface surface_create(struct vector2 size) {
	struct surface surface = {
		.size = size,
		.pixels = calloc(size.x*size.y, sizeof *surface.pixels),
	};
	if (!surface.pixels) {
		return (struct surface){0};
	}
	return surface;
}

// struct surface surface_create_with_pixels(pixel *pixels, struct vector2 size) {
// 	return (struct surface){
// 		.size = size,
// 		.pixels = pixels,
// 	};
// }

void surface_destroy(struct surface *surface) {
	free(surface->pixels);
	*surface = (struct surface){0};
}

struct surface surface_get_subsurface(struct surface *surface, struct vector2 offset, struct vector2 size) {
	return (struct surface){
		.size = size,
		.parent_size = surface->size,
		.offset = offset,
		.pixels = surface->pixels,
	};
}

pixel surface_get_pixel(struct surface *surface, struct vector2 position) {
	if (surface->parent_size.x) {
		return surface->pixels[(position.y + surface->offset.y)*surface->parent_size.x + position.x + surface->offset.x];
	}
	return surface->pixels[position.y*surface->size.x + position.x];
}

bool surface_is_valid(struct surface *surface) {
	return surface->pixels != NULL;
}

void surface_fill(struct surface *surface, pixel color) {
	for (int32_t y = 0; y < surface->size.y; ++y) {
		for (int32_t x = 0; x < surface->size.x; ++x) {
			surface_draw_pixel(surface, vec2(x, y), color);
		}
	}
}

bool surface_draw_pixel(struct surface *surface, struct vector2 position, pixel color) {
	if (position.x < 0 || position.x >= surface->size.x || position.y < 0 || position.y >= surface->size.y) {
		return false;
	}
	if (surface->parent_size.x) {
		surface->pixels[(position.y + surface->offset.y)*surface->parent_size.x + position.x + surface->offset.x] = color;
	} else {
		surface->pixels[position.y*surface->size.x + position.x] = color;
	}
	return true;
}

// TODO: Account for line thickness.
void surface_draw_line2(struct surface *surface, struct vector2 start, struct vector2 end, uint32_t thickness, pixel color) {
	struct vector2 distance = vec2(end.x - start.x, end.y - start.y);
	uint32_t steps = max(abs(distance.x), abs(distance.y));
	float x_increment = (float)distance.x/(float)steps;
	float y_increment = (float)distance.y/(float)steps;
	float x = start.x;
	float y = start.y;
	for (uint32_t i = 0; i <= steps; ++i) {
		if (!surface_draw_pixel(surface, vec2(x, y), color)) {
			return;
		}
		x += x_increment;
		y += y_increment;
	}
}

// TODO: Account for line thickness. Currently the corners will be choppy if the border is too
// thick.
void surface_draw_rectangle2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t thickness, pixel color) {
	struct vector2 top_right = vec2(position.x + size.x - 1, position.y);
	struct vector2 bottom_left = vec2(position.x, position.y + size.y - 1);
	struct vector2 bottom_right = vec2(position.x + size.x - 1, position.y + size.y - 1);
	surface_draw_line2(surface, position, top_right, thickness, color);
	surface_draw_line2(surface, position, bottom_left, thickness, color);
	surface_draw_line2(surface, bottom_left, bottom_right, thickness, color);
	surface_draw_line2(surface, bottom_right, top_right, thickness, color);
}

void surface_draw_rectangle_centered2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t thickness, pixel color) {
	int32_t x = position.x - size.x/2;
	int32_t y = position.y - size.y/2;
	surface_draw_rectangle2(surface, vec2(x, y), size, thickness, color);
}

void surface_draw_rectangle_filled2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t border_thickness, pixel border_color, pixel fill_color) {
	surface_draw_rectangle2(surface, position, size, border_thickness, border_color);
	for (int32_t y = position.y + border_thickness; y < position.y + size.y - border_thickness; ++y) {
		surface_draw_line2(surface, vec2(position.x + border_thickness, y), vec2(position.x + size.x - border_thickness, y), 1, fill_color);
	}
}

void surface_draw_rectangle_filled_centered2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t border_thickness, pixel border_color, pixel fill_color) {
	int32_t x = position.x - size.x/2;
	int32_t y = position.y - size.y/2;
	surface_draw_rectangle_filled2(surface, vec2(x, y), size, border_thickness, border_color, fill_color);
}

void surface_draw_surface2(struct surface *surface, struct surface *sprite, struct vector2 position, struct vector2 size, float angle) {
	angle *= M_PI/180.0f;
	float scale_x = (float)sprite->size.x/(float)size.x;
	float scale_y = (float)sprite->size.y/(float)size.y;
	float sin_angle = sinf(angle);
	float cos_angle = cosf(angle);
	int32_t end_x = min(position.x + size.x, surface->size.x);
	int32_t end_y = min(position.y + size.y, surface->size.y);
	for (int32_t surface_y = position.y; surface_y < end_y; ++surface_y) {
		for (int32_t surface_x = position.x; surface_x < end_x; ++surface_x) {
			float difference_x = surface_x - position.x;
			float difference_y = surface_y - position.y;
			int32_t sprite_x = scale_x*difference_x;
			int32_t sprite_y = scale_y*difference_y;
			int32_t rotated_surface_x = cos_angle*difference_x - sin_angle*difference_y + position.x;
			int32_t rotated_surface_y = sin_angle*difference_x + cos_angle*difference_y + position.y;
			surface_draw_pixel(surface, vec2(rotated_surface_x, rotated_surface_y), surface_get_pixel(sprite, vec2(sprite_x, sprite_y)));
		}
	}
}

void surface_draw_surface_centered2(struct surface *surface, struct surface *sprite, struct vector2 position, struct vector2 size, float angle) {
	angle *= M_PI/180.0f;
	float sin_angle = sinf(angle);
	float cos_angle = cosf(angle);
	float sin_minus_angle = sinf(-angle);
	float cos_minus_angle = cosf(-angle);
	float scale_x = (float)sprite->size.x/(float)size.x;
	float scale_y = (float)sprite->size.y/(float)size.y;
	// `*1.1f` to account for rounding errors cutting off edges and corners.
	float size_x = round((float)size.x*abs(cos_angle) + (float)size.y*abs(sin_angle))*1.1f;
	float size_y = round((float)size.y*abs(sin_angle) + (float)size.y*abs(cos_angle))*1.1f;
	struct vector2 rotated_size = {
		.x = size_x,
		.y = size_y,
	};
	float half_size_x = (size_x*0.5);
	float half_size_y = (size_y*0.5);
	int32_t start_x = position.x - half_size_x;
	int32_t start_y = position.y - half_size_y;
	int32_t end_x = position.x + half_size_x;
	int32_t end_y = position.y + half_size_y;
	// surface_draw_rectangle_centered2(surface, position, rotated_size, 1, COLOR_WHITE);
	for (int32_t surface_y = start_y; surface_y < end_y; ++surface_y) {
		for (int32_t surface_x = start_x; surface_x < end_x; ++surface_x) {
			float difference_x = surface_x - position.x;
			float difference_y = surface_y - position.y;
			int32_t rotated_surface_x = cos_minus_angle*difference_x - sin_minus_angle*difference_y;
			int32_t rotated_surface_y = sin_minus_angle*difference_x + cos_minus_angle*difference_y;
			int32_t sprite_x = scale_x*(float)(rotated_surface_x + size.x/2);
			int32_t sprite_y = scale_y*(float)(rotated_surface_y + size.y/2);
			if (sprite_x >= 0 && sprite_x < sprite->size.x && sprite_y >= 0 && sprite_y < sprite->size.y)
				surface_draw_pixel(surface, vec2(surface_x, surface_y), surface_get_pixel(sprite, vec2(sprite_x, sprite_y)));
		}
	}
}

struct window *window_create(char *name, struct vector2 position, struct vector2 size) {
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}
	*window = (struct window){
		.name = name,
		.is_open = true,
		.backend_window = backend_window_create(name, position, size),
	};
	if (!window->backend_window) {
		free(window);
		return NULL;
	}
	return window;
}

void window_destroy(struct window *window) {
	backend_window_destroy(window->backend_window);
	free(window);
}

struct surface *window_get_surface(struct window *window) {
	return backend_window_get_surface(window->backend_window);
}

struct vector2 window_get_mouse_position(struct window *window) {
	return backend_window_get_mouse_position(window->backend_window);
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

void window_update(struct window *window) {
	window->is_open = backend_window_update(window->backend_window);
}

#undef abs
#undef round
#undef min
#undef max
