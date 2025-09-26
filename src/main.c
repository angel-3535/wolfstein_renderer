
#include "globals.h"
#include "types.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  vec2 position;
  vec2 direction;
  vec2 camera_plane;
  f64 move_speed;
  f64 rot_speed;
} Player;

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

void Cast_Ray(RayCast *ray, Player *player);
void Draw_RayHit(RayCast *ray, f64 camera_x);
void _process_input(f64 delta_time);

Player player = {
    .position = {22.0, 12.0},
    .direction = {-1.0, 0.0},
    .camera_plane = {0.0, 0.66},
    .move_speed = 5.0,
    .rot_speed = 3.0,
};

i32 main(i32 argc, char *argv[]) {
  InitWindow(screen_width, screen_height, "Wolf3d");

  while (!WindowShouldClose()) {
    const f32 delta_time = GetFrameTime();
    const i32 fps = GetFPS();

    _process_input(delta_time);

    BeginDrawing();
    ClearBackground(BLACK);

    for (i32 pos_x = 0; pos_x < screen_width; pos_x++) {
      f64 camera_x = 2 * pos_x / (f64)screen_width - 1;

      RayCast ray = {};
      ray.direction =
          (vec2){player.direction.x + player.camera_plane.x * camera_x,
                 player.direction.y + player.camera_plane.y * camera_x};

      ray.origin.x = player.position.x;
      ray.origin.y = player.position.y;

      Cast_Ray(&ray, &player);
      Draw_RayHit(&ray, camera_x);
    }
    DrawText(TextFormat("FPS: %d", fps), 10, 10, 12, GREEN);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void _process_input(f64 delta_time) {
  if (IsKeyDown(KEY_W)) {
    f32 velocity = player.move_speed * delta_time;

    i32 pos_x = (player.position.x + (player.direction.x * velocity));
    i32 pos_y = (player.position.y + (player.direction.y * velocity));

    if (worldMap[(i32)player.position.x][pos_y] == false) {
      player.position.y += player.direction.y * velocity;
    }
    if (worldMap[pos_x][(i32)player.position.y] == false) {
      player.position.x += player.direction.x * velocity;
    }
  }

  if (IsKeyDown(KEY_S)) {
    f32 velocity = player.move_speed * delta_time;

    i32 pos_x = (player.position.x - (player.direction.x * velocity));
    i32 pos_y = (player.position.y - (player.direction.y * velocity));

    if (worldMap[(i32)player.position.x][pos_y] == false) {
      player.position.y -= player.direction.y * velocity;
    }
    if (worldMap[pos_x][(i32)player.position.y] == false) {
      player.position.x -= player.direction.x * velocity;
    }
  }
  // Rotate right
  if (IsKeyDown(KEY_A)) {
    f32 old_dir_x = player.direction.x;
    player.direction.x =
        player.direction.x * cos(player.rot_speed * delta_time) -
        player.direction.y * sin(player.rot_speed * delta_time);

    player.direction.y =
        old_dir_x * sin(player.rot_speed * delta_time) +
        player.direction.y * cos(player.rot_speed * delta_time);
    f32 old_plane_x = player.camera_plane.x;
    player.camera_plane.x =
        player.camera_plane.x * cos(player.rot_speed * delta_time) -
        player.camera_plane.y * sin(player.rot_speed * delta_time);
    player.camera_plane.y =
        old_plane_x * sin(player.rot_speed * delta_time) +
        player.camera_plane.y * cos(player.rot_speed * delta_time);
  }

  // Rotate left
  if (IsKeyDown(KEY_D)) {
    f32 old_dir_x = player.direction.x;
    player.direction.x =
        player.direction.x * cos(-player.rot_speed * delta_time) -
        player.direction.y * sin(-player.rot_speed * delta_time);

    player.direction.y =
        old_dir_x * sin(-player.rot_speed * delta_time) +
        player.direction.y * cos(-player.rot_speed * delta_time);
    f32 old_plane_x = player.camera_plane.x;
    player.camera_plane.x =
        player.camera_plane.x * cos(-player.rot_speed * delta_time) -
        player.camera_plane.y * sin(-player.rot_speed * delta_time);
    player.camera_plane.y =
        old_plane_x * sin(-player.rot_speed * delta_time) +
        player.camera_plane.y * cos(-player.rot_speed * delta_time);
  }
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

void Draw_RayHit(RayCast *ray, f64 camera_x) {
  if (ray->hit_info.hit == 0)
    return;
  i32 line_height = (i32)(screen_height / ray->hit_info.perp_wall_dist);
  i32 draw_start = -line_height / 2 + screen_height / 2;
  if (draw_start < 0)
    draw_start = 0;
  i32 draw_end = line_height / 2 + screen_height / 2;
  if (draw_end >= screen_height)
    draw_end = screen_height - 1;
  DrawLineV((vec2f){(f32)((camera_x + 1) * screen_width / 2), (f32)draw_start},
            (vec2f){(f32)((camera_x + 1) * screen_width / 2), (f32)draw_end},
            ray->hit_info.color);
}
