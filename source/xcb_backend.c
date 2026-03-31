#include <stdint.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics_backend.h"
#include "graphics.h"

struct backend_window {
	struct surface surface;
};

struct backend_window *backend_window_create(char *name, struct vector2 position, struct vector2 size) {
	return NULL;
}

void backend_window_destroy(struct backend_window *window) {
	free(window);
}

struct vector2 backend_window_get_resolution(struct backend_window *window) {
	return vec2(0, 0);
}

struct surface backend_window_get_surface(struct backend_window *window) {
	return window->surface;
}

bool backend_window_update(struct backend_window *window) {
	return false;
}
