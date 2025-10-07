#include "renderer.h"
#include "types.h"
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

enum TextureType {
  TEXTURE_WALL_1 = 0,
  TEXTURE_WALL_2,
  TEXTURE_WALL_3,
  TEXTURE_WALL_4,
  TEXTURE_WALL_5,
  TEXTURE_WALL_6,
  TEXTURE_WALL_7,
  TEXTURE_COUNT
};

enum SpriteType {
  SPRITE_BARREL = 0,
  SPRITE_PILLAR,
  SPRITE_LIGHT,
  SPRITE_COUNT
};

Image sprite_textures[SPRITE_COUNT];
Image textures[TEXTURE_COUNT];
Image buffer;
Texture2D gpu_buffer;

void init_renderer() {
  // Load textures
  Image texture_atlas = LoadImage("../res/wolftextures.png");
  for (i32 i = 0; i < TEXTURE_COUNT; i++) {
    textures[i] = ImageFromImage(
        texture_atlas,
        (Rectangle){i * TEXTURE_WIDTH, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT});
  }
  Image sprite_atlas = LoadImage("../res/wolfsprites.png");

  for (i32 i = 0; i < SPRITE_COUNT; i++) {
    sprite_textures[i] = ImageFromImage(
        sprite_atlas,
        (Rectangle){i * TEXTURE_WIDTH, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT});
  }

  UnloadImage(texture_atlas);
  UnloadImage(sprite_atlas);

  printf("Loaded %d textures\n", TEXTURE_COUNT);

  buffer = GenImageColor(SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
}

void destroy_renderer() {
  for (i32 i = 0; i < TEXTURE_COUNT; i++) {
    UnloadImage(textures[i]);
  }
  if (buffer.data != NULL)
    UnloadImage(buffer);
  if (gpu_buffer.id != 0)
    UnloadTexture(gpu_buffer);
}

void Draw_Map(i32 TILE_SIZE) {
  const i32 y_offset = 5;
  const i32 x_offset = (SCREEN_WIDTH - TILE_SIZE * MAP_WIDTH) / 2;
  const i32 ui_map_size = TILE_SIZE * MAP_WIDTH;
  for (i32 y = 0; y < MAP_HEIGHT; y++) {
    DrawLine(x_offset, y * TILE_SIZE + y_offset, x_offset + ui_map_size,
             y * TILE_SIZE + y_offset, GRAY);
    for (i32 x = 0; x < MAP_WIDTH; x++) {
      if (y == 0) {
        DrawLine(x * TILE_SIZE + x_offset, y_offset, x * TILE_SIZE + x_offset,
                 y_offset + ui_map_size, GRAY);
      }
      if (worldMap[x][y] > 0) {
        Color color = Get_Wall_Color(worldMap[x][y], 0);
        const i32 ui_map_size = TILE_SIZE * MAP_WIDTH;
        DrawRectangle(x * TILE_SIZE + (SCREEN_WIDTH - ui_map_size) / 2,
                      y * TILE_SIZE + 5, TILE_SIZE, TILE_SIZE, color);
      }
    }
  }
}

void Draw_RayHit_To_Buffer(RayCast *ray, f64 camera_x) {
  if (ray->hit_info.hit == 0)
    return;
  i32 line_height = (i32)(SCREEN_HEIGHT / ray->hit_info.perp_wall_dist);
  i32 draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_start < 0)
    draw_start = 0;
  i32 draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_end >= SCREEN_HEIGHT)
    draw_end = SCREEN_HEIGHT - 1;

  i32 texture_index = ray->hit_info.hit - 1;

  if (texture_index < 0 || texture_index >= TEXTURE_COUNT) {
    ImageDrawLineV(&buffer, (vec2f){camera_x, (f32)draw_start},
                   (vec2f){camera_x, (f32)draw_end}, ray->hit_info.color);
    return;
  }

  f64 wall_x;
  if (ray->hit_info.side == 0)
    wall_x = ray->origin.y + ray->hit_info.perp_wall_dist * ray->direction.y;
  else
    wall_x = ray->origin.x + ray->hit_info.perp_wall_dist * ray->direction.x;
  // GET THE % FRACTIONAL PART OF wall_x
  wall_x -= floor((wall_x));
  // x coordinate on the texture based on wall_x
  i32 tex_x = (i32)(wall_x * (f64)TEXTURE_WIDTH);
  // If the wall is facing a certain direction, invert the texture x coordinate
  if (ray->hit_info.side == 0 && ray->direction.x > 0)
    tex_x = TEXTURE_WIDTH - tex_x - 1;
  if (ray->hit_info.side == 1 && ray->direction.y < 0)
    tex_x = TEXTURE_WIDTH - tex_x - 1;

  // How much to increase the texture coordinate per screen pixel
  f64 step = 1.0 * TEXTURE_HEIGHT / line_height;
  f64 tex_pos = (draw_start - SCREEN_HEIGHT / 2 + line_height / 2) * step;

  for (i32 y = draw_start; y < draw_end; y++) {
    i32 tex_y = (i32)tex_pos & (TEXTURE_HEIGHT - 1);
    tex_pos += step;
    Color color = GetImageColor(textures[texture_index], tex_x, tex_y);
    if (ray->hit_info.side == 1) {
      color.r = color.r / 2;
      color.g = color.g / 2;
      color.b = color.b / 2;
    }

    ImageDrawPixel(&buffer, camera_x, y, color);
  }
}

