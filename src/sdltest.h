#pragma once
#include <stdbool.h>

typedef struct game_s Game;
typedef struct scene_s Scene;
struct InputState
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool select;
    struct pos
    {
	bool active;
	int x;
	int y;
    } cursor;
    bool quit;
};
typedef struct InputState InputState;
struct InputKeymap
{
    // not yet supported
};
typedef struct InputKeymap InputKeymap;

enum SceneID
{
    SCENE_MENU,
    SCENE_GAMEPLAY,
};
typedef enum SceneID SceneID;
