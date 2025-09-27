#ifndef RENDERER_H
#define RENDERER_H

#include "../globals.h"
#include "../player.h"
#include "../types.h"
#include "../world/world.h"
#include <math.h>
#include <raylib.h>

typedef struct {
  i32 hit;
  i32 side;
  f64 perp_wall_dist;
  Color color;
} HitInfo;

typedef struct {
  vec2 origin;
  vec2 direction;
  HitInfo hit_info;
} RayCast;

void Draw_RayHit(RayCast *ray, f64 camera_x);
void Cast_Ray(RayCast *ray, Player *player);
Color Get_Wall_Color(int wall_type, int side);

#endif // INCLUDE/Users/angel/dev/personal/wolf3d/src/gfxrendererrenderer.h_
