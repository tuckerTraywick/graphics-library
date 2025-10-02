#include <stdbool.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "graphics.h"

struct window *window_create(char *name, unsigned int width, unsigned int height) {
	struct window *window = malloc(sizeof *window);
	if (!window) {
		return NULL;
	}
	*window = (struct window){
		.name = name,
		.width = width,
		.height = height,
	};
	// Setup the X window.
	window->x_display = XOpenDisplay(NULL);
	window->x_window = XCreateSimpleWindow(window->x_display, XDefaultRootWindow(window->x_display), 0, 0, width, height, 0, 0, 0);
	XMapWindow(window->x_display, window->x_window);
	// Setup the X grahpics context.
	int screen_number = DefaultScreen(window->x_display);
	XGCValues values = {
		.foreground = BlackPixel(window->x_display, screen_number),
		.background = BlackPixel(window->x_display, screen_number),
		.line_width = 2,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	window->x_context = XCreateGC(window->x_display, window->x_window, mask, &values);
	return window;
}

void window_destroy(struct window *window) {
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window);
}

bool window_is_open(struct window *window) {
	XWindowAttributes attributes = {0};
	return XGetWindowAttributes(window->x_display, window->x_window, &attributes) != 0;
}

void window_flush(struct window *window) {
	XFlush(window->x_display);
}

void window_fill(struct window *window, unsigned long color) {
	XSetWindowBackground(window->x_display, window->x_window, color);
	XClearWindow(window->x_display, window->x_window);
}

void window_draw_line(struct window *window, unsigned long color, unsigned int line_width, int x1, int y1, int x2, int y2) {
	XSetForeground(window->x_display, window->x_context, color);
	XSetLineAttributes(window->x_display, window->x_context, line_width, LineSolid, CapButt, JoinRound);
	XDrawLine(window->x_display, window->x_window, window->x_context, x1, y1, x2, y2);
}
