#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

void release_imgui();
void render_imgui();
void release_glfw();
void deallocate_gl();
void initialize_testbed();
void initialize_imgui();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int load_texture(const char *path);
void use_lighting(unsigned int diffuse, unsigned int specular, unsigned int emission);
void render_cube(float angle, glm::vec3 position);
void stage_setup();

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;
GLFWwindow *window;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// transformations
bool pressed{false};
float light_speed{0.004f};
glm::vec3 light_color{1.0f, 0.86f, 0.73f};
float material_shininess{0.3f};
float ambient_intensity{0.3f};
float diffuse_intensity{3.1f};

bool directional_light_on{true};
glm::vec3 directional_color{1.0f};
float directional_ambient_i{0.2f};
float directional_diffuse_i{0.6f};
float directional_specular_i{0.5f};
glm::vec3 light_direction{0.2f, -1.0f, -1.0f};
glm::vec3 directional_light;
glm::vec3 directional_ambient;
glm::vec3 directional_diffuse;
glm::vec3 directional_specular;

float spec_intensity{1.0f};
float move_speed{12.0f};
float emission_speed{0.33f};

bool point_light_states[4]{true, true, true, true};
glm::vec3 point_light_colors[4]{{1, 1, 1}, {1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
bool spot_light_on{true};
glm::vec3 pointLightPositions[] = {glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(-4.0f, 2.0f, -12.0f),
                                   glm::vec3(0.0f, 0.0f, -3.0f)};

unsigned int VBO{}, cubeVAO, lightCubeVAO{};

Shader lightingShader;
Shader lightCubeShader;
glm::mat4 projection;
glm::mat4 view;

glm::vec3 lightViewPos;
glm::vec3 lightViewPoint;
glm::vec3 lightFront;
glm::vec3 ambientColor;
glm::vec3 diffuseColor;
glm::vec3 specular_color;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// lighting
glm::vec3 lightPos(-1.7f, 5.5f, -7.5f);

int main() {
   // initialization steps
   initialize_testbed();
   initialize_imgui();

   // create shader programs
   lightingShader = {"res/shaders/lighting.vert", "res/shaders/lighting.frag"};
   lightCubeShader = {"res/shaders/light.vert", "res/shaders/light.frag"};

   // set up vertex data (and buffer(s)) and configure vertex attributes
   // ------------------------------------------------------------------
   float vertices[] = {// positions          // normals           // texture coords
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

   const size_t NUM_CUBES{1210};
   glm::vec3 cubePositions[NUM_CUBES] = {glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                                         glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                                         glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                                         glm::vec3(-1.3f, 1.0f, -1.5f)};

   unsigned const int WIDTH{40};
   for (size_t i{10}; i < NUM_CUBES; i++) {
      float x{(i - 10) / WIDTH};
      float y{-6.0f};
      float z{(i - 10) % WIDTH};
      glm::vec3 pos{x, y, z};
      cubePositions[i] = pos;
   }

   // cube vao
   glGenVertexArrays(1, &cubeVAO);
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glBindVertexArray(cubeVAO);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   // configure the light's VAO
   glGenVertexArrays(1, &lightCubeVAO);
   glBindVertexArray(lightCubeVAO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
   glEnableVertexAttribArray(0);

   unsigned int diffuseMap = load_texture("res/container2.png");
   unsigned int specularMap = load_texture("res/container2_specular.png");
   unsigned int emissionMap = load_texture("res/matrix.jpg");

   // render loop
   // -----------
   while (!glfwWindowShouldClose(window)) {
      // per-frame time logic
      // --------------------
      float currentFrame = static_cast<float>(glfwGetTime());
      delta_time = currentFrame - last_frame;
      last_frame = currentFrame;
      camera.MovementSpeed = move_speed;

      // input
      // -----
      processInput(window);

      // updates
      // -------
      stage_setup();

      // render
      // ------
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // cubes
      use_lighting(diffuseMap, specularMap, emissionMap);
      glBindVertexArray(cubeVAO);
      for (unsigned int i = 0; i < NUM_CUBES; i++) {
         float angle = i < 10 ? 20.0f * i + currentFrame / 4 : 0.0f;
         render_cube(angle, cubePositions[i]);
      }

      // lamp objects
      lightCubeShader.use();
      lightCubeShader.set_matrix("projection", projection);
      lightCubeShader.set_matrix("view", view);
      glm::mat4 model = glm::translate(glm::mat4{1.0f}, lightPos);
      model = glm::scale(model, glm::vec3{0.1f});
      lightCubeShader.set_matrix("model", model);
      // glBindVertexArray(lightCubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      for (size_t i{0}; i < 4; i++) {
         if (!point_light_states[i]) continue;
         glm::vec3 color{point_light_colors[i]};
         lightCubeShader.set_float("lightColor", color.x, color.y, color.z);
         glm::mat4 model{glm::translate(glm::mat4{1.0f}, pointLightPositions[i])};
         model = glm::scale(model, glm::vec3{0.3f});
         lightCubeShader.set_matrix("model", model);
         glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      render_imgui();

      // glfw: swap buffers and poll IO events
      // -------------------------------------
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   // deallocate resources
   // --------------------
   deallocate_gl();
   release_imgui();
   release_glfw();

   return 0;
}

// resources
// -------------------------------------------
void release_imgui() {
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();
}

void release_glfw() {
   glfwDestroyWindow(window);
   glfwTerminate();
}

void deallocate_gl() {
   glDeleteVertexArrays(1, &cubeVAO);
   glDeleteVertexArrays(1, &lightCubeVAO);
   glDeleteBuffers(1, &VBO);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame
// -----------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, true);
   }
   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      camera.process_keyboard(FORWARD, delta_time);
   }
   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      camera.process_keyboard(BACKWARD, delta_time);
   }
   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      camera.process_keyboard(LEFT, delta_time);
   }
   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      camera.process_keyboard(RIGHT, delta_time);
   }
   if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      camera.process_keyboard(UP, delta_time);
   }

   glm::vec3 light_offset{0};
   if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
      light_offset += glm::vec3(-1.0f, 0.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      light_offset += glm::vec3(1.0f, 0.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      light_offset += glm::vec3(0.0f, 0.0f, -1.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      light_offset += glm::vec3(0.0f, 0.0f, 1.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      light_offset += glm::vec3(0.0f, 1.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      light_offset += glm::vec3(0.0f, -1.0f, 0.0f);
   }
   lightPos += light_offset * delta_time;

   if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      pressed = true;
      firstMouse = true;
   }
   if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      pressed = false;
   }
}

// glfw: callbacks
// -----------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
   // make sure the viewport matches the new window dimensions; note that width and
   // height will be significantly larger than specified on retina displays.
   glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
   if (pressed) return;

   float xpos = static_cast<float>(xposIn);
   float ypos = static_cast<float>(yposIn);

   if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
   }

   float xoffset = xpos - lastX;
   float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

   lastX = xpos;
   lastY = ypos;

   camera.process_mouse_movement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
   camera.process_mouse_scroll(static_cast<float>(yoffset));
}

