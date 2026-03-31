#include <stdio.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics.h"

#define abs(x) (((x) < 0) ? (-x) : (x))

#define round(x) (((x) < 0.0f) ? (int32_t)(x - 0.5f) : (int32_t)(x + 0.5f))

#define min(a, b) (((a) <= (b)) ? (a) : (b))

#define max(a, b) (((a) >= (b)) ? (a) : (b))

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

struct surface surface_create_with_pixels(pixel *pixels, struct vector2 size) {
	return (struct surface){
		.size = size,
		.pixels = pixels,
	};
}

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

void surface_set_pixel(struct surface *surface, struct vector2 position, pixel color) {
	if (position.x >= surface->size.x || position.y >= surface->size.y) {
		return;
	}
	if (surface->parent_size.x) {
		surface->pixels[(position.y + surface->offset.y)*surface->parent_size.x + position.x + surface->offset.x] = color;
		return;
	}
	surface->pixels[position.y*surface->size.x + position.x] = color;
}

bool surface_is_valid(struct surface *surface) {
	return surface->pixels != NULL;
}

void surface_fill(struct surface *surface, pixel color) {
	for (int32_t y = 0; y < surface->size.y; ++y) {
		for (int32_t x = 0; x < surface->size.x; ++x) {
			surface_set_pixel(surface, vec2(x, y), color);
		}
	}
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
		surface_set_pixel(surface, vec2(round(x), round(y)), color);
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
			surface_set_pixel(surface, vec2(rotated_surface_x, rotated_surface_y), surface_get_pixel(sprite, vec2(sprite_x, sprite_y)));
		}
	}
}

void surface_draw_surface_centered2(struct surface *surface, struct surface *sprite, struct vector2 position, struct vector2 size, float angle) {
	angle *= M_PI/180.0f;
	float scale_x = (float)sprite->size.x/(float)size.x;
	float scale_y = (float)sprite->size.y/(float)size.y;
	float sin_angle = sinf(angle);
	float cos_angle = cosf(angle);
	int32_t end_x = min(position.x + size.x/2, surface->size.x);
	int32_t end_y = min(position.y + size.y/2, surface->size.y);
	for (int32_t surface_y = position.y - size.y/2; surface_y < end_y; ++surface_y) {
		for (int32_t surface_x = position.x - size.x/2; surface_x < end_x; ++surface_x) {
			int32_t sprite_x = scale_x*(float)(surface_x - position.x + size.x/2);
			int32_t sprite_y = scale_y*(float)(surface_y - position.y + size.y/2);
			float difference_x = surface_x - position.x;
			float difference_y = surface_y - position.y;
			int32_t rotated_surface_x = cos_angle*difference_x - sin_angle*difference_y + position.x;
			int32_t rotated_surface_y = sin_angle*difference_x + cos_angle*difference_y + position.y;
			surface_set_pixel(surface, vec2(rotated_surface_x, rotated_surface_y), surface_get_pixel(sprite, vec2(sprite_x, sprite_y)));
		}
	}
}

struct window window_create(char *name, struct vector2 position, struct vector2 size) {
	struct window window = {
		.name = name,
		.size = size,
		.is_open = true,
	};

	// Setup the X window.
	window.x_display = XOpenDisplay(NULL);
	window.x_window = XCreateSimpleWindow(window.x_display, XDefaultRootWindow(window.x_display), (int)position.x, (int)position.y, size.x, size.y, 0, 0, 0);
	XMapWindow(window.x_display, window.x_window);
	int default_screen = DefaultScreen(window.x_display);
	int default_depth = DefaultDepth(window.x_display, default_screen);
	Visual *default_visual = DefaultVisual(window.x_display, default_screen);
	window.resolution = vec2(DisplayWidth(window.x_display, default_screen), DisplayHeight(window.x_display, default_screen));

	// Allocate the framebuffer.
	window.frame_buffer = calloc(window.resolution.x*window.resolution.y, sizeof *window.frame_buffer);
	if (!window.frame_buffer) {
		goto error1;
	}

	// Setup the XImage.
	window.x_image = XCreateImage(
		window.x_display, default_visual, default_depth, ZPixmap, 0, (char*)window.frame_buffer, window.resolution.x, window.resolution.y, 32, 0
	);
	if (!window.x_image) {
		goto error2;
	}

	// Setup the X graphics context.
	XGCValues values = {
		.foreground = WhitePixel(window.x_display, default_screen),
		.background = BlackPixel(window.x_display, default_screen),
		.line_width = 1,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	window.x_context = XCreateGC(window.x_display, window.x_window, mask, &values);
	window.xft_draw = XftDrawCreate(window.x_display, window.x_window, DefaultVisual(window.x_display, default_screen), DefaultColormap(window.x_display, default_screen));

	// Make it so we can detect the window being closed.
	window.x_delete_window = XInternAtom(window.x_display, "WM_DELETE_WINDOW", False);
	if (!XSetWMProtocols(window.x_display, window.x_window, &window.x_delete_window, 1)) {
		goto error3;
	}

	return window;

error3:
	XDestroyImage(window.x_image);
	XftDrawDestroy(window.xft_draw);
	XFreeGC(window.x_display, window.x_context);
error2:
	free(window.frame_buffer);
error1:
	XDestroyWindow(window.x_display, window.x_window);
	XCloseDisplay(window.x_display);
	return (struct window){0};
}

void window_destroy(struct window *window) {
	XDestroyImage(window->x_image);
	XftDrawDestroy(window->xft_draw);
	XFreeGC(window->x_display, window->x_context);
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	// free(window->frame_buffer); // Not freeing the frame buffer because `XDestroyImage()` frees it.
}

struct surface window_get_surface(struct window *window) {
	return (struct surface){
		.size = window->size,
		.parent_size = window->resolution,
		.pixels = window->frame_buffer,
	};
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

void window_update(struct window *window) {
	// Display the framebuffer.
	XPutImage(window->x_display, window->x_window, window->x_context, window->x_image, 0, 0, 0, 0, window->size.x, window->size.y);
	XFlush(window->x_display);

	// Check for events.
	if (!XPending(window->x_display)) {
		return;
	}
	XEvent event = {0};
	XNextEvent(window->x_display, &event); // Blocks.
	if (event.type == ClientMessage) {
		window->is_open = false;
	}
}

#undef abs
#undef round
#undef min
#undef max
