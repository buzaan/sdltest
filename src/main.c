#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "scene.h"
#include "scenes.h"

int main(int argc, char *argv[])
{
    int status = 0;
    Game *game = game_create("SDL Test", 800, 600);
    if(game != NULL)
    {
	Scene *test = scene_create(game);
	scene_set_start(test, test_scene_start);
	scene_set_update(test, test_scene_update);
	scene_set_draw(test, test_scene_draw);
	scene_set_stop(test, test_scene_stop);

	game_switch_to_scene(game, test);
	while(!game_should_quit(game))
	{
	    game_tick(game);
	}
	scene_destroy(test);
	game_destroy(game);
    }
    else
    {
	status = 1;
    }
    return status;
}
