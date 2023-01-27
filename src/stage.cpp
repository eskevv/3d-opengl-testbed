#include "stage.hpp"
#include "GLFW/glfw3.h"
#include "utils.hpp"
#include <corecrt.h>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>
#include <iostream>

std::map<std::string, unsigned> active_textures_;
int index_tex = {0};
int timer_s = {1};
auto it = active_textures_.begin();

void add_texture(const char *label, const char *path) {
  unsigned int texture = load_texture(path);
  active_textures_[label] = texture;
  std::cout << texture;
}

void Stage::update(float delta_time) {
  glClearColor(clear_.x, clear_.y, clear_.z, 1.0f);
  camera.MovementSpeed = move_speed;
  projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
  view = camera.get_view_matrix();
}

void Stage::render() {
  use_lighting(*this);

  bool switch_texture = {false};
  if (static_cast<int>(glfwGetTime()) % timer_s == 0) {
    timer_s += 1;
    switch_texture = true;
  }

  Material basic_material{active_textures_["diffuse"], active_textures_["specular"], active_textures_["matrix"]};

  batcher.begin(view, projection);
  batcher.draw_cube(cube_positions[0], basic_material);
  batcher.draw_cube(cube_positions[1], basic_material);
  batcher.draw_cube(cube_positions[2], basic_material);
  for (size_t i{3}; i < NUM_CUBES; i++) {
    // float angle = {i < 10 ? 20.0f * i + glfwGetTime() / 4 : 0.0f}; // rotate only the first 10 cubes
    // if (switch_texture) {
    //   index_tex = {rand() % static_cast<int>(active_textures_.size() - 1)};
    //   it = active_textures_.begin();
    //   std::advance(it, index_tex);
    // }
    batcher.draw_cube(cube_positions[i], basic_material);
  }

  batcher.end();

  // // with lightning
  // render_model(backpack, lighting_shader, glm::vec3{9.0f, 0.0f, 0.0f});

  // // unaffected by light
  // use_true_tone();
  // for (size_t i{0}; i < 1; i++) {
  //   if (!spot_lights[i].enabled)
  //     continue;
  //   light_vao.bind();
  //   render_lamp(light_cube_shader, spot_lights[i], spot_lights[i].position);
  // }

  // for (size_t i{0}; i < 4; i++) {
  //   if (!point_lights[i].enabled)
  //     continue;
  //   render_lamp(light_cube_shader, point_lights[i], point_lights[i].position);
  // }
}

void Stage::setup() {
  srand(static_cast<unsigned int>(time(nullptr)));

  stbi_set_flip_vertically_on_load(true);
  // create shader programs
  lighting_shader = Shader{"shaders/lighting.vert", "shaders/lighting.frag"};
  light_cube_shader = Shader{"shaders/light.vert", "shaders/light.frag"};
  simple_shader = Shader{"shaders/simple.vert", "shaders/simple.frag"};

  add_texture("diffuse", "res/textures/container2.png");
  add_texture("specular", "res/textures/container2_specular.png");
  add_texture("matrix", "res/textures/matrix.jpg");
  // add_texture("hills", "res/textures/hills.jpg");

  // initial setup
  dir_lights[0] = DirectionalLight{{0.0f, -1.0f, -0.3f}};

  spot_lights[0] = SpotLight{{2.6f, 0.0f, 5.3f}, {0.0f, -1.0f, 0.0f}};
  spot_lights[0].enabled = true;

  point_lights[0] = PointLight{{0.7f, 0.2f, 2.0f}};
  point_lights[1] = PointLight{{2.3f, -3.3f, -4.0f}};
  point_lights[2] = PointLight{{-4.0f, 2.0f, -12.0f}};
  point_lights[3] = PointLight{{0.0f, 0.0f, -3.0f}};

  point_lights[0].enabled = true;
  point_lights[0].color = glm::vec3{0.098f, 1.0f, 0.24f};
  point_lights[0].ambient_strength = 0.054f;
  point_lights[0].diffuse_strength = 9.0f;
  point_lights[0].specular_strength = 2.743f;

  point_lights[3].enabled = true;
  point_lights[3].color = glm::vec3{1.0f, 0.129f, 0.498f};
  point_lights[3].ambient_strength = 0.462f;
  point_lights[3].diffuse_strength = 0.919f;
  point_lights[3].specular_strength = 2.31f;

  backpack = Model{"res/models/backpack/backpack.obj"};
  backpack.has_diffuse = true;
  backpack.has_specular = true;
  backpack.has_emission = true;

  // NDCs -- normals -- texture coords for cube (stride 8 floats)

  const unsigned WIDTH = {40};
  for (unsigned i{10}; i < NUM_CUBES; i++) {
    float x = {(i - 10.0f) / WIDTH};
    float y = {-6.0f};
    unsigned z_step = {(i - 10) % WIDTH};
    float z = {static_cast<float>(z_step)/* z_step + z_step * 0.008f */};
    cube_positions[i] = glm::vec3{x, y, z};
  }

  // // configure the light's VAO
  // light_vao = {sizeof(vertices), 8 * sizeof(float), vertices};
  // VertexArray light_va = {sizeof(vertices), 8 * sizeof(float), vertices};
  // std::memcpy(&light_vao, &light_va, sizeof(Vertex));
  // light_vao.bind();
  // light_vao.data_pointer<float>(3);
  // light_vao.unbind();

  std::vector<unsigned> tex_ids;
  for (auto &&i : active_textures_) {
    tex_ids.push_back((i.second));
  }

  batcher.init();
  batcher.load(tex_ids);
}

