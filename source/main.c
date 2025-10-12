#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "graphics.h"

int main(void) {
	struct window *window = window_create("Hello World", 800, 600);
	if (!window) {
		fprintf(stderr, "Couldn't create window.\n");
		return 1;
	}

	int i = 0;
	while (window_is_open(window)) {
		window_fill(window, COLOR_BLACK);
		window_draw_rectangle(window, COLOR_BLUE, 2, 10, i, 100, 100);
		window_draw_text(window, "hello world", COLOR_WHITE, i, 20);
		window_flush(window);
		usleep(1000);
		i = (i + 1)%800;
	}

	printf("done\n");
	window_destroy(window);
	return 0;
}
