#ifndef RECT_H_
#define RECT_H_

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <stdlib.h>
#include <math.h>

// VECTOR STUFF =====
typedef struct {
	float x;
	float y;
} Vector2f;

Vector2f vector2f_negate(Vector2f vector);
Vector2f vector2f_scale(Vector2f vector, float scalar);
Vector2f vector2f_sub(Vector2f a, Vector2f b);
Vector2f vector2f_normalize(Vector2f vector);
Vector2f vector2f_mul(Vector2f a, Vector2f b);
Vector2f ball_reflection_direction(Vector2f ball_direction, Vector2f surface_normal);

float vector2f_dot(Vector2f a, Vector2f b);
float vector2f_magnitude(Vector2f vector);

bool is_rect_collide(SDL_FRect a, SDL_FRect b);
bool is_rect_wall_collide(SDL_FRect a, float win_width, float win_height);

void vector2f_randomize(Vector2f *vector, float bound);
void surface_normal_wall_get(SDL_FRect a, Vector2f *surface_normal, float win_width);
void rect_translate(SDL_FRect *a, float x, float y);


typedef struct {
	SDL_FRect brick;
	bool hit;
} Brick;

void brick_init(int rows, int cols, Brick bricks[][cols], float win_width);

#endif // RECT_H_
