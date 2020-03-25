#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_net.h>
#include <stdbool.h>

bool init();
void close();

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

int main(int argc, char* args[]) {
	
	bool running = false;
	
	if (!init()) {
		printf("Failed to initialize!\n");
		printf("Hej");
	}


	while (!running) {
		// här anropas allt när spelet körs
		SDL_UpdateWindowSurface(gWindow);
	}

	close();

	return 0;
}

bool init(void) {
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO != 0)) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		success = false;
	}
	else{
		//Create window
		gWindow = SDL_CreateWindow("SDL Example",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL){
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}
	return success;
}

void close(void){
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}