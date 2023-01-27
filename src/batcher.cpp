#include "batcher.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "structs.hpp"
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <algorithm>
#include <numeric>
#include <vector>
#include "structs.hpp"

auto max_units() -> unsigned { // utility function since gl takes a pointer instead of returning
  int max_units = {0};
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_units);
  return max_units;
}

Batcher::Batcher(unsigned w, unsigned h)
    : width_{w}, height_{h}, vao_{sizeof(Vertex) * MAX_VERTICES_, sizeof(unsigned) * MAX_INDICES_, sizeof(Vertex)},
      TEXTURE_UNITS_{max_units()} {}

void Batcher::update_statistics() {
  statistics_.flushes = flushes_ + 1;
  statistics_.unique_textures = unique_textures_.size();
  statistics_.scene_vertices = (flushes_ + 1) * MAX_VERTICES_ + vertex_count_;
}

void Batcher::init() {
  vao_.bind();
  vao_.data_pointer<float>(3);    // position
  vao_.data_pointer<float>(3);    // normals
  vao_.data_pointer<float>(2);    // tex coords
  vao_.data_pointer<unsigned>(1); // tex id
  vao_.data_pointer<unsigned>(1); // tex id
  vao_.data_pointer<unsigned>(1); // tex id

  printf("Hardware - Maximum Texture Units: %i\n", TEXTURE_UNITS_);
}

void Batcher::begin(const glm::mat4 &view, const glm::mat4 &proj) {
  view_ = view;
  projection_ = proj;
  is_started_ = true;
  unique_textures_.clear();
  flushes_ = 0;
  unique_cap_ = TEXTURE_UNITS_;
}

void Batcher::end() {
  update_statistics();
  flush();
  is_started_ = false;
}

// preload as many textures into memory and assign each one a texture slot
void Batcher::load(const std::vector<unsigned> &textures) {
  textures_loaded_ = textures.size();
  unsigned slot = {0};
  bool gl_bind = {true};
  for (auto &&texture : textures) {
    bind_texture(texture, slot, gl_bind);

    if (++slot >= TEXTURE_UNITS_) {
      slot = 0;
      gl_bind = false;
    }
  }
}

void Batcher::flush() {
  ensure_started();

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertex_count_, vertices_);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned) * index_count_, indices_);
  glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);

  flushes_++;
  index_count_ = 0;
  vertex_count_ = 0;
}

void Batcher::ensure_started() {
  if (!is_started_)
    throw std::runtime_error("!batcher never began");
}

auto Batcher::ensure_space(unsigned vertex_count, unsigned index_count) -> bool {
  if (vertex_count > MAX_VERTICES_)
    throw std::runtime_error("!too many vertices pushed at once");
  if (index_count > MAX_INDICES_)
    throw std::runtime_error("!too many indices pushed at once");

  bool max_vertices_reached = vertex_count_ + vertex_count >= MAX_VERTICES_;
  bool max_indices_reached = index_count_ + index_count >= MAX_INDICES_;

  if (max_vertices_reached || max_indices_reached) {
    flush();
    return false;
  }
  return true;
}

// texture binding

void Batcher::bind_texture(unsigned texture, unsigned slot, bool gl_bind) {
  if (gl_bind) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
    active_slots_[slot] = texture;
  }
  bound_textures_[texture] = slot;
}

auto Batcher::use_texture(unsigned texture, unsigned &texture_counter) -> unsigned {
  if (std::find(unique_textures_.begin(), unique_textures_.end(), texture) == unique_textures_.end()) {
    unique_textures_.push_back(texture);
  }

  // very convinient utility to maximize performance when graphics have already been drawn due to vertex / index overflow
  // even if the max slots were reached you shouldn't blit agin
  if (texture_counter++ >= TEXTURE_UNITS_ && unique_textures_.size() >= unique_cap_) {
    flush();
    unique_cap_ += TEXTURE_UNITS_;
    texture_counter = 0;
  }

  unsigned slot = {bound_textures_[texture]};
  bool check_slot = {textures_loaded_ - TEXTURE_UNITS_ < slot};

  if (check_slot && active_slots_[slot] != texture) {
    bind_texture(texture, slot, true);
  }

  return bound_textures_[texture];
}

// rendering
// sample usage of textures with simple cube
void Batcher::draw_cube(glm::vec3 pos, Material material) {
  bool flushed = !ensure_space(36, 36);
  unsigned texture_counter = {flushed ? 0 : TEXTURE_UNITS_}; // extremely rare; here if a single object exceeds max bind slots
  unsigned texture_diffuse = use_texture(material.diffuse, texture_counter);
  unsigned texture_specular = use_texture(material.specular, texture_counter);
  unsigned texture_emission = use_texture(material.emission, texture_counter);

  Vertex vertices[] = {
      Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, texture_diffuse, texture_specular, texture_emission},
      Vertex{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}, texture_diffuse, texture_specular, texture_emission}};

  // vertex calculations
  glm::mat4 transform = {glm::translate(glm::mat4{1}, pos)};
  for (size_t i = {0}; i < 36; i++) {
    vertices[i].position = glm::vec3{transform * glm::vec4{vertices[i].position, 1.0}};
  }

  // index calculations
  unsigned int indices[36];
  std::iota(std::begin(indices), std::end(indices), index_count_);

  memcpy(vertices_ + vertex_count_, std::begin(vertices), sizeof(vertices));
  memcpy(indices_ + index_count_, std::begin(indices), sizeof(indices));

  // std::array<Vertex, 36> verts = {std::move(vert_geo_.create_cube(transform, view_, 0))};
  // std::array<uint32_t, 36> indices = {vert_geo_.cubes_indeces(index_count_)};

  index_count_ += 36;
  vertex_count_ += 36;
}

auto Batcher::get_statistics() -> Statistics {
  return statistics_;
}