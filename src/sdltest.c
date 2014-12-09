#include <stdio.h>
#include <SDL2/SDL.h>
#include "scene.h"
#include "sdltest.h"

struct game
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct scene *scene;
    struct scene_list
    {
	struct scene *scene;
	struct scene_list *next;
    } *scenes;
    Uint32 last_tick;
    bool quitting;
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
    game->last_tick = SDL_GetTicks();
    game->quitting = false;
    game->scenes = NULL;
    return game;
}

void game_destroy(struct game *game)
{
    if(game)
    {
	SDL_DestroyRenderer(game->renderer);
	SDL_DestroyWindow(game->window);
	while(game->scenes)
	{
	    struct scene_list *temp = game->scenes;
	    scene_destroy(game->scenes->scene);
	    game->scenes = game->scenes->next;
	    free(temp);
	}
	free(game);
    }
}

bool game_should_quit(struct game *game)
{
    return game && game->quitting;
}

static Uint64 update_timer(struct game *game)
{
    Uint32 cur_tick = SDL_GetTicks();
    Uint32 dt = cur_tick - game->last_tick;
    game->last_tick = cur_tick;
    return dt;
}

void game_tick(struct game *game)
{
    //TODO: move somewhere appropriate
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
	switch(event.type)
	{
	case SDL_QUIT:
	case SDL_KEYDOWN:
	    game->quitting = true;
	    break;
	default:
	    break;
	}
    }

    if(game->scene)
    {
	Uint32 dt = update_timer(game);
	scene_update(game->scene, dt);
	scene_draw(game->scene, game->renderer);
	SDL_Delay(dt < 1000 / 30 ? 1000 / 30 - dt : 0);
    }
}		 

void game_set_scene(struct game *g, struct scene *s)
{
    g->scene = s;
}

struct scene *game_add_scene(struct game *game, SceneInit init, SceneUpdate update, SceneDraw draw)
{
    struct scene *new_scene = scene_create(init, update, draw);
    struct scene_list *node = malloc(sizeof(struct scene_list));
    if(!game->scenes)
    {
	node->scene = new_scene;
	node->next = NULL;
	game->scenes = node;
    }
    else
    {
	node->scene = new_scene;
	node->next = game->scenes;
	game->scenes = node;
    }
    return new_scene;
}

void game_switch_to_scene(struct game *game, struct scene *scene)
{
    if(!game || !scene) return;
    
    struct scene_list *cur = game->scenes;
    while(cur)
    {
	if(cur->scene == scene)
	{
	    game->scene = cur->scene;
	    return;
	}
	else
	{
	    cur = cur->next;
	}
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
