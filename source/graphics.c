#include <stdio.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
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
		.foreground = WhitePixel(window->x_display, screen_number),
		.background = BlackPixel(window->x_display, screen_number),
		.line_width = 1,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	window->x_context = XCreateGC(window->x_display, window->x_window, mask, &values);
	window->xft_draw = XftDrawCreate(window->x_display, window->x_window, DefaultVisual(window->x_display, screen_number), DefaultColormap(window->x_display, screen_number));
	// Allocate an xft color for text rendering.
	XRenderColor x_color = {.red=0xFFFF, .green=0xFFFF, .blue=0xFFFF, .alpha=0xFFFF};	
	XftColorAllocValue(window->x_display, DefaultVisual(window->x_display, screen_number), DefaultColormap(window->x_display, screen_number), &x_color, &window->xft_color);
	return window;
}

void window_destroy(struct window *window) {
	int screen_number = DefaultScreen(window->x_display);
	XftColorFree(window->x_display, DefaultVisual(window->x_display, screen_number), DefaultColormap(window->x_display, screen_number), &window->xft_color);
	XftFontClose(window->x_display, window->xft_font);
	XftDrawDestroy(window->xft_draw);
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

bool window_load_font(struct window *window, char *font_name) {
	window->xft_font = XftFontOpenName(window->x_display, DefaultScreen(window->x_display), font_name);
	if (!window->xft_font) {
		return false;
	}
	return true;
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

void window_draw_rectangle(struct window *window, unsigned long color, unsigned int line_width, int x, int y, unsigned int width, unsigned int height) {
	XSetForeground(window->x_display, window->x_context, color);
	XSetLineAttributes(window->x_display, window->x_context, line_width, LineSolid, CapButt, JoinRound);
	XDrawRectangle(window->x_display, window->x_window, window->x_context, x, y, width, height);	
}

void window_draw_rectangle_filled(struct window *window, unsigned long color, int x, int y, unsigned int width, unsigned int height) {
	XSetForeground(window->x_display, window->x_context, color);
	XFillRectangle(window->x_display, window->x_window, window->x_context, x, y, width, height);	
}

void window_draw_text(struct window *window, char *text, int x, int y) {
	XftDrawString8(window->xft_draw, &window->xft_color, window->xft_font, x, y, (const FcChar8*)text, strlen(text));
}
