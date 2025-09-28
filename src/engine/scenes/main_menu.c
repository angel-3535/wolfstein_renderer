#include "gfx/renderer.h"
#include <engine/scene.h>

#include <raylib.h>
#include <raymath.h>

void main_menu_process(f64 delta_time) {}
void main_menu_draw() {
  DrawText("Main Menu Scene", 10, 30, 20, RED);
  Draw_Map(SCREEN_WIDTH / MAP_WIDTH);
}

Scene main_menu_scene = {.process = main_menu_process, .draw = main_menu_draw};
