#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE 0xFF0000FF

// TODO: Make opaque.
struct window {
	char *name;
	unsigned int width;
	unsigned int height;
	// Xlib state.
	Display *x_display;
	Window x_window;
	GC x_context;
	XftDraw *xft_draw;
	XftFont *xft_font;
	XftColor xft_color;
};

struct window *window_create(char *name, unsigned int width, unsigned int height);

void window_destroy(struct window *window);

bool window_is_open(struct window *window);

void window_flush(struct window *window);

// TODO: Make an unload font function?
bool window_load_font(struct window *window, char *font_name);

void window_fill(struct window *window, unsigned long color);

void window_draw_line(struct window *window, unsigned long color, unsigned int line_width, int x1, int y1, int x2, int y2);

void window_draw_rectangle(struct window *window, unsigned long color, unsigned int line_width, int x, int y, unsigned int width, unsigned int height);

void window_draw_rectangle_filled(struct window *window, unsigned long color, int x, int y, unsigned int width, unsigned int height);

// TODO: Make a variant of this function that takes a length for `text` so it doesn't have to call
// `strlen()`.
// TODO: Add a color argument to this function.
void window_draw_text(struct window *window, char *text, int x, int y);

#endif // GRAPHICS_H
