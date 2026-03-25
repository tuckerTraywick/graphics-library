#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>

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

void window_fill(struct window *window, pixel color);

void window_draw_pixel(struct window *window, struct vector2 position, pixel color);

#endif // GRAPHICS_H
