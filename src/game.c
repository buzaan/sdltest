#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "input.h"
#include "scene.h"
#include "game.h"

struct game_s
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    Scene *scene;
    Uint32 last_tick;
    bool quitting;
};

Game *game_create(char *title, int x_size, int y_size)
{
    if(SDL_Init(SDL_INIT_AUDIO) != 0)
    {
	return NULL;
    }

    Game *game = malloc(sizeof(Game));
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
    game->last_tick = SDL_GetTicks();
    game->quitting = false;
    return game;
}

void game_destroy(Game *game)
{
    if(game)
    {
	if(game->scene)
	{
	    scene_stop(game->scene);
	}
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	free(game);
    }
}

bool game_should_quit(Game *game)
{
    return game && game->quitting;
}

static Uint64 update_timer(Game *game)
{
    Uint32 cur_tick = SDL_GetTicks();
    Uint32 dt = cur_tick - game->last_tick;
    game->last_tick = cur_tick;
    return dt;
}

void game_tick(Game *game)
{
    InputState state = { 
	.up = false, .down = false, 
	.left = false, .right = false,
	.cursor = {.x = 0, .y = 0, .active = false},
	.quit = false};

    input_update(&state, NULL);
    if(state.quit)
    {
	game->quitting = true;
	return;
    }

    if(game->scene)
    {
	Uint32 dt = update_timer(game);
	scene_update(game->scene, dt, &state);
	scene_draw(game->scene, game->renderer);

	// Probably wrong.
	SDL_Delay(dt < 1000 / 30 ? 1000 / 30 - dt : 0);
    }
}		 

void game_set_scene(Game *g, Scene *s)
{
    g->scene = s;
}

void game_switch_to_scene(Game *game, Scene *scene)
{
    if(!game || !scene) return;
    if(game->scene)
    {
	scene_stop(game->scene);
    }
    game->scene = scene;
    scene_start(scene);
}

SDL_Renderer *game_get_renderer(Game *game)
{
    return game->renderer;
}
