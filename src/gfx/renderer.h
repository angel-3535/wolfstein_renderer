#ifndef RENDERER_H
#define RENDERER_H

#include "../types.h"
#include "../world/world.h"
#include <entity/player.h>
#include <math.h>
#include <raylib.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TEXTURE_WIDTH 64
#define TEXTURE_HEIGHT 64

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

typedef struct {
  i32 position;
  vec2 direction0;
  vec2 direction1;
  i32 y;
} FloorCast;

void init_renderer();
void destroy_renderer();
void Draw_Map(i32 TILE_SIZE);
void Draw_RayHit_To_Buffer(RayCast *ray, f64 camera_x);
void Draw_Buffer();
void Cast_Ray(RayCast *ray, Player *player);
void Draw_FloorPixel_To_Buffer(ivec2, ivec2, i32, i32);
void Cast_Floor(FloorCast *floor_cast, Player *player);
void Draw_Sprite_To_Buffer(Sprite *sprite, vec2 sprite_pos, f64 *z_buffer,
                           i32 *sprite_order, Player *player);
Color Get_Wall_Color(int wall_type, int side);

#endif
