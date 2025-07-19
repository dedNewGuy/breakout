#include "rect.h"

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

bool is_rect_wall_collide(SDL_FRect a, float win_width, float win_height)
{
	return
		a.x + a.w >= win_width ||
		a.x <= 0 ||
		a.y + a.h >= win_height ||
		a.y < 0;
}

void surface_normal_wall_get(SDL_FRect a, Vector2f *surface_normal, float win_width)
{
	if (a.x + a.w >= win_width) {
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

void rect_translate(SDL_FRect *a, float x, float y)
{
	a->x += x;
	a->y += y;
}
