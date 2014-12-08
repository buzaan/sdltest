#include <stdio.h>
#include <SDL2/SDL.h>
#include "scene.h"
#include "sdltest.h"

struct game
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct scene *scene;
};

struct game *game_create(char *title, int x_size, int y_size)
{
    if(SDL_Init(SDL_INIT_AUDIO) != 0)
    {
	return NULL;
    }

    struct game *game = malloc(sizeof(struct game));
    game->window = SDL_CreateWindow(
	title, 
	100, 100, 
	x_size, y_size, 
	SDL_WINDOW_SHOWN);
    if(!game->window)
    {
	return NULL;
    }

    game->renderer = SDL_CreateRenderer(
	game->window, 
	-1, 
	SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!game->renderer)
    {
	return NULL;
    }

    game->scene = NULL;
    return game;
}

void game_destroy(struct game *game)
{
    if(game)
    {
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	if(game->scene)
	{
	    scene_destroy(game->scene);
	}
	free(game);
    }
}

bool game_should_quit(struct game *game)
{
    return true;
}

void game_tick(struct game *game)
{
    if(!game) return;

    if(scene)
    {
	scene->update(0);
	scene->draw(game->renderer);
    }
}		 


/*
SDL_Texture *load_bmp(SDL_Renderer *renderer, const_cstr path)
{
    if(!renderer || !path) return NULL;

    SDL_Surface *tmp = SDL_LoadBMP(path);
    if(tmp)
    {
	SDL_Texture *out = SDL_CreateTextureFromSurface(renderer, tmp);
	SDL_FreeSurface(tmp);
	return out;
    }
    else
    {
	return NULL;
    }
}

void init(Game *g)
{
}

void destroy()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
}

void game_loop()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    
    SDL_Delay(2000);
}
*/
