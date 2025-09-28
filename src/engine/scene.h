#ifndef SCENE_H
#define SCENE_H

#include "../types.h"

typedef void (*_process)(f64 delta_time);
typedef void (*_draw)();

typedef struct {
  _process process;
  _draw draw;
} Scene;

extern Scene main_map_scene;
extern Scene main_menu_scene;

#endif