void Draw_Buffer() {
  if (gpu_buffer.id == 0) {
    gpu_buffer = LoadTextureFromImage(buffer);
  }
  UpdateTexture(gpu_buffer, buffer.data);
  DrawTexture(gpu_buffer, 0, 0, WHITE);
  ImageClearBackground(&buffer, BLACK);
}

void Cast_Ray(RayCast *ray, Player *player) {
  ivec2 map = {(i32)ray->origin.x, (i32)ray->origin.y};
  vec2 side_dist;
  vec2 delta_dist = {
      (ray->direction.x == 0) ? 1e30 : fabs(1 / ray->direction.x),
      (ray->direction.y == 0) ? 1e30 : fabs(1 / ray->direction.y)};

  ivec2 step;

  if (ray->direction.x < 0) {
    step.x = -1;
    side_dist.x = (ray->origin.x - map.x) * delta_dist.x;
  } else {
    step.x = 1;
    side_dist.x = (map.x + 1.0 - ray->origin.x) * delta_dist.x;
  }
  if (ray->direction.y < 0) {
    step.y = -1;
    side_dist.y = (ray->origin.y - map.y) * delta_dist.y;
  } else {
    step.y = 1;
    side_dist.y = (map.y + 1.0 - ray->origin.y) * delta_dist.y;
  }

  while (ray->hit_info.hit == 0) {
    if (side_dist.x < side_dist.y) {
      side_dist.x += delta_dist.x;
      map.x += step.x;
      ray->hit_info.side = 0;
    } else {
      side_dist.y += delta_dist.y;
      map.y += step.y;
      ray->hit_info.side = 1;
    }

    ray->hit_info.hit = worldMap[(i32)map.x][(i32)map.y];
  }

  if (ray->hit_info.side == 0) {
    ray->hit_info.perp_wall_dist =
        (map.x - player->position.x + (1 - step.x) / 2) / ray->direction.x;
  }

  else {
    ray->hit_info.perp_wall_dist =
        (map.y - player->position.y + (1 - step.y) / 2) / ray->direction.y;
  }

  ray->hit_info.color =
      Get_Wall_Color(worldMap[(i32)map.x][(i32)map.y], ray->hit_info.side);
}

Color Get_Wall_Color(int wall_type, int side) {
  Color color;
  switch (wall_type) {
  case 1:
    color = (Color){255, 0, 0, 255};
    break;
  case 2:
    color = (Color){0, 255, 0, 255};
    break;
  case 3:
    color = (Color){0, 0, 255, 255};
    break;
  case 4:
    color = (Color){255, 255, 255, 255};
    break;
  default:
    color = (Color){255, 255, 0, 255};
    break;
  }
  if (side == 1) {
    color.r = color.r / 2;
    color.g = color.g / 2;
    color.b = color.b / 2;
  }

  return color;
}

