
#ifndef WORLD_H
#define WORLD_H
#include "../types.h"

#define MAP_WIDTH 24
#define MAP_HEIGHT 24
#define NUM_SPRITES 19

typedef struct {
  vec2 position;
  i32 texture;
} Sprite;

extern Sprite sprite[NUM_SPRITES];
extern const i32 worldMap[MAP_WIDTH][MAP_HEIGHT];

#endif