// rendering
// -----------------------
void render_cube(float angle, glm::vec3 position) {
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, position);
   model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
   lightingShader.set_matrix("model", model);
   lightingShader.set_matrix("normalView", glm::mat3{glm::transpose(glm::inverse(view * model))});
   glDrawArrays(GL_TRIANGLES, 0, 36);
}

void header_point(const char *label, glm::vec3 *pos, glm::vec3 *color, float *ambient, float *diffuse, float *specular, bool *on) {
   if (ImGui::CollapsingHeader(label)) {
      ImGui::Checkbox("Enabled", on);
      ImGui::DragFloat3("Position", (float *)(pos), 0.1f);
      ImGui::ColorEdit3("Color", (float *)(color));
      ImGui::SliderFloat("Ambient Strength", ambient, 0.0f, 1.0f);
      ImGui::SliderFloat("Diffuse Strength", diffuse, 0.0f, 1.0f);
      ImGui::SliderFloat("Specular Strength", specular, 0.0f, 1.0f);
   }
}

void header_dir(const char *label, glm::vec3 *dir, glm::vec3 *color, float *ambient, float *diffuse, float *specular, bool *on) {
   if (ImGui::CollapsingHeader(label)) {
      ImGui::Checkbox("Enabled", on);
      ImGui::DragFloat3("Direction", (float *)(dir), 0.1f);
      ImGui::ColorEdit3("Color", (float *)(color));
      ImGui::SliderFloat("Ambient Strength", ambient, 0.0f, 1.0f);
      ImGui::SliderFloat("Diffuse Strength", diffuse, 0.0f, 10.0f);
      ImGui::SliderFloat("Specular Strength", specular, 0.0f, 20.0f);
   }
}

