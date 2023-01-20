#include "utils.hpp"
#include "shader.hpp"
#include "light_sources.hpp"
#include "model.hpp"

// #include <GLFW/glfw3.h>
#include <stb_image.hpp>
#include <glm/glm.hpp>

// loading

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

unsigned int texture_from_file(const char *path, const std::string &directory) {
  std::string filename = {std::string(path)};
  filename = directory + '/' + filename;

  unsigned int textureID{};
  glGenTextures(1, &textureID);

  int width{}, height{}, nrComponents{};
  unsigned char *data = {stbi_load(filename.c_str(), &width, &height, &nrComponents, 0)};
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

// rendering

void render_cube(Shader &shader, float angle, glm::vec3 position, glm::mat4 view) {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
  shader.set_matrix("model", model);
  shader.set_matrix("normalView", glm::mat3{glm::transpose(glm::inverse(view * model))});
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void render_lamp(Shader &shader, LightSource light, glm::vec3 pos) {
  glm::vec3 color{light.color};
  glm::mat4 model{glm::translate(glm::mat4{1.0f}, pos)};
  model = glm::scale(model, glm::vec3{0.3f});

  shader.set_float("lightColor", color.x, color.y, color.z);
  shader.set_matrix("model", model);
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

