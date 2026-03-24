#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#define vec2(x, y) ((struct vector2){(x), (y)})

typedef uint32_t pixel;

struct vector2 {
	uint32_t x;
	uint32_t y;
};

struct window;

struct window *window_create(char *name, struct vector2 position, struct vector2 size);

void window_destroy(struct window *window);

bool window_is_open(struct window *window);

void window_update(struct window *window);

// // TODO: Make an unload font function?
// bool window_load_font(struct window *window, char *font_name);

// void window_fill(struct window *window, unsigned long color);

// void window_draw_point(struct window *window, unsigned long color, int x, int y);

// void window_draw_line(struct window *window, unsigned long color, unsigned int line_width, int x1, int y1, int x2, int y2);

// void window_draw_rectangle(struct window *window, unsigned long color, unsigned int line_width, int x, int y, unsigned int width, unsigned int height);

// void window_draw_rectangle_filled(struct window *window, unsigned long color, int x, int y, unsigned int width, unsigned int height);

// // TODO: Make a variant of this function that takes a length for `text` so it doesn't have to call
// // `strlen()`.
// // TODO: Add a color argument to this function.
// void window_draw_text(struct window *window, char *text, int x, int y);

#endif // GRAPHICS_H
