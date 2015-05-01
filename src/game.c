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

    struct scene_node
    {
	int scene_id;
	Scene *scene;
	struct scene_node *next;
    } *scenes;

    struct texture_node
    {
        SDL_Texture *texture;
        struct texture_node *next;
    } *textures;
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
    game->scenes = NULL;
    game->textures = NULL;
    return game;
}

static void free_scenes(struct scene_node *head)
{
    struct scene_node *cur = head;
    while(cur)
    {
        struct scene_node *tmp = cur;
        cur = tmp->next;
        free(tmp);
    };
}

static void free_textures(struct texture_node *head)
{
    struct texture_node *cur = head;
    while(cur)
    {
        struct texture_node *tmp = cur;
        SDL_DestroyTexture(tmp->texture);
        cur = tmp->next;
        free(tmp);
    }    
}

void game_destroy(Game *game)
{
    if(game)
    {
	if(game->scene)
	{
	    scene_stop(game->scene);
	}

        free_scenes(game->scenes);
        free_textures(game->textures);

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

	SDL_RenderPresent(game->renderer);

	// Probably wrong.
	SDL_Delay(dt < 1000 / 30 ? 1000 / 30 - dt : 0);
    }
}		 

static Scene *find_scene(struct scene_node *start, SceneID scene_id)
{
    while(start)
    {
	if(start->scene_id == scene_id)
	{
	    return start->scene;
	}
	else
	{
	    start = start->next;
	}
    }
    return NULL;
}

void game_set_scene(Game *g, SceneID scene_id)
{
    Scene *s = find_scene(g->scenes, scene_id);
    if(s)
    {
	g->scene = s;
    }
}

void game_register_scene(Game *game, Scene *scene, SceneID scene_id)
{
    struct scene_node *node = malloc(sizeof(struct scene_node));
    node->next = game->scenes;
    node->scene_id = scene_id;
    node->scene = scene;
    game->scenes = node;
}

void game_switch_to_scene(Game *game, SceneID scene_id)
{
    if(!game) return;

    Scene *s = find_scene(game->scenes, scene_id);
    if(game->scene)
    {
	scene_stop(game->scene);
    }

    if(s)
    {
	game->scene = s;
	scene_start(s);
    }
    else
    {
	fprintf(stderr, "Attempted to start nonextistent scene %d\n", scene_id);
    }
}

SDL_Texture *game_load_texture(Game *game, char *filename)
{
    SDL_Surface *bmp = SDL_LoadBMP(filename);
    if(!bmp)
    {
        fprintf(stderr, "LoadBMP: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture *tex = SDL_CreateTextureFromSurface(game->renderer, bmp);
    if(!tex)
    {
        fprintf(stderr, "CreateTextureFromSurface: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(bmp);    
    
    struct texture_node *node = malloc(sizeof(struct texture_node));
    node->texture = tex;
    node->next = game->textures;

    return tex;
}                              

SDL_Renderer *game_get_renderer(Game *game)
{
    return game->renderer;
}
