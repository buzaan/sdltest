#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "gameplay_scene.h"
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
	game_register_scene(game, menu, SCENE_MENU);
	
	Scene *gameplay = scene_create(game);
	scene_set_start(gameplay, gameplay_scene_start);
	scene_set_update(gameplay, gameplay_scene_update);
	scene_set_draw(gameplay, gameplay_scene_draw);
	scene_set_stop(gameplay, gameplay_scene_stop);
	game_register_scene(game, gameplay, SCENE_GAMEPLAY);

	game_switch_to_scene(game, SCENE_MENU);
	while(!game_should_quit(game))
	{
	    game_tick(game);
	}
	scene_destroy(gameplay);
	scene_destroy(menu);
	game_destroy(game);
    }
    else
    {
	status = 1;
    }
    return status;
}
