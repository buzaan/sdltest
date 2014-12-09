#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "scenes.h"

int main(int argc, char *argv[])
{
    int status = 0;
    struct game *game = game_create("SDL Test", 800, 600);
    if(game != NULL)
    {
	struct scene *test = game_add_scene(game, 
		       test_scene_init, 
		       test_scene_update, 
		       test_scene_draw);
	game_switch_to_scene(game, test);
	while(!game_should_quit(game))
	{
	    game_tick(game);
	}
	game_destroy(game);
    }
    else
    {
	status = 1;
    }
    return status;
}
