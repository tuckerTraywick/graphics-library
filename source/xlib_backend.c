#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "backend.h"
#include "graphics.h"

struct backend_window {
	struct surface surface;
	Display *x_display;
	Window x_window;
	GC x_context;
	XftDraw *xft_draw;
	XImage *x_image;
	Atom x_delete_window;
};

struct backend_window *backend_window_create(char *name, struct vector2 position, struct vector2 size) {
	// Create the window.
	struct backend_window *window = malloc(sizeof *window);
	if (!window) {
		goto error1;
	}
	window->x_display = XOpenDisplay(NULL);
	window->x_window = XCreateSimpleWindow(window->x_display, XDefaultRootWindow(window->x_display), (int)position.x, (int)position.y, size.x, size.y, 0, 0, 0);
	XStoreName(window->x_display, window->x_window, name);
	XMapWindow(window->x_display, window->x_window);
	int default_screen = DefaultScreen(window->x_display);
	int default_depth = DefaultDepth(window->x_display, default_screen);
	Visual *default_visual = DefaultVisual(window->x_display, default_screen);

	// Create the surface.
	window->surface = surface_create(vec2(DisplayWidth(window->x_display, default_screen), DisplayHeight(window->x_display, default_screen)));
	if (!surface_is_valid(&window->surface)) {
		goto error2;
	}
	window->surface.parent_size = window->surface.size;
	window->surface.size = size;

	// Create the image.
	window->x_image = XCreateImage(
		window->x_display, default_visual, default_depth, ZPixmap, 0, (char*)window->surface.pixels, window->surface.parent_size.x, window->surface.parent_size.y, 32, 0
	);

	// Create the graphics context.
	XGCValues values = {
		.foreground = WhitePixel(window->x_display, default_screen),
		.background = BlackPixel(window->x_display, default_screen),
		.line_width = 1,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	window->x_context = XCreateGC(window->x_display, window->x_window, mask, &values);
	window->xft_draw = XftDrawCreate(window->x_display, window->x_window, DefaultVisual(window->x_display, default_screen), DefaultColormap(window->x_display, default_screen));

	// Make it so we can detect the window being closed.
	window->x_delete_window = XInternAtom(window->x_display, "WM_DELETE_WINDOW", False);
	if (!XSetWMProtocols(window->x_display, window->x_window, &window->x_delete_window, 1)) {
		goto error3;
	}
	return window;

error3:
	XftDrawDestroy(window->xft_draw);
	XFreeGC(window->x_display, window->x_context);
	XDestroyImage(window->x_image);
	// surface_destroy(&window->surface);
error2:
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window);
error1:
	return NULL;
}

void backend_window_destroy(struct backend_window *window) {
	XftDrawDestroy(window->xft_draw);
	XFreeGC(window->x_display, window->x_context);
	XDestroyImage(window->x_image);
	// surface_destroy(&window->surface); // Don't destroy the surface because its pixels get freed by `XDestroyImage()`.
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window);
}

struct surface *backend_window_get_surface(struct backend_window *window) {
	return &window->surface;
}

bool backend_window_update(struct backend_window *window) {
	// Display the surface.
	XPutImage(window->x_display, window->x_window, window->x_context, window->x_image, 0, 0, 0, 0, window->surface.size.x, window->surface.size.y);
	XFlush(window->x_display);

	// Check for events.
	if (XPending(window->x_display)) {
		XEvent event = {0};
		XNextEvent(window->x_display, &event); // Blocks.
		return event.type != ClientMessage;
	}
	return true;
}
