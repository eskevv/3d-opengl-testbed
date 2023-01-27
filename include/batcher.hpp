#ifndef __BATCHER_H__
#define __BATCHER_H__

#include "assimp/material.h"
#include "vertex_array.hpp"
#include "vert_geometry.hpp"
#include <cassert>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <map>
#include "structs.hpp"

struct Statistics {
  unsigned flushes = {0};
  unsigned scene_vertices = {0};
  unsigned texture_flushes = {0};
  unsigned unique_textures = {0};
};

class Batcher {
public:
  Batcher(unsigned int w, unsigned int h);

  void init();
  void load(const std::vector<unsigned> &textures);
  void begin(const glm::mat4 &view, const glm::mat4 &proj);
  void end();
  void flush();
  void draw_cube(glm::vec3 pos, Material material);

  auto get_statistics() -> Statistics;

private:
  void ensure_started();
  auto ensure_space(uint32_t vertex_count, uint32_t index_count) -> bool;
  auto use_texture(unsigned texture, unsigned &texture_counter) -> unsigned int;
  void bind_texture(unsigned texture, unsigned slot, bool gl_bind);
  void update_statistics();

  static constexpr unsigned int MAX_VERTICES_ = {8000};
  static constexpr unsigned int MAX_INDICES_ = {MAX_VERTICES_ * 3};
  const unsigned int TEXTURE_UNITS_;

  Vertex vertices_[MAX_VERTICES_];
  uint32_t indices_[MAX_INDICES_];
  bool is_started_;
  VertexArray vao_;
  Statistics statistics_;

  glm::mat4 view_;
  glm::mat4 projection_;

  unsigned int width_;
  unsigned int height_;

  std::map<unsigned, unsigned> bound_textures_;
  std::map<unsigned, unsigned> active_slots_;
  std::vector<unsigned> unique_textures_;

  unsigned flushes_;
  unsigned index_count_;
  unsigned vertex_count_;
  unsigned textures_loaded_;
  unsigned unique_cap_;
};

#endif // __BATCHER_H__