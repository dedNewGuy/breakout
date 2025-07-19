#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL3/SDL.h>

#define WIN_TITLE "breakout"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

typedef struct {
	float x;
	float y;
} Vector2f;

void vector2f_randomize(Vector2f *vector, float bound)
{
	vector->x = (float)rand() / RAND_MAX * bound;
	vector->y = (float)rand() / RAND_MAX * bound;
}

Vector2f vector2f_normalize(Vector2f vector)
{
	float magnitude = (vector.x * vector.x) + (vector.y* vector.y);
	magnitude = sqrtf(magnitude);

	return (Vector2f){
		.x = vector.x / magnitude,
		.y = vector.y / magnitude,
	};
}

bool is_rect_collide(SDL_FRect a, SDL_FRect b)
{
	return 
		a.x < b.x + b.w &&
		a.x + a.w > b.x &&
		a.y < b.y + b.w &&
		a.y + a.h > b.y;
}

bool is_rect_wall_collide(SDL_FRect a)
{
	return
		a.x + a.w >= WIN_WIDTH ||
		a.x <= 0 ||
		a.y + a.h >= WIN_HEIGHT ||
		a.y < 0;
}

void rect_translate(SDL_FRect *a, float x, float y)
{
	a->x += x;
	a->y += y;
}

void ball_update(SDL_FRect *ball, Vector2f direction)
{
	rect_translate(ball, 1 * direction.x, -1 * direction.y);
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

	// ========== PADDLE
	SDL_FRect paddle = {
		.w = 150,
		.h = 15,
	};
	paddle.x = WIN_WIDTH / 2 - paddle.w / 2;
	paddle.y = WIN_HEIGHT - WIN_HEIGHT / 4;
	// ==========
	
	
	// ========== BALL
	SDL_FRect ball = {
		.w = 15,
		.h = 15,
	};
	ball.y = paddle.y - ball.h;
	Vector2f ball_direction;
	// ==========

	// ========== MOUSE
	float mouse_x = 0;
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

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		paddle.x = fmin((double)mouse_x, (double)WIN_WIDTH - paddle.w);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(renderer, &paddle);

		if (at_starting_game)
			ball.x = paddle.x + (paddle.w / 2) - (ball.w / 2);
		else
			ball_update(&ball, ball_direction);

		if (ball.y + ball.h >= WIN_HEIGHT) {
			// TODO: Game over instead of quit
			done = true;
		}

		if (is_rect_wall_collide(ball) || is_rect_collide(ball, paddle)) {
			ball_direction.x *= -1;
			ball_direction.y *= -1;
		}

		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
		SDL_RenderFillRect(renderer, &ball);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
	return 0;
}