void header_spot(const char *label, glm::vec3 *pos, glm::vec3 *dir, glm::vec3 *color, float *ambient, float *diffuse, float *specular, bool *on) {
   if (ImGui::CollapsingHeader(label)) {
      ImGui::Checkbox("Enabled", on);
      ImGui::DragFloat3("Position", (float *)(pos), 0.1f);
      ImGui::DragFloat3("Direction", (float *)(dir), 0.1f);
      ImGui::ColorEdit3("Color", (float *)(color));
      ImGui::SliderFloat("Ambient Strength", ambient, 0.0f, 1.0f);
      ImGui::SliderFloat("Diffuse Strength", diffuse, 0.0f, 1.0f);
      ImGui::SliderFloat("Specular Strength", specular, 0.0f, 1.0f);
   }
}

void render_imgui() {
   ImGui_ImplOpenGL3_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();

   // bool show_demo_window = true;
   // ImGui::ShowDemoWindow(&show_demo_window);

   ImGui::Begin("Editor");
   if (ImGui::CollapsingHeader("Help")) {
      ImGui::Text("PROGRAMMER GUIDE:");
      ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
      ImGui::BulletText("See comments in imgui.cpp.");
      ImGui::BulletText("See example applications in the examples/ folder.");
      ImGui::BulletText("Read the FAQ at http://www.dearimgui.org/faq/");
      ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
      ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");
      ImGui::Separator();

      ImGui::Text("USER GUIDE:");
      ImGui::ShowUserGuide();
   }

   ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: 1 to use cursor | 2 to pan | WASD to move | SPACE to elevate");
   ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: Arrow Keys controls the spot light");
   ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: What may appear to be shadows is linear mipmapping over steel borders.");
   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

   ImGui::Separator();
   ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "AMBIENT: Anything in range of the light (directional is global)");
   ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "DIFFUSE: Color intensity affected by the angle of the light");
   ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "SPECULAR: Shiny or glosiness effect perceived by a view space angle");

   ImGui::Separator();
   ImGui::SliderFloat("Camera Speed", &move_speed, 0.0f, 50.0f);
   ImGui::SliderFloat("Material Shine", &material_shininess, 0.0f, 5.0f);

   header_dir("Directional Lighting", &light_direction, &directional_color, &directional_ambient_i, &directional_diffuse_i, &directional_specular_i,
              &directional_light_on);

   header_point("Point Light #1", &pointLightPositions[0], &point_light_colors[0], &ambient_intensity, &diffuse_intensity, &spec_intensity,
                &point_light_states[0]);

   header_point("Point Light #2", &pointLightPositions[1], &point_light_colors[1], &ambient_intensity, &diffuse_intensity, &spec_intensity,
                &point_light_states[1]);

   header_point("Point Light #3", &pointLightPositions[2], &point_light_colors[2], &ambient_intensity, &diffuse_intensity, &spec_intensity,
                &point_light_states[2]);

   header_point("Point Light #4", &pointLightPositions[3], &point_light_colors[3], &ambient_intensity, &diffuse_intensity, &spec_intensity,
                &point_light_states[3]);

   header_spot("Spot Light", &lightPos, &lightFront, &light_color, &ambient_intensity, &diffuse_intensity, &spec_intensity, &spot_light_on);

   ImGui::End();

   ImGui::Render();
   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// setup
// -----------------------
unsigned int load_texture(char const *path) {
   unsigned int textureID;
   glGenTextures(1, &textureID);

   int width, height, nrComponents;
   unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
   if (data) {
      GLenum format;
      if (nrComponents == 1)
         format = GL_RED;
      else if (nrComponents == 3)
         format = GL_RGB;
      else if (nrComponents == 4)
         format = GL_RGBA;

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      stbi_image_free(data);
   } else {
      std::cout << "Texture failed to load at path: " << path << std::endl;
      stbi_image_free(data);
   }

   return textureID;
}