void Draw_FloorPixel_To_Buffer(ivec2 cell, ivec2 text_cord, i32 x, i32 y) {
  Color floor_color = GetImageColor(textures[3], text_cord.x, text_cord.y);
  Color ceiling_color = GetImageColor(textures[6], text_cord.x, text_cord.y);

  ImageDrawPixel(&buffer, x, y, floor_color);
  ImageDrawPixel(&buffer, x, SCREEN_HEIGHT - y - 1, ceiling_color);
}

void Cast_Floor(FloorCast *floor_cast, Player *player) {
  f64 pos_z = 0.5 * SCREEN_HEIGHT;

  f64 row_distance = pos_z / floor_cast->position;
  vec2 floor_step = {
      row_distance * (floor_cast->direction1.x - floor_cast->direction0.x) /
          SCREEN_WIDTH,
      row_distance * (floor_cast->direction1.y - floor_cast->direction0.y) /
          SCREEN_WIDTH};
  vec2 floor = {player->position.x + row_distance * floor_cast->direction0.x,
                player->position.y + row_distance * floor_cast->direction0.y};

  for (i32 x = 0; x < SCREEN_WIDTH; x++) {
    ivec2 cell = {(i32)(floor.x), (i32)(floor.y)};

    ivec2 tx_cord = {
        (i32)(TEXTURE_WIDTH * (floor.x - cell.x)) & (TEXTURE_WIDTH - 1),
        (i32)(TEXTURE_HEIGHT * (floor.y - cell.y)) & (TEXTURE_HEIGHT - 1)};

    floor.x += floor_step.x;
    floor.y += floor_step.y;

    Draw_FloorPixel_To_Buffer(cell, tx_cord, x, floor_cast->y);
  }
}

void Draw_Sprite_To_Buffer(Sprite *sprite, vec2 sprite_pos, f64 *z_buffer,
                           i32 *sprite_order, Player *player) {

  f64 inv_det = 1.0 / (player->camera_plane.x * player->direction.y -
                       player->direction.x * player->camera_plane.y);

  vec2 transform = {
      inv_det * (player->direction.y * sprite_pos.x -
                 player->direction.x * sprite_pos.y),
      inv_det * (-player->camera_plane.y * sprite_pos.x +
                 player->camera_plane.x * sprite_pos.y),
  };

  i32 sprite_screen_x =
      (i32)((SCREEN_WIDTH / 2) * (1 + transform.x / transform.y));

  i32 sprite_height = abs((i32)(SCREEN_HEIGHT / (transform.y)));

  i32 draw_start_y = -sprite_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_start_y < 0)
    draw_start_y = 0;
  i32 draw_end_y = sprite_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_end_y >= SCREEN_HEIGHT)
    draw_end_y = SCREEN_HEIGHT - 1;

  i32 sprite_width = abs((i32)(SCREEN_HEIGHT / (transform.y)));
  i32 draw_start_x = -sprite_width / 2 + sprite_screen_x;
  if (draw_start_x < 0)
    draw_start_x = 0;
  i32 draw_end_x = sprite_width / 2 + sprite_screen_x;
  if (draw_end_x >= SCREEN_WIDTH)
    draw_end_x = SCREEN_WIDTH - 1;

  for (i32 stripe = draw_start_x; stripe < draw_end_x; stripe++) {
    i32 tex_x = (i32)(256 * (stripe - (-sprite_width / 2 + sprite_screen_x)) *
                      TEXTURE_WIDTH / sprite_width) /
                256;
    if (transform.y > 0 && stripe > 0 && stripe < SCREEN_WIDTH &&
        transform.y < z_buffer[stripe]) {
      for (i32 y = draw_start_y; y < draw_end_y; y++) {
        i32 d = (y) * 256 - SCREEN_HEIGHT * 128 + sprite_height * 128;
        i32 tex_y = ((d * TEXTURE_HEIGHT) / sprite_height) / 256;
        Color color =
            GetImageColor(sprite_textures[sprite->texture], tex_x, tex_y);
        if (color.r > 0) {
          ImageDrawPixel(&buffer, stripe, y, color);
        }
      }
    }
  }
}
