#define STB_IMAGE_IMPLEMENTATION

#include "application.hpp"

int main() {
  Application *application = {Application::get_instance()};

  Stage stage{};
  application->initialize(1200, 800, "OpenGL Testbed", &stage);

  float delta_time = {0.0f};
  float last_frame = {0.0f};
  float current_frame = {0.0f};

  while (application->is_running()) {
    current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    application->input();
    application->update(delta_time);
    application->render();
  }

  application->shutdown();

  return 0;
}