void stage_setup() {
   projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
   view = camera.get_view_matrix();

   // directional
   directional_light = {glm::normalize(view * glm::vec4{light_direction, 0.0f})};
   directional_ambient = {directional_color * directional_ambient_i};
   directional_diffuse = {directional_ambient * directional_diffuse_i};
   directional_specular = {directional_diffuse * directional_specular_i};

   // spotlight
   lightViewPos = {view * glm::vec4{lightPos, 1.0}};
   lightViewPoint = {view * glm::vec4{lightPos + glm::vec3{0.0, 1.0, 0.0}, 1.0}};
   lightFront = {glm::normalize(lightViewPoint - lightViewPos)};
   ambientColor = {glm::vec3{0.3f} * light_color * ambient_intensity};
   diffuseColor = {ambientColor * diffuse_intensity};
   specular_color = {diffuseColor * spec_intensity};
}

void use_lighting(unsigned int diffuse, unsigned int specular, unsigned int emission) {
   // set uniforms
   lightingShader.use();
   lightingShader.set_matrix("projection", projection);
   lightingShader.set_matrix("view", view);
   // directional uniforms
   lightingShader.set_float("dirLight.direction", directional_light.x, directional_light.y, directional_light.z);
   lightingShader.set_float("dirLight.ambient", directional_ambient.x, directional_ambient.y, directional_ambient.z);
   lightingShader.set_float("dirLight.diffuse", directional_diffuse.x, directional_diffuse.y, directional_diffuse.z);
   lightingShader.set_float("dirLight.specular", directional_specular.x, directional_specular.y, directional_specular.z);
   // spotlight uniforms
   lightingShader.set_float("spotLight.position", lightViewPos.x, lightViewPos.y, lightViewPos.z);
   lightingShader.set_float("spotLight.direction", lightFront.x, lightFront.y, lightFront.z);
   lightingShader.set_float("spotLight.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
   lightingShader.set_float("spotLight.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
   lightingShader.set_float("spotLight.specular", specular_color.x, specular_color.y, specular_color.z);
   lightingShader.set_float("spotLight.constant", 1.0f);
   lightingShader.set_float("spotLight.linear", 0.09f);
   lightingShader.set_float("spotLight.quadratic", 0.032f);
   lightingShader.set_float("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
   lightingShader.set_float("spotLight.outerCutoff", glm::cos(glm::radians(13.0f)));
   // point uniforms
   for (size_t i{0}; i < 4; i++) {
      std::string k{std::to_string(i)};
      glm::vec3 pos{view * glm::vec4{pointLightPositions[i], 1.0}};
      glm::vec3 color{point_light_colors[i] * ambient_intensity};
      lightingShader.set_float("pointLights[" + k + "].position", pos.x, pos.y, pos.z);
      lightingShader.set_float("pointLights[" + k + "].ambient", color.x, color.y, color.z);
      lightingShader.set_float("pointLights[" + k + "].diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
      lightingShader.set_float("pointLights[" + k + "].specular", specular_color.x, specular_color.y, specular_color.z);
      lightingShader.set_float("pointLights[" + k + "].constant", 1.0f);
      lightingShader.set_float("pointLights[" + k + "].linear", 0.09f);
      lightingShader.set_float("pointLights[" + k + "].quadratic", 0.032f);
   }
   // material uniforms
   lightingShader.set_int("material.diffuse", 0);
   lightingShader.set_int("material.specular", 1);
   lightingShader.set_int("material.emission", 2);
   lightingShader.set_float("material.shininess", 1.0f / material_shininess);
   lightingShader.set_float("emissionSpeed", emission_speed);
   lightingShader.set_float("time", static_cast<float>(glfwGetTime()));

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, diffuse);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D, specular);
   glActiveTexture(GL_TEXTURE2);
   glBindTexture(GL_TEXTURE_2D, emission);
}

void initialize_testbed() {
   // initialize glfw
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // window creation
   window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL TestBed", NULL, NULL);
   if (window == nullptr) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
   }

   // configure glfw
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetCursorPosCallback(window, mouse_callback);
   glfwSetScrollCallback(window, scroll_callback);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // load all OpenGL function pointers
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
   }

   // configure global opengl state
   glEnable(GL_DEPTH_TEST);
   glClearColor(0.085f, 0.085f, 0.085f, 1.0f);
}

void initialize_imgui() {
   // DearImGui
   // ---------
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   (void)io;

   ImGui::StyleColorsClassic();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init("#version 330");
}
