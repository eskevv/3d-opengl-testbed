#ifndef __LIGHT_SOURCE_H__
#define __LIGHT_SOURCE_H__

#include <glm/glm.hpp>

struct LightSource {
  bool enabled{false};
  glm::vec3 color{1.0f, 1.0f, 1.0f};
  float ambient_strength{0.3f};
  float diffuse_strength{2.0f};
  float specular_strength{7.0f};
};

struct PointLight : LightSource {
  PointLight() {}
  PointLight(glm::vec3 position) : position{position} {}

  glm::vec3 position;
  float constant{1.0f};
  float linear{0.09f};
  float quadratic{0.032f};
};

struct DirectionalLight : LightSource {
  DirectionalLight() {}
  DirectionalLight(glm::vec3 direction) : direction{direction} {}
  glm::vec3 direction;
};

struct SpotLight : LightSource {
  SpotLight() {}
  SpotLight(glm::vec3 position, glm::vec3 direction) : position{position}, direction{direction} {}

  glm::vec3 position;
  glm::vec3 direction;

  float constant{1.0f};
  float linear{0.09f};
  float quadratic{0.032f};

  float cutoff{12.5f};
  float outer_cutoff{13.0f};
};

#endif // __LIGHT_SOURCE_H__