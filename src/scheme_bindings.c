#include <libguile.h>
#include <SDL/SDL2.h>
#include <stdlib.h>
#include "scheme_bindings.h"

void *init_scheme(void *data)
{
    scm_c_define_gsubr("sdl-log", 1, 0, 0, sdl_log);
    //scm_c_define_gsubr("create-window", 3, 0, 0, create_window);
    return NULL;
}

static SCM sdl_log(SCM value)
{
    char *message = scm_to_locale_string(value);
    SDL_Log(message);
    free(message);
    return SCM_UNSPECIFIED;
}
