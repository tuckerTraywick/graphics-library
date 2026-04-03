#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>
#include "graphics.h"

struct backend_window;

struct backend_window *backend_window_create(char *name, struct vector2 position, struct vector2 size);

void backend_window_destroy(struct backend_window *window);

struct surface *backend_window_get_surface(struct backend_window *window);

struct vector2 backend_window_get_mouse_position(struct backend_window *window);

// Returns true if the window is still open.
bool backend_window_update(struct backend_window *window);

#endif // BACKEND_H
