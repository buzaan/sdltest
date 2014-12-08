#include <stdio.h>
#include <SDL2/SDL.h>
#include "sdltest.h"

int main(int argc, char *argv[])
{
    int status = 0;
    struct game *game = game_create("SDL Test", 800, 600);
    if(game != NULL)
    {
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
