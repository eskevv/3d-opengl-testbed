#ifndef __STAGE_H__
#define __STAGE_H__

#include "camera.hpp"
#include "light_sources.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"
#include "model.hpp"
#include "utils.hpp"
#include "batcher.hpp"

#include <stb_image.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Stage {
public:
  void setup();
  void update(float delta_time);
  void render();

  void apply_spotlight(Stage &stage, const SpotLight &light, unsigned int place);
  void apply_pointlight(Stage &stage, const PointLight &light, unsigned int place);
  void apply_directional(Stage &stage, const DirectionalLight &light, unsigned int place);

  void use_true_tone();
  void use_lighting(Stage &stage);
  void apply_textures();

  bool imgui_hovering = {false};
  bool first_mouse = {true};
  bool show_gui = {true};
  bool can_press = {true};
  Camera camera = {{0.0f, 3.0f, 20.0f}};
  float move_speed = {12.0f};
  const unsigned int SCR_WIDTH = {1200};
  const unsigned int SCR_HEIGHT = {800};
  float last_x = {SCR_WIDTH / 2.0f};
  float last_y = {SCR_HEIGHT / 2.0f};
  glm::vec3 clear_{0.094f, 0.086f, 0.063f};

  // lights / objects
  static constexpr size_t NUM_CUBES = {1210};
  DirectionalLight dir_lights[1];
  SpotLight spot_lights[1];
  PointLight point_lights[4];
  glm::mat4 projection;
  glm::mat4 view;
  float material_shininess = {0.02f};
  float emission_strength = {1.3f};
  float emission_speed = {0.45f};
  Model backpack;
  unsigned int diffuse_map;
  unsigned int specular_map;
  unsigned int emission_map;
  unsigned int hills;
  Batcher batcher = {1200, 800};

  // opengl
  Shader lighting_shader;
  Shader light_cube_shader;
  Shader simple_shader;

  glm::vec3 cube_positions[NUM_CUBES] = {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 0, 3), glm::vec3(2, 1, 0)};
};

#endif // __STAGE_H__