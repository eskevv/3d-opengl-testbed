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
  unsigned int emission_nr{1};
  bool has_diffuse = {false};
  bool has_specular = {false};
  bool has_emission = {false};

  for (size_t i{0}; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);

    std::string number{};

    if (textures[i].type == "texture_diffuse") {
      number = std::to_string(diffuse_nr++);
      has_diffuse = true;
    }
    if (textures[i].type == "texture_specular") {
      number = std::to_string(specular_nr++);
      has_specular = true;
    }
    if (textures[i].type == "texture_emission") {
      number = std::to_string(emission_nr++);
      has_emission = true;
    }

    shader.set_int("material." + textures[i].type + number, i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
  }

  // shader.set_bool("emissive", has_emission);
  // shader.set_bool("specular", has_specular);
  // shader.set_bool("diffuse", has_diffuse);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
