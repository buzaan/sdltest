#include <assert.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "input.h"

bool input_read_keymap(char const *file, InputKeymap *map)
{
    fputs("Custom keybindings not yet supported.\n", stderr);
    return false;

    FILE *f = fopen(file, "r");
    if(f)
    {
        char symname[16];
        char action[16];
        while(fscanf(f, "%15s:%15s\n", symname, action))
        {
            //input_keymap_set_action(map, ?, ?);
        }
        return true;
    }
    else
    {
        return false;
    }
}

void input_update(InputState *state, InputKeymap *map)
{
    assert(state);

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            state->quit = true;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_UP: state->up = true; break;
            case SDLK_DOWN: state->down = true; break;
            case SDLK_LEFT: state->left = true; break;
            case SDLK_RIGHT: state->right = true; break;
            case SDLK_SPACE:
            case SDLK_RETURN: 
                state->select = true;
                break;
            default: 
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch(event.button.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                state->cursor.x = event.button.x;
                state->cursor.y = event.button.y;
                state->cursor.active = true;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

