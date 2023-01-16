#include "mesh.hpp"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures)
    : vertices{vertices}, indices{indices}, textures{textures} {
  setup_mesh();
}

void Mesh::setup_mesh() {
  // generate ids
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);
  // bind the vertex array
  glBindVertexArray(vao_);
  // initialize the vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
  // initialize the index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
  // textures
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tex_coords));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::draw(const Shader &shader) {
  unsigned int diffuse_nr{1};
  unsigned int specular_nr{1};

  for (size_t i{0}; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    bool is_diffuse{textures[i].type == "texture_diffuse"};
    bool is_specular{textures[i].type == "texture_specular"};
    std::string number{is_diffuse ? std::to_string(diffuse_nr++) : is_specular ? std::to_string(specular_nr++) : 0};

    shader.set_int("material." + textures[i].type + number, i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  shader.set_bool("emissive", false);
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
