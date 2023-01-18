#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <glm/glm.hpp>
#include <string>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coords;
};

struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};

#endif // __VERTEX_H__