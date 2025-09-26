
#include "map.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  InitWindow(screen_width, screen_height, "Wolf3d");

  const Vector2 player_pos = {22.0f, 12.0f};
  const Vector2 player_dir = {-1.0f, 0.0f};
  const Vector2 camera_plane = {0.0f, 0.66f};

  while (!WindowShouldClose()) {
    const float delta_time = GetFrameTime();
    const int fps = GetFPS();
    BeginDrawing();
    ClearBackground(BLACK);

    for (int pos_x = 0; pos_x < screen_width; pos_x++) {
      double camera_x = 2 * pos_x / (double)screen_width - 1;
      Vector2 ray_dir = {player_dir.x + camera_plane.x * camera_x,
                         player_dir.y + camera_plane.y * camera_x};
      int map_x = (int)player_pos.x;
      int map_y = (int)player_pos.y;

      double side_dist_x;
      double side_dist_y;

      Vector2 delta_dist = {(ray_dir.x == 0) ? 1e30 : fabs(1 / ray_dir.x),
                            (ray_dir.y == 0) ? 1e30 : fabs(1 / ray_dir.y)};
      double perp_wall_dist;

      int step_x;
      int step_y;

      int hit = 0;
      int side;

      if (ray_dir.x < 0) {
        step_x = -1;
        side_dist_x = (player_pos.x - map_x) * delta_dist.x;
      } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0 - player_pos.x) * delta_dist.x;
      }
      if (ray_dir.y < 0) {
        step_y = -1;
        side_dist_y = (player_pos.y - map_y) * delta_dist.y;
      } else {
        step_y = 1;
        side_dist_y = (map_y + 1.0 - player_pos.y) * delta_dist.y;
      }

      while (hit == 0) {
        if (side_dist_x < side_dist_y) {
          side_dist_x += delta_dist.x;
          map_x += step_x;
          side = 0;
        } else {
          side_dist_y += delta_dist.y;
          map_y += step_y;
          side = 1;
        }

        if (worldMap[map_x][map_y] > 0)
          hit = 1;
      }

      if (side == 0)
        perp_wall_dist = (map_x - player_pos.x + (1 - step_x) / 2) / ray_dir.x;
      else
        perp_wall_dist = (map_y - player_pos.y + (1 - step_y) / 2) / ray_dir.y;

      int line_height = (int)(screen_height / perp_wall_dist);

      int draw_start = -line_height / 2 + screen_height / 2;
      if (draw_start < 0)
        draw_start = 0;

      int draw_end = line_height / 2 + screen_height / 2;

      if (draw_end >= screen_height)
        draw_end = screen_height - 1;

      Color color;
      switch (worldMap[map_x][map_y]) {
      case 1:
        if (side == 1)
          color = (Color){255, 0, 0, 255};
        break;
      case 2:
        if (side == 2)
          color = (Color){0, 255, 0, 255};
        break;
      case 3:
        if (side == 3)
          color = (Color){0, 0, 255, 255};
        break;
      case 4:
        if (side == 4)
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

      DrawLine(pos_x, draw_start, pos_x, draw_end, color);
      DrawText(TextFormat("FPS: %d", fps), 10, 10, 12, GREEN);
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
