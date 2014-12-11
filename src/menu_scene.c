#include <stdio.h>
#include <SDL2/SDL.h>
#include "game.h"
#include "graphics.h"
#include "menu_scene.h"
#include "scene.h"

/* TODO
struct Menu
{
    int choices;
    int x;
    int y;
    int selection;
    int item_width;
    int item_height;
    int offset;
    // void (*actions[5])(); ??
};
typedef struct Menu Menu;

Menu *menu_create(unsigned int x, unsigned int y, unsigned int iwidth, unsigned int iheight, unsigned int offset)
{
    Menu *out = malloc(sizeof(Menu));
    out->choice = 0;
    out->x = x;
    out->y = y;
    out->selection = 0;
    out->item_width = iwidth;
    out->item_height = iheight;
    out->offset = offset;
    return out;
}

void menu_add_item(Menu *menu, 
*/
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
    if(input->up && input->down)
    {
	// Cancel up & down. Ignore
    }
    else if(input->up)
    {
	//menu_move_up()
    }
    else if(input->down)
    {
	//menu_move_down()
    }
    else if(input->select)
    {
	//menu_do_selection()
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
