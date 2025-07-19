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

Vector2f vector2f_scale(Vector2f vector, float scalar)
{
	vector.x *= scalar;
	vector.y *= scalar;

	return (Vector2f){
		.x = vector.x,
		.y = vector.y,
	};
}

Vector2f vector2f_sub(Vector2f a, Vector2f b)
{
	return (Vector2f){
		.x = a.x - b.x,
		.y = a.y - b.y,
	};
}

void vector2f_randomize(Vector2f *vector, float bound)
{
	vector->x = (float)rand() / RAND_MAX * bound;
	vector->y = (float)rand() / RAND_MAX * bound;
}

float vector2f_magnitude(Vector2f vector)
{
	float magnitude = (vector.x * vector.x) + (vector.y* vector.y);
	magnitude = sqrtf(magnitude);
	return magnitude;
}

Vector2f vector2f_normalize(Vector2f vector)
{

	float magnitude = vector2f_magnitude(vector);

	return (Vector2f){
		.x = vector.x / magnitude,
		.y = vector.y / magnitude,
	};
}

Vector2f vector2f_mul(Vector2f a, Vector2f b)
{
	return (Vector2f)
	{
		.x = a.x * b.x,
		.y = a.y * b.y
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

void surface_normal_wall_get(SDL_FRect a, Vector2f *surface_normal)
{
	if (a.x + a.w >= WIN_WIDTH) {
		surface_normal->x = -1;
		surface_normal->y = 0;
	} else if (a.x <= 0) {
		surface_normal->x = 1;
		surface_normal->y = 0;
	} else if (a.y < 0) {
		surface_normal->x = 0;
		surface_normal->y = 1;
	} else {
		return;
	}
}

float vector2f_dot(Vector2f a, Vector2f b)
{
	return (a.x * b.x) + (a.y * b.y);
}

Vector2f vector2f_negate(Vector2f vector)
{
	vector.x *= -1;
	vector.y *= -1;
	return vector;
}

Vector2f ball_reflection_direction(Vector2f ball_direction, Vector2f surface_normal)
{

	float vdotn = vector2f_dot(ball_direction, surface_normal);
	vdotn *= 2;
	Vector2f x = vector2f_scale(surface_normal, vdotn);
	return vector2f_sub(ball_direction, x);
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

	Vector2f surface_normal = {0};

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

		if (is_rect_wall_collide(ball)) {
			surface_normal_wall_get(ball, &surface_normal);
			ball_direction = ball_reflection_direction(ball_direction, surface_normal);
		} else if (is_rect_collide(ball, paddle)) {
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
