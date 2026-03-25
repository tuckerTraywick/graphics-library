#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

#define vec2(x, y) ((struct vector2){(x), (y)})

#define COLOR_OPAQUE 0xFFFFFFFF

#define COLOR_TRANSPARENT 0x00FFFFFF

#define COLOR_WHITE 0xFFFFFFFF

#define COLOR_BLACK 0x00000000

#define COLOR_RED 0xFFFF0000

#define COLOR_GREEN 0xFF00FF00

#define COLOR_BLUE 0xFF0000FF

typedef uint32_t pixel;

struct vector2 {
	int32_t x;
	int32_t y;
};

struct window;

struct window *window_create(char *name, struct vector2 position, struct vector2 size);

void window_destroy(struct window *window);

bool window_is_open(struct window *window);

void window_update(struct window *window);

void window_fill(struct window *window, pixel color);

void window_draw_pixel2(struct window *window, struct vector2 position, pixel color);

void window_draw_line2(struct window *window, struct vector2 start, struct vector2 end, uint32_t thickness, pixel color);

void window_draw_rectangle2(struct window *window, struct vector2 position, struct vector2 size, uint32_t thickness, pixel color);

void window_draw_rectangle_filled2(struct window *window, struct vector2 position, struct vector2 size, uint32_t border_thickness, pixel border_color, pixel fill_color);

#endif // GRAPHICS_H
