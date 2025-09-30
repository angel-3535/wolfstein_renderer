#include "gfx/renderer.h"
#include <engine/scene.h>

#include <entity/player.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

Player player = {
    .position = {22.0, 12.0},
    .direction = {-1.0, 0.0},
    .camera_plane = {0.0, 0.66},
    .move_speed = 5.0,
    .rot_speed = 3.0,
};

f64 z_buffer[SCREEN_WIDTH];
i32 sprite_order[NUM_SPRITES];
f64 sprite_distance[NUM_SPRITES];

void Sort_Sprites(i32 *order, f64 *dist, i32 ammount);

f64 Distance_Between_Points(vec2 a, vec2 b) {
  return sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

f64 Fast_Distance_Between_Points(vec2 a, vec2 b) {
  return (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
}

void main_map_process_input(f64 delta_time) {
  vec2f move_direction = {0, 0};
  if (IsKeyDown(KEY_W)) {
    move_direction.x = 1;
  }
  if (IsKeyDown(KEY_S)) {
    move_direction.x = -1;
  }
  if (IsKeyDown(KEY_A)) {
    move_direction.y = 1;
  }
  if (IsKeyDown(KEY_D)) {
    move_direction.y = -1;
  }
  if (IsKeyDown(KEY_Q) || IsKeyDown(KEY_LEFT)) {
    Rotate_Player(player.rot_speed, delta_time);
  }
  if (IsKeyDown(KEY_E) || IsKeyDown(KEY_RIGHT)) {
    Rotate_Player(-player.rot_speed, delta_time);
  }

  move_direction = Vector2Normalize(move_direction);

  Move_Player(move_direction, delta_time);
}

void main_map_process(f64 delta_time) { main_map_process_input(delta_time); }

void main_map_draw() {

  // Floor casting
  for (i32 y = 0; y < SCREEN_HEIGHT; y++) {

    FloorCast floor_cast = {
        .y = y,
        .direction0 =
            {
                player.direction.x - player.camera_plane.x,
                player.direction.y - player.camera_plane.y,
            },
        .direction1 =
            {
                player.direction.x + player.camera_plane.x,
                player.direction.y + player.camera_plane.y,
            },
        .position = y - SCREEN_HEIGHT / 2,

    };

    Cast_Floor(&floor_cast, &player);
  }

  // Ray casting for walls
  for (i32 screen_x_pos = 0; screen_x_pos < SCREEN_WIDTH; screen_x_pos++) {
    f64 screen_plane_offset = 2 * screen_x_pos / (f64)SCREEN_WIDTH - 1;

    RayCast ray = {
        .direction = (vec2){player.direction.x +
                                player.camera_plane.x * screen_plane_offset,
                            player.direction.y +
                                player.camera_plane.y * screen_plane_offset},
        .origin = player.position,
    };

    Cast_Ray(&ray, &player);
    Draw_RayHit_To_Buffer(&ray, screen_x_pos);
    z_buffer[screen_x_pos] = ray.hit_info.perp_wall_dist;
  }

  for (i32 i = 0; i < NUM_SPRITES; i++) {
    sprite_order[i] = i;
    sprite_distance[i] =
        Fast_Distance_Between_Points(player.position, sprite[i].position);
  }

  Sort_Sprites(sprite_order, sprite_distance, NUM_SPRITES);

  for (i32 i = 0; i < NUM_SPRITES; i++) {

    vec2 sprite_pos = {
        sprite[sprite_order[i]].position.x - player.position.x,
        sprite[sprite_order[i]].position.y - player.position.y,
    };
    Draw_Sprite_To_Buffer(&sprite[sprite_order[i]], sprite_pos, z_buffer,
                          sprite_order, &player);
  }
  DrawText("Main Map Scene", 10, 30, 20, BLUE);
  Draw_Buffer();
}

Scene main_map_scene = {
    .process = main_map_process,
    .draw = main_map_draw,
};

void Sort_Sprites(i32 *order, f64 *dist, i32 ammount) {
  for (i32 i = 0; i < ammount - 1; i++) {
    for (i32 j = i + 1; j < ammount; j++) {
      if (dist[i] < dist[j]) {
        f64 temp_dist = dist[i];
        dist[i] = dist[j];
        dist[j] = temp_dist;
        i32 temp_order = order[i];
        order[i] = order[j];
        order[j] = temp_order;
      }
    }
  }
}
