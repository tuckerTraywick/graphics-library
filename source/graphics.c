#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <X11/Xlib.h>
// #include <X11/Xft/Xft.h>
// #include <X11/extensions/XShm.h>
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
	// XShmSegmentInfo x_shm_info;
	// XImage *x_image;
	Atom x_delete_window;
};

struct window *window_create(char *name, struct vector2 position, struct vector2 size) {
	// Allocate the window.
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}
	*window = (struct window){
		.is_open = true,
	};

	// Setup the X window.
	window->x_display = XOpenDisplay(NULL);
	window->x_window = XCreateSimpleWindow(window->x_display, XDefaultRootWindow(window->x_display), (int)position.x, (int)position.y, size.x, size.y, 0, 0, 0);
	XStoreName(window->x_display, window->x_window, name);
	XMapWindow(window->x_display, window->x_window);
	int default_screen = DefaultScreen(window->x_display);

	// Create the graphics context.
	XGCValues values = {
		.foreground = WhitePixel(window->x_display, default_screen),
		.background = BlackPixel(window->x_display, default_screen),
		.line_width = 1,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	window->x_context = XCreateGC(window->x_display, window->x_window, mask, &values);

	// Make it so we can detect the window being closed.
	window->x_delete_window = XInternAtom(window->x_display, "WM_DELETE_WINDOW", False);
	if (!XSetWMProtocols(window->x_display, window->x_window, &window->x_delete_window, 1)) {
		XFreeGC(window->x_display, window->x_context);
		XDestroyWindow(window->x_display, window->x_window);
		XCloseDisplay(window->x_display);
		free(window);
		return NULL;
	}
	return window;
}

void window_destroy(struct window *window) {
	XFreeGC(window->x_display, window->x_context);
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window);
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

void window_update(struct window *window) {
	// Display the content of the window.
	XFlush(window->x_display);
	// XSync(window->x_display, false);
	
	// Check for events.
	if (XPending(window->x_display)) {
		XEvent event = {0};
		XNextEvent(window->x_display, &event); // Blocks.
		window->is_open = (event.type != ClientMessage);
	}
}

#undef ABS
#undef ROUND
#undef MIN
#undef MAX
