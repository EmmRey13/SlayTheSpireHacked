#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include "AtlasParse.h"

const int SC_WIDTH = 1080;
const int SC_HEIGHT = 920;

struct Pane 
{
	SDL_Window* window;
	SDL_Renderer* renderer;	
};

void close(Pane &p) 
{
	SDL_DestroyWindow(p.window);
	SDL_Quit();
}

int main(int argc, char* argv[]) 
{
	SDL_Init(SDL_INIT_VIDEO);

	Pane pane;
	Atlas atlas;

	pane.window = SDL_CreateWindow("title", SC_WIDTH, SC_HEIGHT, 0);
	pane.renderer = SDL_CreateRenderer(pane.window, NULL);
	SDL_SetRenderDrawColor(pane.renderer, 0, 0, 255, 255);

	if (!pane.window) {
		close(pane);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window not created", nullptr);
		return 1;
	}

	if (!pane.renderer) {
		close(pane);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Renderer not created",pane.window);
		return 1;
	}

	bool go = true;
	while (go == true) {

	

	}

	close(pane);

	return 0;
}

