#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

#include <stdbool.h>
#include <stdint.h>
#include "graphics.h"

struct backend_window;

// Return value must be destroyed with `backend_window_destroy()` after use.
struct backend_window *backend_window_create(char *name, struct vector2 position, struct vector2 size);

void backend_window_destroy(struct backend_window *window);

struct vector2 backend_window_get_resolution(struct backend_window *window);

struct surface backend_window_get_surface(struct backend_window *window);

// Returns true if the window is still open.
bool backend_window_update(struct backend_window *window);

#endif // GRAPHICS_BACKEND_H
