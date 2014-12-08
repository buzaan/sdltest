#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>



static void *init_scheme(void *data)
{
    scm_c_define_gsubr("sdl-log", 1, 0, 0, sdl_log);
    scm_c_define_gsubr("create-window", 3, 0, 0, create_window);
    return NULL;
}

int main(int argc, char *argv[])
{
    int status = 0;
    scm_with_guile(init_scheme, NULL);

    if(SDL_Init(SDL_INIT_AUDIO) != 0)
    {
	fprintf(stderr, "Init error:%s\n", SDL_GetError());
	return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Hello World!", 
				       100, 100, 
				       800, 600,
				       SDL_WINDOW_SHOWN);
    if(!win)
    {
	fprintf(stderr, "%s\n", SDL_GetError());
	status = 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
    {
	fprintf(stderr, "%s\n", SDL_GetError());
	status = 1;
    }

    scm_c_primitive_load("src/main.scm");
    SCM func = scm_variable_ref(scm_c_lookup("main"));
    scm_call_0(func);


    SDL_DestroyWindow(win);
    SDL_Quit();
    return status;
}
