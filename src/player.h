#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include <raylib.h>

typedef struct {
  vec2 position;
  vec2 direction;
  vec2 camera_plane;
  f64 move_speed;
  f64 rot_speed;
} Player;

void Move_Player(Vector2 direction, f64 delta_time);
void Rotate_Player(f64 angle, f64 delta_time);

extern Player player;
#endif // INCLUDE/Users/angel/dev/personal/wolf3d/srcplayerplayer.h_
