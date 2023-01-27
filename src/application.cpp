
#include "application.hpp"
#include "GLFW/glfw3.h"
#include "structs.hpp"
#include "light_sources.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "utils.hpp"
#include "vertex_array.hpp"
#include "editor.hpp"

#include <iterator>
#include <stb_image.hpp>
#include <glm/glm.hpp>
#include <iostream>

Application *Application::instance_ = {nullptr};

// prototypes

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_in, double y_in);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// body

void Application::initialize(unsigned int screen_w, unsigned int screen_h, const char *label) {

  // initialize glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  screen_width, screen_height = screen_w, screen_h;

  // window creation
  window_ = glfwCreateWindow(screen_w, screen_h, label, nullptr, nullptr);
  if (window_ == nullptr) {
    throw std::runtime_error("!failed to create glfw window");
  }

  // configure glfw
  glfwMakeContextCurrent(window_);
  glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
  glfwSetCursorPosCallback(window_, mouse_callback);
  glfwSetScrollCallback(window_, scroll_callback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    throw std::runtime_error("!failed to initialize glad");
  }

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  initialize_imgui(window_);
}

void Application::set_stage(Stage *stage_in) {
  stage_in->setup();
  stage = stage_in;
  glClearColor(stage->clear_.x, stage->clear_.y, stage->clear_.z, 1.0f);
}

void Application::input() {
  glfwPollEvents();

  Camera &camera = stage->camera;

  if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window_, true);
  }
  if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
    camera.process_keyboard(FORWARD, delta_time_);
  }
  if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
    camera.process_keyboard(BACKWARD, delta_time_);
  }
  if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    camera.process_keyboard(LEFT, delta_time_);
  }
  if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    camera.process_keyboard(RIGHT, delta_time_);
  }
  if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera.process_keyboard(UP, delta_time_);
  }

  glm::vec3 light_offset{0};
  if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
    light_offset += glm::vec3(-1.0f, 0.0f, 0.0f);
  }
  if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    light_offset += glm::vec3(1.0f, 0.0f, 0.0f);
  }
  if (glfwGetKey(window_, GLFW_KEY_UP) == GLFW_PRESS) {
    light_offset += glm::vec3(0.0f, 0.0f, -1.0f);
  }
  if (glfwGetKey(window_, GLFW_KEY_DOWN) == GLFW_PRESS) {
    light_offset += glm::vec3(0.0f, 0.0f, 1.0f);
  }
  if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
    light_offset += glm::vec3(0.0f, 1.0f, 0.0f);
  }
  if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
    light_offset += glm::vec3(0.0f, -1.0f, 0.0f);
  }
  stage->spot_lights[0].position += light_offset * delta_time_;


  if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS) {
    if (!stage->can_press)
      return;

    stage->can_press = false;
    stage->show_gui = !stage->show_gui;

    if (stage->show_gui) {
      stage->first_mouse = true;
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      stage->imgui_hovering = false;
    }
  }

  if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_RELEASE) {
    stage->can_press = true;
  }
}

void Application::update(float delta_time) {
  delta_time_ = delta_time;
  stage->update(delta_time);
}

void Application::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  stage->render();
  render_imgui(*stage, stage->batcher.get_statistics());

  glfwSwapBuffers(window_);
}

void Application::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

// getters

auto Application::get_instance() -> Application *{
  return instance_ == nullptr ? instance_ = new Application{} : instance_;
}

auto Application::is_running() const -> bool {
  return !glfwWindowShouldClose(window_);
}

auto Application::get_width() const -> unsigned int {
  return screen_width;
}

auto Application::get_height() const -> unsigned int {
  return screen_height;
}

// callbacks

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // width and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  Application::get_instance()->render();
}

void mouse_callback(GLFWwindow *window, double x_in, double y_in) {
  Stage *stage = Application::get_instance()->stage;
  if (stage->show_gui)
    return;

  float x_pos = static_cast<float>(x_in);
  float y_pos = static_cast<float>(y_in);

  if (stage->first_mouse) {
    stage->last_x = x_pos;
    stage->last_y = y_pos;
    stage->first_mouse = false;
  }

  float x_offset = x_pos - stage->last_x;
  float y_offset = stage->last_y - y_pos; // reversed since y-coordinates go from bottom to top

  stage->last_x = x_pos;
  stage->last_y = y_pos;

  stage->camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
  Stage *stage = Application::get_instance()->stage;
  if (stage->imgui_hovering)
    return;

  stage->camera.process_mouse_scroll(static_cast<float>(y_offset));
}