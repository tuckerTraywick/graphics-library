#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/XShm.h>
#include "backend.h"
#include "graphics.h"

struct backend_window {
	struct surface surface;
	Display *x_display;
	Window x_window;
	GC x_context;
	XftDraw *xft_draw;
	XShmSegmentInfo x_shm_info;
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

	window->surface = (struct surface){
		.parent_size = vec2(DisplayWidth(window->x_display, default_screen), DisplayHeight(window->x_display, default_screen)),
		.size = size,
	};

	// Create the image.
	window->x_image = XShmCreateImage(
		window->x_display, default_visual, default_depth, ZPixmap, NULL, &window->x_shm_info, window->surface.parent_size.x, window->surface.parent_size.y
	);
	// Allocate shared memory for the image's pixels.
	window->x_shm_info.shmid = shmget(IPC_PRIVATE, window->surface.parent_size.x*window->surface.parent_size.y*sizeof *window->surface.pixels, IPC_CREAT | 0777);
	window->x_shm_info.shmaddr = shmat(window->x_shm_info.shmid, NULL, 0);
	window->x_shm_info.readOnly = false;
	window->x_image->data = window->x_shm_info.shmaddr;
	XShmAttach(window->x_display, &window->x_shm_info);
	window->surface.pixels = (uint32_t*)window->x_shm_info.shmaddr;

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
	XShmDetach(window->x_display, &window->x_shm_info);
	XDestroyImage(window->x_image);
	shmdt(window->x_shm_info.shmaddr);
	shmctl(window->x_shm_info.shmid, IPC_RMID, NULL);
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
	XShmPutImage(window->x_display, window->x_window, window->x_context, window->x_image, 0, 0, 0, 0, window->surface.size.x, window->surface.size.y, false);
	XSync(window->x_display, false);
	
	// Check for events.
	if (XPending(window->x_display)) {
		XEvent event = {0};
		XNextEvent(window->x_display, &event); // Blocks.
		return event.type != ClientMessage;
	}
	return true;
}
