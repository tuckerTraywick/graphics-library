#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/XShm.h>
#include "graphics.h"

#define ABS(x) (((x) < 0.0f) ? (-x) : (x))

#define ROUND(x) (((x) < 0.0f) ? (int32_t)(x - 0.5f) : (int32_t)(x + 0.5f))

#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))

struct surface {
	struct vector2 size;
	struct vector2 parent_size;
	struct vector2 offset;
};

struct window {
	bool is_open;
	Display *x_display;
	Window x_window;
	GC x_context;
	// XftDraw *xft_draw;
	// XShmSegmentInfo x_shm_info;
	// XImage *x_image;
	Atom x_delete_window;
};

struct window *window_create(char *name, struct vector2 position, struct vector2 size) {
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}
	*window = (struct window){
		.is_open = true,
	};
	return window;
}

void window_destroy(struct window *window) {
	free(window);
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

#undef ABS
#undef ROUND
#undef MIN
#undef MAX
