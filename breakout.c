#include <stdbool.h>
#include <SDL3/SDL.h>

#define WIN_TITLE "breakout"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main()
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	bool done = false;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init sdl: %s\n", SDL_GetError());
		return 1;
	}

	if(!SDL_CreateWindowAndRenderer(WIN_TITLE, WIN_WIDTH, WIN_HEIGHT,
			0, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_FRect paddle = {
		.w = 150,
		.h = 15,
	};
	paddle.x = WIN_WIDTH / 2 - paddle.w / 2;
	paddle.y = WIN_HEIGHT - WIN_HEIGHT / 4;

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
			}
		}

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(renderer, &paddle);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
	return 0;
}
