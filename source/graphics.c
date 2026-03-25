#include <stdio.h>

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics.h"

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

struct window *window_create(char *name, struct vector2 position, struct vector2 size) {
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}
	*window = (struct window){
		.name = name,
		.size = size,
		.is_open = true,
	};

	// Setup the X window.
	window->x_display = XOpenDisplay(NULL);
	window->x_window = XCreateSimpleWindow(window->x_display, XDefaultRootWindow(window->x_display), (int)position.x, (int)position.y, size.x, size.y, 0, 0, 0);
	XMapWindow(window->x_display, window->x_window);
	int default_screen = DefaultScreen(window->x_display);
	int default_depth = DefaultDepth(window->x_display, default_screen);
	Visual *default_visual = DefaultVisual(window->x_display, default_screen);
	window->resolution = vec2(DisplayWidth(window->x_display, default_screen), DisplayHeight(window->x_display, default_screen));

	// Allocate the framebuffer.
	window->frame_buffer = calloc(window->resolution.x*window->resolution.y, sizeof *window->frame_buffer);
	if (!window->frame_buffer) {
		goto error1;
	}

	// Setup the XImage.
	window->x_image = XCreateImage(
		window->x_display, default_visual, default_depth, ZPixmap, 0, (char*)window->frame_buffer, window->resolution.x, window->resolution.y, 32, 0
	);
	if (!window->x_image) {
		goto error2;
	}

	// Setup the X graphics context.
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
	XDestroyImage(window->x_image);
	XftDrawDestroy(window->xft_draw);
	XFreeGC(window->x_display, window->x_context);
error2:
	free(window->frame_buffer);
error1:
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window);
	return NULL;
}

void window_destroy(struct window *window) {
	XDestroyImage(window->x_image);
	XftDrawDestroy(window->xft_draw);
	XFreeGC(window->x_display, window->x_context);
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	// free(window->frame_buffer); // Not freeing the frame buffer because `XDestroyImage()` frees it.
	free(window);
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

void window_update(struct window *window) {
	XPutImage(window->x_display, window->x_window, window->x_context, window->x_image, 0, 0, 0, 0, window->size.x, window->size.y);
	XFlush(window->x_display);
	XEvent event = {0};
	XNextEvent(window->x_display, &event);
	if (event.type == ClientMessage) {
		window->is_open = false;
	}
}

void window_fill(struct window *window, pixel color) {
	for (size_t i = 0; i < window->resolution.x*window->resolution.y; ++i) {
		window->frame_buffer[i] = color;
	}
}

void window_draw_pixel(struct window *window, struct vector2 position, pixel color) {
	window->frame_buffer[position.y*window->resolution.x + position.x] = color;
}
