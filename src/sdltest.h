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

typedef struct TileMap TileMap;
typedef struct TileSet TileSet;

enum TileType
{
    TT_EMPTY,
    TT_STONE,
    TT_WALL,
    TT_NUM_TYPES,
};

struct TileInfo
{
    enum TileType type;
    int hit_points;
};
typedef struct TileInfo TileInfo;

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
