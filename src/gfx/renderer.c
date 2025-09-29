#include "renderer.h"
#include "types.h"
#include <raylib.h>
#include <stdio.h>

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
