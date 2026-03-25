#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#define vec2(x, y) ((struct vector2){(x), (y)})

#define COLOR_OPAQUE 0xFFFFFFFF

#define COLOR_TRANSPARENT 0x00FFFFFF

#define COLOR_WHITE 0xFFFFFFFF

#define COLOR_BLACK 0xFF000000

#define COLOR_RED 0xFFFF0000

#define COLOR_GREEN 0xFF00FF00

#define COLOR_BLUE 0xFF0000FF

typedef uint32_t pixel;

struct vector2 {
	int32_t x;
	int32_t y;
};

struct surface {
	struct vector2 size;
	struct vector2 parent_size; // Only applicable if this is a subsurface.
	struct vector2 offset; // Only applicable if this is a subsurface.
	pixel *pixels;
};

struct window {
	char *name;
	struct vector2 size;
	struct vector2 resolution;
	bool is_open;
	pixel *frame_buffer;

	// Xlib state.
	Display *x_display;
	Window x_window;
	GC x_context;
	XftDraw *xft_draw;
	XImage *x_image;
	Atom x_delete_window;
};

// You must call `surface_is_valid()` on the result of this function to make sure initialization was
// successful and `surface_destroy()` when you are done with it.
struct surface surface_create(struct vector2 size);

// Creates a new surface with the given pixels. Lets the user decide how to allocate pixels. You do
// NOT have to call `surface_is_valid()` or `surface_destroy()` on the result of this function.
struct surface surface_create_with_pixels(pixel *pixels, struct vector2 size);

// Only call this on surfaces created with `surface_create()`.
void surface_destroy(struct surface *surface);

struct surface surface_get_subsurface(struct surface *surface, struct vector2 offset, struct vector2 size);

pixel surface_get_pixel(struct surface *surface, struct vector2 position);

void surface_set_pixel(struct surface *surface, struct vector2 position, pixel color);

// Returns true if the given surface has been successfully initialized.
bool surface_is_valid(struct surface *surface);

void surface_fill(struct surface *surface, pixel color);

void surface_draw_line2(struct surface *surface, struct vector2 start, struct vector2 end, uint32_t thickness, pixel color);

void surface_draw_rectangle2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t thickness, pixel color);

void surface_draw_rectangle_filled2(struct surface *surface, struct vector2 position, struct vector2 size, uint32_t border_thickness, pixel border_color, pixel fill_color);

// Scales `sprite` to fit `size`.
void surface_draw_surface2(struct surface *surface, struct surface *sprite, struct vector2 position, struct vector2 size);

struct window window_create(char *name, struct vector2 position, struct vector2 size);

void window_destroy(struct window *window);

struct surface window_get_surface(struct window *window);

bool window_is_open(struct window *window);

void window_update(struct window *window);

#endif // GRAPHICS_H
