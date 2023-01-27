#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <glm/glm.hpp>
#include <string>

struct Material {
  unsigned diffuse;
  unsigned specular;
  unsigned emission;
};

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
  unsigned tex_index;
  unsigned specular_index;
  unsigned emission_index;
};

struct Texture {
  unsigned id;
  std::string type;
  std::string path;
};

#endif // __VERTEX_H__