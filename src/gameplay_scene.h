#pragma once

void gameplay_scene_start(Scene *s);
void gameplay_scene_update(Scene *s, int dt, const InputState *input);
void gameplay_scene_draw(Scene *s, SDL_Renderer *renderer);
void gameplay_scene_stop(Scene *s);
