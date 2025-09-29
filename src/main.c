#include "gfx/renderer.h"
#include "types.h"
#include <engine/scene.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

void _process_system_input(f64 delta_time);

enum SCENES {
  SCENE_MAIN_MENU = 0,
  SCENE_MAIN_MAP,
  SCENE_COUNT,

};

enum SCENES current_scene = SCENE_MAIN_MAP;

i32 main(i32 argc, char *argv[]) {
  init_renderer();
  Scene scenes[SCENE_COUNT] = {
      [SCENE_MAIN_MENU] = main_menu_scene,
      [SCENE_MAIN_MAP] = main_map_scene,
  };

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Wolf3d");

  while (!WindowShouldClose()) {
    const f32 delta_time = GetFrameTime();
    const i32 fps = GetFPS();

    scenes[current_scene].process(delta_time);
    _process_system_input(delta_time);

    BeginDrawing();
    ClearBackground(BLACK);

    scenes[current_scene].draw();

    DrawText(TextFormat("FPS: %d", fps), 10, 10, 12, GREEN);
    EndDrawing();
  }

  CloseWindow();

  destroy_renderer();
  return 0;
}

void _process_system_input(f64 delta_time) {
  if (IsKeyPressed(KEY_TAB)) {
    current_scene = (current_scene + 1) % SCENE_COUNT;
  }
}
