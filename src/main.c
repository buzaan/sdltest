#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "scene.h"
#include "menu_scene.h"

int main(int argc, char *argv[])
{
    int status = 0;
    Game *game = game_create("SDL Test", 800, 600);
    if(game != NULL)
    {
	Scene *menu = scene_create(game);
	scene_set_start(menu, menu_scene_start);
	scene_set_update(menu, menu_scene_update);
	scene_set_draw(menu, menu_scene_draw);
	scene_set_stop(menu, menu_scene_stop);

	game_switch_to_scene(game, menu);
	while(!game_should_quit(game))
	{
	    game_tick(game);
	}
	scene_destroy(menu);
	game_destroy(game);
    }
    else
    {
	status = 1;
    }
    return status;
}
