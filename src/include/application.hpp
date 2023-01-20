#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "stage.hpp"
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Application {
public:
  Application() = default;

  auto static get_instance() -> Application *;

  void initialize(unsigned int w, unsigned int h, const char *label, Stage *stage);
  void input();
  void update(float delta_time);
  void render();
  void shutdown();

  auto is_running() const -> bool;
  auto get_width() const -> unsigned int;
  auto get_height() const -> unsigned int;

  GLFWwindow *window_;
  Stage *stage;

private:
  float delta_time_;
  static Application *instance_;
  unsigned int screen_width;
  unsigned int screen_height;
};

#endif // __APPLICATION_H__