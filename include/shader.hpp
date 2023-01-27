#ifndef __SHADER_H__
#define __SHADER_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class Shader {
public:
  Shader() {}
  Shader(const char *vertexPath, const char *fragmentPath);

  void use();

  void set_bool(const std::string &name, bool value) const;
  void set_int(const std::string &name, int value) const;
  void set_int(const std::string &name, int count, int *values) const;
  void set_float(const std::string &name, float v0) const;
  void set_float(const std::string &name, float v0, float v1) const;
  void set_float(const std::string &name, float v0, float v1, float v2) const;
  void set_float(const std::string &name, float v0, float v1, float v2, float v3) const;
  void set_matrix(const std::string &name, const glm::mat4 &matrix) const;
  void set_matrix(const std::string &name, const glm::mat3 &matrix) const;

private:
  unsigned int id_;
};

#endif // __SHADER_H__