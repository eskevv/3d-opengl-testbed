#ifndef __MESH_H__
#define __MESH_H__

#include "structs.hpp"
#include "shader.hpp"

#include <vector>

class Mesh {
public:
   Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures);

   void draw(const Shader &shader);

   std::vector<Vertex> vertices;
   std::vector<unsigned int> indices;
   std::vector<Texture> textures;

private:
   void setup_mesh();

   unsigned int vao_;
   unsigned int vbo_;
   unsigned int ebo_;
};

#endif // __MESH_H__