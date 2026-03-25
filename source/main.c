#define POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <time.h>

#include <stdio.h>
#include "graphics.h"

int main(void) {
	struct window window = window_create("Hello World", vec2(0, 0), vec2(800, 600));
	if (!window_is_open(&window)) {
		fprintf(stderr, "Couldn't create window.\n");
		return 1;
	}

	int32_t y = 0;
	while (window_is_open(&window)) {
		struct timespec start = {0};
		struct timespec end = {0};
		clock_gettime(CLOCK_MONOTONIC, &start);

		// Draw stuff.
		window_fill(&window, COLOR_BLACK);
		window_draw_rectangle_filled2(&window, vec2(100, 100), vec2(100, 100), 1, COLOR_WHITE, COLOR_RED);
		
		// Respond to events.
		window_update(&window);
		y = (y + 1)%200;

		clock_gettime(CLOCK_MONOTONIC, &end);
		double frame_time_seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
		// printf("frame time = %f, frame rate = %f\n", frame_time_seconds, 1.0/frame_time_seconds);
	}
 
	printf("Done.\n");
	window_destroy(&window);
	return 0;
}
