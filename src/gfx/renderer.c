#include "renderer.h"

void Draw_RayHit(RayCast *ray, f64 camera_x) {
  if (ray->hit_info.hit == 0)
    return;
  i32 line_height = (i32)(SCREEN_HEIGHT / ray->hit_info.perp_wall_dist);
  i32 draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_start < 0)
    draw_start = 0;
  i32 draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
  if (draw_end >= SCREEN_HEIGHT)
    draw_end = SCREEN_HEIGHT - 1;
  DrawLineV((vec2f){(f32)((camera_x + 1) * SCREEN_WIDTH / 2), (f32)draw_start},
            (vec2f){(f32)((camera_x + 1) * SCREEN_WIDTH / 2), (f32)draw_end},
            ray->hit_info.color);
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

    if (worldMap[(i32)map.x][(i32)map.y] > 0)
      ray->hit_info.hit = 1;
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
