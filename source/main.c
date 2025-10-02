#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include "graphics.h"

int main(void) {
	struct window *window = window_create("Hello World", 800, 600);
	if (!window) {
		fprintf(stderr, "Couldn't create window.\n");
		return 1;
	}

	while (window_is_open(window)) {
		window_fill(window, COLOR_BLACK);
		window_draw_line(window, COLOR_BLUE, 4, 0, 0, 100, 100);
		window_draw_line(window, COLOR_BLUE, 4, 100, 100, 100, 0);
		window_flush(window);
		usleep(1000);
	}

	window_destroy(window);
	return 0;
}
