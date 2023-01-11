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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// transformations
bool pressed{false};
float light_speed{0.004f};
glm::vec3 light_color{1.0f, 0.86f, 0.73f};
glm::vec3 specular_color{1.0f, 1.0f, 1.0f};
float material_shininess{30.0f};
float ambient_intensity{0.14f};
float diffuse_intensity{3.8f};
float shine_intensity{8.0f};

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// lighting
glm::vec3 lightPos(-0.19f, 1.64f, -4.1f);

int main() {
   // glfw: initialize and configure
   // ------------------------------
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   // glfw window creation
   // --------------------
   GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
   if (window == NULL) {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
   glfwSetCursorPosCallback(window, mouse_callback);
   glfwSetScrollCallback(window, scroll_callback);

   // tell GLFW to capture our mouse
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

   // glad: load all OpenGL function pointers
   // ---------------------------------------
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      return -1;
   }

   // configure global opengl state
   // -----------------------------
   glEnable(GL_DEPTH_TEST);
   glClearColor(0.085f, 0.085f, 0.085f, 1.0f);

   // build and compile our shader zprogram
   // ------------------------------------
   Shader lightingShader("res/shaders/lighting.vert", "res/shaders/lighting.frag");
   Shader lightCubeShader("res/shaders/light.vert", "res/shaders/light.frag");

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

   glm::vec3 cubePositions[] = {glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(2.0f, 5.0f, -15.0f), glm::vec3(-1.5f, -2.2f, -2.5f),
                                glm::vec3(-3.8f, -2.0f, -12.3f), glm::vec3(2.4f, -0.4f, -3.5f), glm::vec3(-1.7f, 3.0f, -7.5f),
                                glm::vec3(1.3f, -2.0f, -2.5f),   glm::vec3(1.5f, 2.0f, -2.5f),  glm::vec3(1.5f, 0.2f, -1.5f),
                                glm::vec3(-1.3f, 1.0f, -1.5f)};

   // first, configure the cube's VAO (and VBO)
   unsigned int VBO, cubeVAO;
   glGenVertexArrays(1, &cubeVAO);
   glGenBuffers(1, &VBO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glBindVertexArray(cubeVAO);

   // position attribute
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
   glEnableVertexAttribArray(0);
   // normal attribute
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   // tex attribute
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
   glEnableVertexAttribArray(2);

   // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
   unsigned int lightCubeVAO;
   glGenVertexArrays(1, &lightCubeVAO);
   glBindVertexArray(lightCubeVAO);

   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   // note that we update the lamp's position attribute's stride to reflect the updated buffer data
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(0 * sizeof(float)));
   glEnableVertexAttribArray(0);

   unsigned int diffuseMap = loadTexture("res/container2.png");
   unsigned int specularMap = loadTexture("res/container2_specular.png");
   unsigned int emissionMap = loadTexture("res/matrix.jpg");

   // DearImGui
   // ---------
   const char *glsl_version = "#version 330";
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO &io = ImGui::GetIO();
   (void)io;
   // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
   // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   // ImGui::StyleColorsLight();

   // Setup Platform/Renderer backends
   ImGui_ImplGlfw_InitForOpenGL(window, true);
   ImGui_ImplOpenGL3_Init(glsl_version);

   // render loop
   // -----------
   while (!glfwWindowShouldClose(window)) {
      // per-frame time logic
      // --------------------
      float currentFrame = static_cast<float>(glfwGetTime());
      delta_time = currentFrame - last_frame;
      last_frame = currentFrame;

      // input
      // -----
      processInput(window);

      // updates
      // -------
      // float dir_x{cos(currentFrame)};
      // float dir_y{sin(currentFrame)};
      // float dir_z{cos(currentFrame)};
      // glm::vec3 direction{glm::normalize(glm::vec3{dir_x, dir_y, 0}) * 0.02f};
      // lightPos += direction;

      // render
      // ------
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // transformations
      glm::mat4 projection = {glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f)};
      glm::mat4 view = camera.get_view_matrix();
      // glm::mat4 model{glm::translate(glm::mat4{1}, glm::vec3{0.0f, 0.0, -4.0f})};
      // model = {glm::rotate(model, (float)(currentFrame / 2), glm::vec3{0.0f, 0.4f, 0.0f})};

      // uniforms
      glm::vec3 lightViewPos{view * glm::vec4{lightPos, 1.0}};
      glm::vec3 ambientColor{light_color * ambient_intensity};
      glm::vec3 diffuseColor{ambientColor * diffuse_intensity};
      specular_color = {diffuseColor * shine_intensity};


      lightingShader.use();
      lightingShader.set_float("light.position", lightViewPos.x, lightViewPos.y, lightViewPos.z);
      lightingShader.set_float("light.ambient", ambientColor.x, ambientColor.y, ambientColor.z);
      lightingShader.set_float("light.diffuse", diffuseColor.x, diffuseColor.y, diffuseColor.z);
      lightingShader.set_float("light.specular", specular_color.x, specular_color.y, specular_color.z);

      lightingShader.set_int("material.diffuse", 0);
      lightingShader.set_int("material.specular", 1);
      lightingShader.set_int("material.emission", 2);
      lightingShader.set_float("material.shininess", material_shininess);
      lightingShader.set_float("time", currentFrame);

      lightingShader.set_matrix("projection", projection);
      lightingShader.set_matrix("view", view);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, diffuseMap);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specularMap);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, emissionMap);

      // render the cube
      glBindVertexArray(cubeVAO);

      // glDrawArrays(GL_TRIANGLES, 0, 36);
      for (unsigned int i = 0; i < 10; i++) {
         float angle = 20.0f * i;
         glm::mat4 model = glm::mat4(1.0f);
         model = glm::translate(model, cubePositions[i]);
         model = glm::rotate(model, angle + currentFrame / 4.0f, glm::vec3(1.0f, 0.3f, 0.5f));
         lightingShader.set_matrix("model", model);
         lightingShader.set_matrix("normalView", glm::mat3{glm::transpose(glm::inverse(view * model))});

         glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      // also draw the lamp object
      lightCubeShader.use();
      glm::mat4 model = glm::translate(glm::mat4{1.0f}, lightPos);
      model = glm::scale(model, glm::vec3{0.1f}); // a smaller cube
      lightCubeShader.set_float("lightColor", light_color.x, light_color.y, light_color.z);
      lightCubeShader.set_matrix("projection", projection);
      lightCubeShader.set_matrix("view", view);
      lightCubeShader.set_matrix("model", model);

      glBindVertexArray(lightCubeVAO);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      // DearImGui
      // ---------
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
      // ImGui!).
      bool show_demo_window = true;
      // ImGui::ShowDemoWindow(&show_demo_window);

      // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
      {
         ImGui::Begin("Light Settings");
         ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "1 to use cursor | 2 to pan | WASD to move | SPACE to elevate");
         ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "Arrow Keys controls light");
         ImGui::DragFloat3("light pos", (float *)(&lightPos), 0.05f); // Edit 3 floats representing a color
         ImGui::ColorEdit3("light color", (float *)(&light_color));   // Edit 3 floats representing a color
         // ImGui::SliderFloat("light speed", &light_speed, 0.002f, 0.080f);
         ImGui::SliderFloat("ambient intensity", &ambient_intensity, 0.0f, 20.0f);
         ImGui::SliderFloat("diffuse intensity", &diffuse_intensity, 0.0f, 20.0f);
         ImGui::SliderFloat("shine intensity", &shine_intensity, 0.0f, 20.0f);
         ImGui::SliderFloat("material shininess", &material_shininess, 0.0f, 80.0f);

         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
         ImGui::End();
      }

      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      // -------------------------------------------------------------------------------
      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   // optional: de-allocate all resources once they've outlived their purpose:
   // ------------------------------------------------------------------------
   glDeleteVertexArrays(1, &cubeVAO);
   glDeleteVertexArrays(1, &lightCubeVAO);
   glDeleteBuffers(1, &VBO);

   // glfw: terminate, clearing all previously allocated GLFW resources.
   // ------------------------------------------------------------------
   ImGui_ImplOpenGL3_Shutdown();
   ImGui_ImplGlfw_Shutdown();
   ImGui::DestroyContext();

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
      light_offset = glm::vec3(-1.0f, 0.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
      light_offset = glm::vec3(1.0f, 0.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
      light_offset = glm::vec3(0.0f, 0.0f, -1.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
      light_offset = glm::vec3(0.0f, 0.0f, 1.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
      light_offset = glm::vec3(0.0f, 1.0f, 0.0f);
   }
   if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      light_offset = glm::vec3(0.0f, -1.0f, 0.0f);
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
   // make sure the viewport matches the new window dimensions; note that width and
   // height will be significantly larger than specified on retina displays.
   glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
   camera.process_mouse_scroll(static_cast<float>(yoffset));
}

// texture loading
unsigned int loadTexture(char const *path) {
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
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      stbi_image_free(data);
   } else {
      std::cout << "Texture failed to load at path: " << path << std::endl;
      stbi_image_free(data);
   }

   return textureID;
}
