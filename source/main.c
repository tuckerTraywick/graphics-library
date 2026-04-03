#define POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <time.h>

#include <stdio.h>
#include "graphics.h"

int main(void) {
	struct window *window = window_create("Hello World", vec2(0, 0), vec2(1280, 720));
	if (!window_is_open(window)) {
		fprintf(stderr, "Couldn't create window.\n");
		return 1;
	}
	struct surface *surface = window_get_surface(window);

	struct surface sprite = surface_create(vec2(100, 100));
	surface_fill(&sprite, COLOR_RED);
	surface_draw_line2(&sprite, vec2(0, 0), vec2(99, 99), 1, COLOR_WHITE);
	surface_draw_line2(&sprite, vec2(0, 99), vec2(99, 0), 1, COLOR_WHITE);

	float y = 0.0f;
	struct vector2 mouse = {0};
	uint32_t samples = 0;
	double avg_frame_time = 0.0f;
	while (window_is_open(window)) {
		struct timespec start = {0};
		struct timespec end = {0};
		clock_gettime(CLOCK_MONOTONIC, &start);

		// Draw stuff.
		surface_fill(surface, COLOR_BLUE);
		surface_draw_surface_centered2(surface, &sprite, vec2(y, y), vec2(y, y), y);
		surface_draw_rectangle_centered2(surface, vec2(200, 200), vec2(20, 20), 1, COLOR_GREEN);
		surface_draw_surface_centered2(surface, &sprite, mouse, vec2(y, y), -y);

		// Respond to events.
		window_update(window);
		y = (y >= 180.0f) ? 0.0f : y + 0.1f;
		mouse = window_get_mouse_position(window);

		clock_gettime(CLOCK_MONOTONIC, &end);
		double frame_time_seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
		avg_frame_time += frame_time_seconds;
		++samples;
		if (samples == 50) {
			printf("frame rate = %f\n", 1.0/(avg_frame_time/(double)samples));
			avg_frame_time = 0.0f;
			samples = 0;
		}
	}

	surface_destroy(&sprite);

	printf("Done.\n");
	window_destroy(window);
	return 0;
}
