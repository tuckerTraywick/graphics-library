#include <stdio.h>
#include <X11/Xlib.h>

int main(void) {
	Display *main_display = XOpenDisplay(0);
	Window root_window = XDefaultRootWindow(main_display);
	Window main_window = XCreateSimpleWindow(main_display, root_window, 0, 0, 800, 600, 0, 0, 0x00000000);
	XMapWindow(main_display, main_window);
	XFlush(main_display	);

	while (1) {

	}

	return 0;
}
