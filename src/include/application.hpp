#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <glad/glad.h>
#include <glfw/glfw3.h>

class Application {
public:
  Application() = default;

  void initialize(unsigned int w, unsigned int h, const char *label);
  void input();
  void update(float delta_time);
  void render();
  void shutdown();

  bool is_running() const;

  static Application *get_instance();

  GLFWwindow *window_;

private:
  float delta_time_;
  static Application *instance_;
  unsigned int screen_width;
  unsigned int screen_height;
};

#endif // __APPLICATION_H__