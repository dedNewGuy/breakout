#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL3/SDL.h>

#include "rect.h"

#define WIN_TITLE "breakout"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define FPS 60
#define FRAMERATE (1000 / FPS)

void ball_update(SDL_FRect *ball, Vector2f direction, float deltatime)
{
	float speed = 500.0f * deltatime;
	rect_translate(ball, speed * direction.x, -speed * direction.y);
}

int main(void)
{
	srand(time(NULL));
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	bool done = false;
	bool at_starting_game = true;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init sdl: %s\n", SDL_GetError());
		return 1;
	}

	if(!SDL_CreateWindowAndRenderer(WIN_TITLE, WIN_WIDTH, WIN_HEIGHT,
			0, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	Vector2f surface_normal = {0};

	// ========== PADDLE
	SDL_FRect paddle = {
		.w = 150,
		.h = 15,
	};
	paddle.x = WIN_WIDTH / 2 - paddle.w / 2;
	paddle.y = WIN_HEIGHT - WIN_HEIGHT / 4;
	// ==========

	int brick_rows = 10;
	int brick_cols = 20;
	Brick bricks[brick_rows][brick_cols];
	brick_init(brick_rows, brick_cols, bricks, WIN_WIDTH);

	// ========== BALL
	SDL_FRect ball = {
		.w = 15,
		.h = 15,
	};
	ball.y = paddle.y - ball.h;
	Vector2f ball_direction;
	// ==========

	// ========== GENERAL SETTING
	float mouse_x = 0;
	int last_frame_time = 0;
	float deltatime = 0;
	// ==========


	while (!done) {


		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					done = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					SDL_Scancode key_code = event.key.scancode; 
					switch (key_code) {
						case SDL_SCANCODE_Q:
							done = true;
							break;
						default:
							break;
					}
					break;
				case SDL_EVENT_KEY_UP:
					break;
				case SDL_EVENT_MOUSE_MOTION:
					mouse_x = event.motion.x;
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					switch (event.button.button) 
					{
						case 1:
							at_starting_game = false;
							vector2f_randomize(&ball_direction, 1.0f);
							ball_direction = vector2f_normalize(ball_direction);
							break;
					}
					break;
			}
		}

		// CAP FRAMERATE
		int time_to_wait = FRAMERATE - (SDL_GetTicks() - last_frame_time);

		if (time_to_wait > 0 && time_to_wait <= FRAMERATE) 
			SDL_Delay(time_to_wait);

		deltatime = (SDL_GetTicks() - last_frame_time) / 1000.0f;
		// printf("deltatime: %f\n", deltatime);

		last_frame_time = SDL_GetTicks();

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		paddle.x = fmin((double)mouse_x, (double)WIN_WIDTH - paddle.w);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(renderer, &paddle);

		if (at_starting_game)
			ball.x = paddle.x + (paddle.w / 2) - (ball.w / 2);
		else
			ball_update(&ball, ball_direction, deltatime);

		if (ball.y + ball.h >= WIN_HEIGHT) {
			// TODO: Game over instead of quit
			done = true;
		}

		if (is_rect_wall_collide(ball, WIN_WIDTH, WIN_HEIGHT)) {
			surface_normal_wall_get(ball, &surface_normal, WIN_WIDTH);
			ball_direction = ball_reflection_direction(ball_direction, surface_normal);
		} else if (is_rect_collide(ball, paddle)) {
			surface_normal.x = 0;
			surface_normal.y = -1;
			ball_direction = ball_reflection_direction(ball_direction, surface_normal);
		}

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
		SDL_RenderFillRect(renderer, &ball);

		for (int y = 0; y < brick_rows; ++y) {
			for (int x = 0; x < brick_cols; ++x) {
				Brick brick = bricks[y][x];
				if (!brick.hit) {
					SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0x00);
					SDL_RenderFillRect(renderer, &brick.brick);

					SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0x00);
					SDL_RenderRect(renderer, &brick.brick);
				}
			}
		}

		for (int y = 0; y < brick_rows; ++y) {
			for (int x = 0; x < brick_cols; ++x) {
				Brick brick = bricks[y][x];
				if (is_rect_collide(ball, brick.brick) && !brick.hit) {
					surface_normal.x = 0;
					surface_normal.y = -1;
					ball_direction = ball_reflection_direction(ball_direction, surface_normal);
					bricks[y][x].hit = true;
					break;
				}
			}
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
	return 0;
}
