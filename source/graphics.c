#include <stdio.h>

#include <stdbool.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics.h"

struct window {
	char *name;
	struct vector2 size;
	bool is_open;
	pixel *frame_buffer;

	// Xlib state.
	Display *x_display;
	Window x_window;
	GC x_context;
	XftDraw *xft_draw;
	XImage *x_image;
	Atom x_delete_window;
	// XftFont *xft_font;
	// XftColor xft_color;
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
	int display_width =	DisplayWidth(window->x_display, default_screen);
	int display_height = DisplayHeight(window->x_display, default_screen);
	int default_depth = DefaultDepth(window->x_display, default_screen);
	Visual *default_visual = DefaultVisual(window->x_display, default_screen);

	// Allocate the framebuffer.
	window->frame_buffer = malloc(display_width*display_height*sizeof *window->frame_buffer);
	if (!window->frame_buffer) {
		goto error1;
	}

	// Setup the XImage.
	window->x_image = XCreateImage(
		window->x_display,
		default_visual,
		default_depth,
		ZPixmap,
		0,
		(char*)window->frame_buffer,
		display_width,
		display_height,
		32,
		0
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

	// Allocate an xft color for text rendering.
	// XRenderColor x_color = {.red=0xFFFF, .green=0xFFFF, .blue=0xFFFF, .alpha=0xFFFF};	
	// XftColorAllocValue(window->x_display, DefaultVisual(window->x_display, screen_number), DefaultColormap(window->x_display, screen_number), &x_color, &window->xft_color);
	return window;

error3:
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
	// XftColorFree(window->x_display, DefaultVisual(window->x_display, screen_number), DefaultColormap(window->x_display, screen_number), &window->xft_color);
	// XftFontClose(window->x_display, window->xft_font);
	XFreeGC(window->x_display, window->x_context);
	XftDrawDestroy(window->xft_draw);
	XDestroyWindow(window->x_display, window->x_window);
	XCloseDisplay(window->x_display);
	free(window->frame_buffer);
	free(window);
}

bool window_is_open(struct window *window) {
	return window->is_open;
}

void window_update(struct window *window) {
	XFlush(window->x_display);
	XEvent event = {0};
	XNextEvent(window->x_display, &event);
	if (event.type == ClientMessage) {
		window->is_open = false;
	}
}

void window_draw_pixel(struct window *window, struct vector2 position, pixel color) {

}

// bool window_load_font(struct window *window, char *font_name) {
// 	window->xft_font = XftFontOpenName(window->x_display, DefaultScreen(window->x_display), font_name);
// 	if (!window->xft_font) {
// 		return false;
// 	}
// 	return true;
// }

// void window_fill(struct window *window, unsigned long color) {
// 	XSetWindowBackground(window->x_display, window->x_window, color);
// 	XClearWindow(window->x_display, window->x_window);
// }

// void window_draw_point(struct window *window, unsigned long color, int x, int y) {
// 	XSetForeground(window->x_display, window->x_context, color);
// 	XDrawPoint(window->x_display, window->x_window, window->x_context, x, y);
// }

// void window_draw_line(struct window *window, unsigned long color, unsigned int line_width, int x1, int y1, int x2, int y2) {
// 	XSetForeground(window->x_display, window->x_context, color);
// 	XSetLineAttributes(window->x_display, window->x_context, line_width, LineSolid, CapButt, JoinRound);
// 	XDrawLine(window->x_display, window->x_window, window->x_context, x1, y1, x2, y2);
// }

// void window_draw_rectangle(struct window *window, unsigned long color, unsigned int line_width, int x, int y, unsigned int width, unsigned int height) {
// 	XSetForeground(window->x_display, window->x_context, color);
// 	XSetLineAttributes(window->x_display, window->x_context, line_width, LineSolid, CapButt, JoinRound);
// 	XDrawRectangle(window->x_display, window->x_window, window->x_context, x, y, width, height);	
// }

// void window_draw_rectangle_filled(struct window *window, unsigned long color, int x, int y, unsigned int width, unsigned int height) {
// 	XSetForeground(window->x_display, window->x_context, color);
// 	XFillRectangle(window->x_display, window->x_window, window->x_context, x, y, width, height);	
// }

// void window_draw_text(struct window *window, char *text, int x, int y) {
// 	XftDrawString8(window->xft_draw, &window->xft_color, window->xft_font, x, y, (const FcChar8*)text, strlen(text));
// }
