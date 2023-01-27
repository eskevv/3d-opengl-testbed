#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include <glad/glad.h>
#include <iostream>

class VertexArray {
public:
  VertexArray() = default;

  VertexArray(size_t max_vertex_size, size_t max_index_size, size_t stride) : stride_{stride} {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ibo_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, max_vertex_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_index_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  ~VertexArray() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    glDeleteBuffers(1, &ibo_);
  }

  void bind() {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
  }

  void unbind() {
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  template <typename T> void data_pointer(uint32_t elements, bool normalized = false) {
    int normal_elements = {normalized ? GL_TRUE : GL_FALSE};
    int data_type = {get_vertex_type<T>()};
    void *start_point = {(void *)(next_start_ * sizeof(T))};

    if (data_type == GL_UNSIGNED_INT || data_type == GL_INT || data_type == GL_UNSIGNED_BYTE)
      glVertexAttribIPointer(location_, elements, data_type, stride_, start_point);
    else if (data_type == GL_FLOAT)
      glVertexAttribPointer(location_, elements, data_type, normal_elements, stride_, start_point);

    glEnableVertexAttribArray(location_++);
    next_start_ += elements;
  }

private:
  template <typename T> int get_vertex_type() {
    if constexpr (std::is_same_v<T, float>)
      return GL_FLOAT;
    if constexpr (std::is_same_v<T, int>)
      return GL_INT;
    if constexpr (std::is_same_v<T, unsigned int>)
      return GL_UNSIGNED_INT;
    if constexpr (std::is_same_v<T, unsigned char>)
      return GL_UNSIGNED_BYTE;
    throw std::runtime_error("!unsupported vao data type");
  }

  uint32_t vao_{};
  uint32_t vbo_{};
  uint32_t ibo_{};
  size_t stride_{};
  uint32_t location_{};
  uint32_t next_start_{};
};

#endif // __VERTEX_ARRAY_H__