#include <SDL2/SDL.h>
#include "game.h"
#include "graphics.h"
#include "menu_scene.h"
#include "scene.h"

struct MenuScene
{
    SDL_Texture *background;
};
typedef struct MenuScene MenuScene;

void menu_scene_start(Scene *scene)
{
    MenuScene *data = malloc(sizeof(MenuScene));
    Game *game = scene_get_game(scene);
    SDL_Renderer *renderer = game_get_renderer(game);

    data->background = load_bmp(renderer, "resources/title.bmp");
    scene_set_data(scene, data);
}

void menu_scene_update(Scene *scene, int dt, const InputState *input)
{
    if(input->select)
    {
	Game *game = scene_get_game(scene);
	game_switch_to_scene(game, SCENE_GAMEPLAY);
    }
}

void menu_scene_draw(Scene *scene, SDL_Renderer *r)
{
    MenuScene *data = scene_get_data(scene);
    SDL_RenderCopy(r, data->background, NULL,NULL);
    SDL_RenderPresent(r);
}

void menu_scene_stop(Scene *scene)
{
    MenuScene *data = scene_get_data(scene);
    if(data)
    {
	SDL_DestroyTexture(data->background);
    }
    free(data);
}
