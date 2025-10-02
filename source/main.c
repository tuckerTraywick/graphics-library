#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>

int main(void) {
	Display *main_display = XOpenDisplay(NULL);
	Window root_window = XDefaultRootWindow(main_display);
	Window main_window = XCreateSimpleWindow(main_display, root_window, 0, 0, 1920, 1080, 0, 0, 0x00000000);
	XMapWindow(main_display, main_window);
	
	int screen_number = DefaultScreen(main_display);
	XGCValues values = {
		.foreground = WhitePixel(main_display, screen_number),
		.background = WhitePixel(main_display, screen_number),
		.line_width = 2,
		.line_style = LineSolid,
	};
	unsigned long mask = GCBackground | GCForeground | GCLineWidth | GCLineStyle;
	GC context = XCreateGC(main_display, main_window, mask, &values);

	int i = 0;
	while (1) {
		XClearWindow(main_display, main_window);
		XDrawLine(main_display, main_window, context, 50, 50, i, 400);  // Draw a line from (50,50) to (400,400)
		XFlush(main_display);
		usleep(1000);
		++i;
	}

	XDestroyWindow(main_display, main_window);
	XCloseDisplay(main_display);
	return 0;
}