// lights

void Stage::apply_spotlight(Stage &stage, const SpotLight &light, unsigned int place) {
  glm::vec3 position = {stage.view * glm::vec4{light.position, 1.0}};
  glm::vec3 light_dir = {glm::normalize(stage.view * glm::vec4{light.direction, 0.0f})};
  glm::vec3 ambient = {light.color * light.ambient_strength};
  glm::vec3 diffuse = {ambient * light.diffuse_strength};
  glm::vec3 specular = {diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  stage.lighting_shader.set_bool("spotLights[" + k + "].enabled", light.enabled);
  stage.lighting_shader.set_float("spotLights[" + k + "].position", position.x, position.y, position.z);
  stage.lighting_shader.set_float("spotLights[" + k + "].direction", light_dir.x, light_dir.y, light_dir.z);
  stage.lighting_shader.set_float("spotLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  stage.lighting_shader.set_float("spotLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  stage.lighting_shader.set_float("spotLights[" + k + "].specular", specular.x, specular.y, specular.z);
  stage.lighting_shader.set_float("spotLights[" + k + "].constant", light.constant);
  stage.lighting_shader.set_float("spotLights[" + k + "].linear", light.linear);
  stage.lighting_shader.set_float("spotLights[" + k + "].quadratic", light.quadratic);
  stage.lighting_shader.set_float("spotLights[" + k + "].cutoff", glm::cos(glm::radians(light.cutoff)));
  stage.lighting_shader.set_float("spotLights[" + k + "].outerCutoff", glm::cos(glm::radians(light.outer_cutoff)));
}

void Stage::apply_pointlight(Stage &stage, const PointLight &light, unsigned int place) {
  glm::vec3 pos{stage.view * glm::vec4{light.position, 1.0}};
  glm::vec3 ambient{light.color * light.ambient_strength};
  glm::vec3 diffuse{ambient * light.diffuse_strength};
  glm::vec3 specular{diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  stage.lighting_shader.set_bool("pointLights[" + k + "].enabled", light.enabled);
  stage.lighting_shader.set_float("pointLights[" + k + "].position", pos.x, pos.y, pos.z);
  stage.lighting_shader.set_float("pointLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  stage.lighting_shader.set_float("pointLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  stage.lighting_shader.set_float("pointLights[" + k + "].specular", specular.x, specular.y, specular.z);
  stage.lighting_shader.set_float("pointLights[" + k + "].constant", light.constant);
  stage.lighting_shader.set_float("pointLights[" + k + "].linear", light.linear);
  stage.lighting_shader.set_float("pointLights[" + k + "].quadratic", light.quadratic);
}

void Stage::apply_directional(Stage &stage, const DirectionalLight &light, unsigned int place) {
  glm::vec3 direction{glm::normalize(stage.view * glm::vec4{light.direction, 0.0f})};
  glm::vec3 ambient{light.color * light.ambient_strength};
  glm::vec3 diffuse{ambient * light.diffuse_strength};
  glm::vec3 specular{diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  stage.lighting_shader.set_bool("dirLights[" + k + "].enabled", light.enabled);
  stage.lighting_shader.set_float("dirLights[" + k + "].direction", direction.x, direction.y, direction.z);
  stage.lighting_shader.set_float("dirLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  stage.lighting_shader.set_float("dirLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  stage.lighting_shader.set_float("dirLights[" + k + "].specular", specular.x, specular.y, specular.z);
}

// shader use

void Stage::use_lighting(Stage &stage) {
  apply_textures();

  stage.lighting_shader.use();
  stage.lighting_shader.set_matrix("projection", stage.projection);
  stage.lighting_shader.set_matrix("view", stage.view);
  stage.lighting_shader.set_bool("emissive", true);
  stage.lighting_shader.set_bool("specular", true);
  stage.lighting_shader.set_bool("diffuse", true);
  stage.lighting_shader.set_float("shininess", 1.0f / material_shininess);
  stage.lighting_shader.set_float("emissionSpeed", emission_speed);
  stage.lighting_shader.set_float("emissionStrength", emission_strength);

  int samplers[192];
  std::iota(std::begin(samplers), std::end(samplers), 0);
  stage.lighting_shader.set_int("textures", sizeof(samplers) / sizeof(int), samplers);
  stage.lighting_shader.set_float("time", static_cast<float>(glfwGetTime()));

  for (size_t i{0}; i < 1; i++) {
    apply_directional(stage, stage.dir_lights[i], i);
  }
  for (size_t i{0}; i < 1; i++) {
    apply_spotlight(stage, stage.spot_lights[i], i);
  }
  for (size_t i{0}; i < 4; i++) {
    apply_pointlight(stage, stage.point_lights[i], i);
  }
}

void Stage::use_true_tone() {
  light_cube_shader.use();
  light_cube_shader.set_matrix("view", view);
  light_cube_shader.set_matrix("projection", projection);
}

// texture application

void Stage::apply_textures() {
  lighting_shader.set_bool("emissive", true);
  lighting_shader.set_bool("specular", true);
  lighting_shader.set_bool("diffuse", true);

  lighting_shader.set_float("material.shininess", 1.0f / material_shininess);
  lighting_shader.set_float("emissionSpeed", emission_speed);
  lighting_shader.set_float("emissionStrength", emission_strength);
}
