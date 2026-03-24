#include <unistd.h>


#include <stdio.h>
#include "graphics.h"

int main(void) {
	struct window *window = window_create("Hello World", vec2(0, 0), vec2(800, 600));
	if (!window) {
		fprintf(stderr, "Couldn't create window.\n");
		return 1;
	}
	// if (!window_load_font(window, "dejavu sans mono-20")) {
	// 	fprintf(stderr, "Couldn't load font.\n");
	// 	return 1;
	// }

	// int i = 0;
	// while (window_is_open(window)) {
	// 	window_fill(window, COLOR_BLACK);
	// 	window_draw_rectangle(window, COLOR_BLUE, 2, 10, 10, 100, 100);
	// 	window_draw_text(window, "hello world", 50, 50);
	// 	window_update(window);
	// 	usleep(1000);
	// 	i = (i + 1)%800;
	// }

	while (window_is_open(window)) {
		// respond to events
		// clear screen
		// draw stuff
		window_update(window);
	}

	printf("Done.\n");
	window_destroy(window);
	return 0;
}
