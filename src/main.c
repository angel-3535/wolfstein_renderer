#include "gfx/renderer.h"
#include "types.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

Player player = {
    .position = {22.0, 12.0},
    .direction = {-1.0, 0.0},
    .camera_plane = {0.0, 0.66},
    .move_speed = 5.0,
    .rot_speed = 3.0,
};

void _process_input(f64 delta_time);

i32 main(i32 argc, char *argv[]) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wolf3d");

  while (!WindowShouldClose()) {
    const f32 delta_time = GetFrameTime();
    const i32 fps = GetFPS();

    _process_input(delta_time);

    BeginDrawing();
    ClearBackground(BLACK);

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
      Draw_RayHit(&ray, screen_plane_offset);
    }
    DrawText(TextFormat("FPS: %d", fps), 10, 10, 12, GREEN);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

void _process_input(f64 delta_time) {
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
    Rotate_Player(1, delta_time);
  }
  if (IsKeyDown(KEY_E) || IsKeyDown(KEY_RIGHT)) {
    Rotate_Player(-1, delta_time);
  }

  move_direction = Vector2Normalize(move_direction);

  Move_Player(move_direction, delta_time);
}
