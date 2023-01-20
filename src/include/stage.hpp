#ifndef __STAGE_H__
#define __STAGE_H__

#include "camera.hpp"
#include "light_sources.hpp"
#include "shader.hpp"
#include "vertex_array.hpp"
#include "model.hpp"
#include "utils.hpp"

#include <stb_image.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

struct Stage {
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

  // opengl
  Shader lighting_shader;
  Shader light_cube_shader;
  VertexArray cube_vao;
  VertexArray light_vao;

  glm::vec3 cube_positions[NUM_CUBES] = {glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                                         glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                                         glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                                         glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                                         glm::vec3(1.5f, 0.2f, -1.2f),   glm::vec3(-1.3f, 1.0f, -1.4f)};

  void setup() {
    stbi_set_flip_vertically_on_load(true);
    // create shader programs
    lighting_shader = Shader{"shaders/lighting.vert", "shaders/lighting.frag"};
    light_cube_shader = Shader{"shaders/light.vert", "shaders/light.frag"};

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

    // NDCs -- normals -- texture coords for cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
        0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

        -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};

    unsigned const int WIDTH{40};
    for (unsigned int i{10}; i < NUM_CUBES; i++) {
      float x{(i - 10) / WIDTH + (i * 0.0005f)};
      float y{-6.0f};
      unsigned int z_step = {(i - 10) % WIDTH};
      float z{z_step + z_step * 0.008f};
      cube_positions[i] = glm::vec3{x, y, z};
    }

    // configure the standard cube VAO
    cube_vao = {sizeof(vertices), 8 * sizeof(float), vertices}; // why
    // VertexArray cube_va = {sizeof(vertices), 8 * sizeof(float), vertices};
    // std::memcpy(&cube_vao, &cube_va, sizeof(VertexArray));
    cube_vao.bind();
    cube_vao.push_data<float>(3);
    cube_vao.push_data<float>(3);
    cube_vao.push_data<float>(2);
    cube_vao.unbind();

    // configure the light's VAO
    light_vao = {sizeof(vertices), 8 * sizeof(float), vertices};
    VertexArray light_va = {sizeof(vertices), 8 * sizeof(float), vertices};
    std::memcpy(&light_vao, &light_va, sizeof(Vertex));
    light_vao.bind();
    light_vao.push_data<float>(3);
    light_vao.unbind();

    diffuse_map = load_texture("res/textures/container2.png");
    specular_map = load_texture("res/textures/container2_specular.png");
    emission_map = load_texture("res/textures/matrix.jpg");
  }

  void update(float delta_time) {
    glClearColor(clear_.x, clear_.y, clear_.z, 1.0f);
    camera.MovementSpeed = move_speed;
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
    view = camera.get_view_matrix();
  }

  void render() {
    // cubes
    use_lighting(*this);
    cube_vao.bind();
    for (size_t i{0}; i < NUM_CUBES; i++) {
      // rotate only the first 10 cubes
      float angle{i < 10 ? 20.0f * i + glfwGetTime() / 4 : 0.0f};
      render_cube(lighting_shader, angle, cube_positions[i], view);
    }

    // model
    render_model(backpack, lighting_shader, glm::vec3{9.0f, 0.0f, 0.0f});

    // lamp objects
    light_vao.bind();
    light_cube_shader.use();
    light_cube_shader.set_matrix("view", view);
    light_cube_shader.set_matrix("projection", projection);

    // spotlights
    for (size_t i{0}; i < 1; i++) {
      if (!spot_lights[i].enabled)
        continue;
      render_lamp(light_cube_shader, spot_lights[i], spot_lights[i].position);
    }

    // point lights
    for (size_t i{0}; i < 4; i++) {
      if (!point_lights[i].enabled)
        continue;
      render_lamp(light_cube_shader, point_lights[i], point_lights[i].position);
    }
  }

  void apply_spotlight(Stage &stage, const SpotLight &light, unsigned int place) {
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

  void apply_pointlight(Stage &stage, const PointLight &light, unsigned int place) {
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

  void apply_directional(Stage &stage, const DirectionalLight &light, unsigned int place) {
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

  void use_lighting(Stage &stage) {
    stage.lighting_shader.use();
    stage.lighting_shader.set_matrix("projection", stage.projection);
    stage.lighting_shader.set_matrix("view", stage.view);

    for (size_t i{0}; i < 1; i++) {
      apply_directional(stage, stage.dir_lights[i], i);
    }
    for (size_t i{0}; i < 1; i++) {
      apply_spotlight(stage, stage.spot_lights[i], i);
    }
    for (size_t i{0}; i < 4; i++) {
      apply_pointlight(stage, stage.point_lights[i], i);
    }

    // material uniforms
    stage.lighting_shader.set_int("material.diffuse", 0);
    stage.lighting_shader.set_int("material.specular", 1);
    stage.lighting_shader.set_int("material.emission", 2);
    stage.lighting_shader.set_float("material.shininess", 1.0f / stage.material_shininess);
    stage.lighting_shader.set_float("emissionSpeed", stage.emission_speed);
    stage.lighting_shader.set_float("emissionStrength", stage.emission_strength);
    stage.lighting_shader.set_float("time", static_cast<float>(glfwGetTime()));
    stage.lighting_shader.set_bool("emissive", true);
    stage.lighting_shader.set_bool("specular", true);
    stage.lighting_shader.set_bool("diffuse", true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stage.diffuse_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, stage.specular_map);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, stage.emission_map);
  }
};

#endif // __STAGE_H__