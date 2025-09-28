#include "player.h"
#include "../world/world.h"
#include <math.h>

void Move_Player(Vector2 move_vector, f64 delta_time) {
  f32 velocity = player.move_speed * delta_time;
  vec2 direction = {
      player.direction.x * move_vector.x - player.direction.y * move_vector.y,
      player.direction.y * move_vector.x + player.direction.x * move_vector.y,
  };

  i32 pos_x = (player.position.x + (direction.x * velocity));
  i32 pos_y = (player.position.y + (direction.y * velocity));

  if (worldMap[(i32)player.position.x][pos_y] == false) {
    player.position.y += direction.y * velocity;
  }
  if (worldMap[pos_x][(i32)player.position.y] == false) {
    player.position.x += direction.x * velocity;
  }
}

void Rotate_Player(f64 angle, f64 delta_time) {
  if (angle != 0) {
    f32 old_dir_x = player.direction.x;
    player.direction.x = player.direction.x * cos(angle * delta_time) -
                         player.direction.y * sin(angle * delta_time);

    player.direction.y = old_dir_x * sin(angle * delta_time) +
                         player.direction.y * cos(angle * delta_time);
    f32 old_plane_x = player.camera_plane.x;
    player.camera_plane.x = player.camera_plane.x * cos(angle * delta_time) -
                            player.camera_plane.y * sin(angle * delta_time);
    player.camera_plane.y = old_plane_x * sin(angle * delta_time) +
                            player.camera_plane.y * cos(angle * delta_time);
  }
}
