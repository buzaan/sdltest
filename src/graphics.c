#include "graphics.h"

SDL_Texture *load_bmp(SDL_Renderer *renderer, char * const path)
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
