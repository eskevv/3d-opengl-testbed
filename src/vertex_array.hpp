#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include <glad/glad.h>
#include <iostream>

class VertexArray {
public:
   VertexArray() = default;
   VertexArray(size_t data_size, size_t vertex_size, const void *data) : vertex_size_{vertex_size} {
      glGenVertexArrays(1, &vao_);
      glGenBuffers(1, &vbo_);

      glBindBuffer(GL_ARRAY_BUFFER, vbo_);
      glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }

   ~VertexArray() {
      glDeleteVertexArrays(1, &vao_);
      glDeleteBuffers(1, &vbo_);
   }

   void bind() {
      glBindVertexArray(vao_);
      glBindBuffer(GL_ARRAY_BUFFER, vbo_);
   }

   void unbind() {
      glBindVertexArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }

   template <typename T>
   void push_data(unsigned int elements, bool normalized = false) {
      unsigned int normal_elements{normalized ? GL_TRUE : GL_FALSE};
      unsigned int data_type{get_vertex_type<T>()};
      void *start_point{(void *)(next_start_ * sizeof(T))};

      glVertexAttribPointer(location_, elements, data_type, normal_elements, vertex_size_, start_point);
      glEnableVertexAttribArray(location_++);
      next_start_ += elements;
   }

private:
   template <typename T>
   unsigned int get_vertex_type() {
      if constexpr (std::is_same_v<T, float>) return GL_FLOAT;
      if constexpr (std::is_same_v<T, unsigned int>) return GL_UNSIGNED_INT;
      if constexpr (std::is_same_v<T, unsigned char>) return GL_UNSIGNED_BYTE;

      throw std::runtime_error("The given type for the vao data is unsupported.");
   }

   unsigned int vao_{};
   unsigned int vbo_{};
   unsigned int vertex_size_{};
   unsigned int location_{};
   unsigned int next_start_{};
};

#endif // __VERTEX_ARRAY_H__