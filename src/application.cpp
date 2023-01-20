#define STB_IMAGE_IMPLEMENTATION

#include "application.hpp"
#include "structs.hpp"
#include "light_sources.hpp"
#include "model.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "utils.hpp"
#include "vertex_array.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stb_image.hpp>
#include <glm/glm.hpp>
#include <iostream>

#pragma region GLOBALS
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
const size_t NUM_CUBES = {1210};
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

glm::vec3 cube_positions[NUM_CUBES] = {
    glm::vec3(0.0f, 0.0f, 0.0f),   glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f), glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.2f),  glm::vec3(-1.3f, 1.0f, -1.4f)};
#pragma endregion

// prototypes

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_in, double y_in);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// setup

void apply_spotlight(const SpotLight &light, unsigned int place) {
  glm::vec3 position = {view * glm::vec4{light.position, 1.0}};
  glm::vec3 light_dir = {glm::normalize(view * glm::vec4{light.direction, 0.0f})};
  glm::vec3 ambient = {light.color * light.ambient_strength};
  glm::vec3 diffuse = {ambient * light.diffuse_strength};
  glm::vec3 specular = {diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  lighting_shader.set_bool("spotLights[" + k + "].enabled", light.enabled);
  lighting_shader.set_float("spotLights[" + k + "].position", position.x, position.y, position.z);
  lighting_shader.set_float("spotLights[" + k + "].direction", light_dir.x, light_dir.y, light_dir.z);
  lighting_shader.set_float("spotLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  lighting_shader.set_float("spotLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  lighting_shader.set_float("spotLights[" + k + "].specular", specular.x, specular.y, specular.z);
  lighting_shader.set_float("spotLights[" + k + "].constant", light.constant);
  lighting_shader.set_float("spotLights[" + k + "].linear", light.linear);
  lighting_shader.set_float("spotLights[" + k + "].quadratic", light.quadratic);
  lighting_shader.set_float("spotLights[" + k + "].cutoff", glm::cos(glm::radians(light.cutoff)));
  lighting_shader.set_float("spotLights[" + k + "].outerCutoff", glm::cos(glm::radians(light.outer_cutoff)));
}

void apply_pointlight(const PointLight &light, unsigned int place) {
  glm::vec3 pos{view * glm::vec4{light.position, 1.0}};
  glm::vec3 ambient{light.color * light.ambient_strength};
  glm::vec3 diffuse{ambient * light.diffuse_strength};
  glm::vec3 specular{diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  lighting_shader.set_bool("pointLights[" + k + "].enabled", light.enabled);
  lighting_shader.set_float("pointLights[" + k + "].position", pos.x, pos.y, pos.z);
  lighting_shader.set_float("pointLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  lighting_shader.set_float("pointLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  lighting_shader.set_float("pointLights[" + k + "].specular", specular.x, specular.y, specular.z);
  lighting_shader.set_float("pointLights[" + k + "].constant", light.constant);
  lighting_shader.set_float("pointLights[" + k + "].linear", light.linear);
  lighting_shader.set_float("pointLights[" + k + "].quadratic", light.quadratic);
}

void apply_directional(const DirectionalLight &light, unsigned int place) {
  glm::vec3 direction{glm::normalize(view * glm::vec4{light.direction, 0.0f})};
  glm::vec3 ambient{light.color * light.ambient_strength};
  glm::vec3 diffuse{ambient * light.diffuse_strength};
  glm::vec3 specular{diffuse * light.specular_strength};

  // uniforms
  std::string k{std::to_string(place)};
  lighting_shader.set_bool("dirLights[" + k + "].enabled", light.enabled);
  lighting_shader.set_float("dirLights[" + k + "].direction", direction.x, direction.y, direction.z);
  lighting_shader.set_float("dirLights[" + k + "].ambient", ambient.x, ambient.y, ambient.z);
  lighting_shader.set_float("dirLights[" + k + "].diffuse", diffuse.x, diffuse.y, diffuse.z);
  lighting_shader.set_float("dirLights[" + k + "].specular", specular.x, specular.y, specular.z);
}

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
  float vertices[] = {-0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
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

void use_lighting(unsigned int diffuse, unsigned int specular, unsigned int emission) {
  lighting_shader.use();
  lighting_shader.set_matrix("projection", projection);
  lighting_shader.set_matrix("view", view);

  for (size_t i{0}; i < 1; i++) {
    apply_directional(dir_lights[i], i);
  }
  for (size_t i{0}; i < 1; i++) {
    apply_spotlight(spot_lights[i], i);
  }
  for (size_t i{0}; i < 4; i++) {
    apply_pointlight(point_lights[i], i);
  }

  // material uniforms
  lighting_shader.set_int("material.diffuse", 0);
  lighting_shader.set_int("material.specular", 1);
  lighting_shader.set_int("material.emission", 2);
  lighting_shader.set_float("material.shininess", 1.0f / material_shininess);
  lighting_shader.set_float("emissionSpeed", emission_speed);
  lighting_shader.set_float("emissionStrength", emission_strength);
  lighting_shader.set_float("time", static_cast<float>(glfwGetTime()));
  lighting_shader.set_bool("emissive", true);
  lighting_shader.set_bool("specular", true);
  lighting_shader.set_bool("diffuse", true);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuse);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, specular);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, emission);
}

void stage_setup() {
  glClearColor(clear_.x, clear_.y, clear_.z, 1.0f);
  camera.MovementSpeed = move_speed;
  projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
  view = camera.get_view_matrix();
}

// imgui

void initialize_imgui(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{8.0f, 6.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{10.0f, 16.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 11.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 2.0f);
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void tree_directional(const char *label, DirectionalLight *directional_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&directional_light->enabled));
    ImGui::DragFloat3("Direction", (float *)(&directional_light->direction), 0.01f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Color", (float *)(&directional_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&directional_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&directional_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&directional_light->specular_strength), 0.0f, 10.0f);
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void tree_points(const char *label, PointLight *point_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&point_light->enabled));
    ImGui::DragFloat3("Position", (float *)(&point_light->position), 0.1f);
    ImGui::ColorEdit3("Color", (float *)(&point_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&point_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&point_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&point_light->specular_strength), 0.0f, 10.0f);
    if (ImGui::TreeNode("Attenuation")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! The distance traveled by the light.");
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! Stronger lights have a significantly lower quadratic value.");
      ImGui::SliderFloat("Linear", (float *)(&point_light->linear), 0.0f, 1.0f, "%f");
      ImGui::SliderFloat("Quadratic", (float *)(&point_light->quadratic), 0.0f, 2.0f, "%f");
      ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void tree_spot(const char *label, SpotLight *spot_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&spot_light->enabled));
    ImGui::DragFloat3("Position", (float *)(&spot_light->position), 0.1f);
    ImGui::DragFloat3("Direction", (float *)(&spot_light->direction), 0.01f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Color", (float *)(&spot_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&spot_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&spot_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&spot_light->specular_strength), 0.0f, 10.0f);
    if (ImGui::TreeNode("Attenuation")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! The distance traveled by the light.");
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! Stronger lights have a significantly lower quadratic value.");
      ImGui::SliderFloat("Linear", (float *)(&spot_light->linear), 0.0f, 1.0f, "%f");
      ImGui::SliderFloat("Quadratic", (float *)(&spot_light->quadratic), 0.0f, 2.0f, "%f");
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("Intensity")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! A large difference in values will result in smoother edges.");

      float initial_inner = {spot_light->cutoff};
      float initial_outer = {spot_light->outer_cutoff};

      if (initial_outer < initial_inner) {
        spot_light->outer_cutoff = spot_light->cutoff;
      }
      ImGui::SliderFloat("Inner Cutoff", (float *)(&spot_light->cutoff), 0.0f, 100.0f);
      ImGui::SliderFloat("Outer Cutoff", (float *)(&spot_light->outer_cutoff), 0.0f, 100.0f);
      if (initial_outer < initial_inner) {
        spot_light->cutoff = spot_light->outer_cutoff;
      }
      ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void render_imgui() {
  if (!show_gui)
    return;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // bool show_demo_window = true;
  // ImGui::ShowDemoWindow(&show_demo_window);

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoTitleBar;
  window_flags |= ImGuiWindowFlags_NoBackground;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;

  imgui_hovering = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

  ImGui::Begin("Editor", nullptr, window_flags);

  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: Press C to toggle cursor | WASD to move | SPACE to elevate");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: Arrow Keys & Q / E controls the first spot light");
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  ImGui::Separator();
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "AMBIENT: Anything in range of the light (directional is global)");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "DIFFUSE: Color intensity affected by the angle of the light");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "SPECULAR: Shiny or glosiness effect perceived by a view space angle");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f},
                     "! By Default: All emission is affected by diffuse angle and color per light.");

  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! What may appear to be shadows is linear mipmapping over steel borders.");

  ImGui::Separator();
  ImGui::TextColored(ImVec4{0.8f, 0.2f, 0.4f, 1.0f}, "Camera Pos:  (X) %.1f (Y) %.1f (Z) %.1f ", camera.Position.x,
                     camera.Position.y, camera.Position.z);
  ImGui::ColorEdit3("Clear Color", (float *)&clear_);
  ImGui::SliderFloat("Camera Speed", &move_speed, 0.0f, 50.0f);
  ImGui::SliderFloat("Material Shine", &material_shininess, 0.0f, 1.0f);
  ImGui::SliderFloat("Emission Speed", &emission_speed, 0.0f, 10.0f);
  ImGui::SliderFloat("Emission Strength", &emission_strength, 0.0f, 10.0f);

  ImGui::Separator();
  if (ImGui::CollapsingHeader("Model Properties")) {
    ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f},
                       "! Models won't always contain specified masks; \nBut also won't cause errors if not supported.");
    ImGui::Checkbox("With Diffuse", &backpack.has_diffuse);
    ImGui::Checkbox("With Specular", &backpack.has_specular);
    ImGui::Checkbox("With Emission", &backpack.has_emission);
  }

  if (ImGui::CollapsingHeader("Directional Lighting")) {
    for (size_t i{0}; i < 1; i++) {
      tree_directional(("Directional Light #" + std::to_string(i + 1)).c_str(), &dir_lights[i]);
    }
  }
  if (ImGui::CollapsingHeader("Spot Lighting")) {
    for (size_t i{0}; i < 1; i++) {
      tree_spot(("Spot Light #" + std::to_string(i + 1)).c_str(), &spot_lights[i]);
    }
  }
  if (ImGui::CollapsingHeader("Point Lighting")) {
    for (size_t i{0}; i < 4; i++) {
      tree_points(("Point Light #" + std::to_string(i + 1)).c_str(), &point_lights[i]);
    }
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// rendering

void render_cube(float angle, glm::vec3 position) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
  lighting_shader.set_matrix("model", model);
  lighting_shader.set_matrix("normalView", glm::mat3{glm::transpose(glm::inverse(view * model))});
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void render_lamp(LightSource light, glm::vec3 pos) {
  glm::vec3 color{light.color};
  glm::mat4 model{glm::translate(glm::mat4{1.0f}, pos)};
  model = glm::scale(model, glm::vec3{0.3f});

  light_cube_shader.set_float("lightColor", color.x, color.y, color.z);
  light_cube_shader.set_matrix("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void render_model(Model &obj_model, const Shader &shader, glm::vec3 pos) {
  glm::mat4 model = {glm::translate(glm::mat4{1.0f}, pos)};
  model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
  shader.set_matrix("model", model);
  shader.set_bool("diffuse", obj_model.has_diffuse);
  shader.set_bool("specular", obj_model.has_specular);
  shader.set_bool("emissive", obj_model.has_emission);
  obj_model.draw(shader);
}

// class members

Application *Application::instance_ = {nullptr};

void Application::initialize(unsigned int w, unsigned int h, const char *label) {
  // initialize glfw
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  screen_width, screen_height = w, h;

  // window creation
  window_ = glfwCreateWindow(w, h, label, nullptr, nullptr);
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
  glClearColor(clear_.x, clear_.y, clear_.z, 1.0f);

  initialize_imgui(window_);
  setup();
}

void Application::input() {
  glfwPollEvents();

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
  spot_lights[0].position += light_offset * delta_time_;

  if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_PRESS) {
    if (!can_press)
      return;

    can_press = false;
    show_gui = !show_gui;

    if (show_gui) {
      first_mouse = true;
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
      glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      imgui_hovering = false;
    }
  }

  if (glfwGetKey(window_, GLFW_KEY_C) == GLFW_RELEASE) {
    can_press = true;
  }
}

void Application::update(float delta_time) {
  delta_time_ = delta_time;
  stage_setup();
}

void Application::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // cubes
  use_lighting(diffuse_map, specular_map, emission_map);
  cube_vao.bind();
  for (size_t i{0}; i < NUM_CUBES; i++) {
    // rotate only the first 10 cubes
    float angle{i < 10 ? 20.0f * i + glfwGetTime() / 4 : 0.0f};
    render_cube(angle, cube_positions[i]);
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
    render_lamp(spot_lights[i], spot_lights[i].position);
  }

  // point lights
  for (size_t i{0}; i < 4; i++) {
    if (!point_lights[i].enabled)
      continue;
    render_lamp(point_lights[i], point_lights[i].position);
  }

  render_imgui();

  glfwSwapBuffers(window_);
}

void Application::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

Application *Application::get_instance() {
  return instance_ == nullptr ? instance_ = new Application{} : instance_;
}

bool Application::is_running() const {
  return !glfwWindowShouldClose(window_);
}

// callbacks

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // width and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
  // Application::get_instance()->render();
}

void mouse_callback(GLFWwindow *window, double x_in, double y_in) {
  if (show_gui)
    return;

  float x_pos = static_cast<float>(x_in);
  float y_pos = static_cast<float>(y_in);

  if (first_mouse) {
    last_x = x_pos;
    last_y = y_pos;
    first_mouse = false;
  }

  float x_offset = x_pos - last_x;
  float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

  last_x = x_pos;
  last_y = y_pos;

  camera.process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (imgui_hovering)
    return;

  camera.process_mouse_scroll(static_cast<float>(yoffset));
}