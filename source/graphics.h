#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <X11/Xlib.h>

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF

struct window {
	char *name;
	unsigned int width;
	unsigned int height;
	// Xlib state.
	Display *x_display;
	Window x_window;
	GC x_context;
};

struct window *window_create(char *name, unsigned int width, unsigned int height);

void window_destroy(struct window *window);

bool window_is_open(struct window *window);

void window_flush(struct window *window);

void window_fill(struct window *window, unsigned long color);

void window_draw_line(struct window *window, unsigned long color, unsigned int line_width, int x1, int y1, int x2, int y2);

#endif // GRAPHICS_H
