#include "shader.hpp"

unsigned int createShader(const char *code, unsigned int type) {
   int success;
   char infoLog[512];

   unsigned int output = glCreateShader(type);
   glShaderSource(output, 1, &code, nullptr);
   glCompileShader(output);
   glGetShaderiv(output, GL_COMPILE_STATUS, &success);

   if (success == GL_FALSE) {
      glGetShaderInfoLog(output, 512, nullptr, infoLog);
      const char *shaderType = (type == GL_VERTEX_SHADER) ? "VERTEX" : (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" : "UNSUPPORTED";
      std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
   }

   return output;
}

unsigned int createProgram(unsigned int vertex, unsigned int fragment) {
   unsigned int id = glCreateProgram();
   glAttachShader(id, vertex);
   glAttachShader(id, fragment);
   glLinkProgram(id);

   int success;
   char infoLog[512];

   glGetProgramiv(id, GL_LINK_STATUS, &success);
   if (success == GL_FALSE) {
      glGetProgramInfoLog(id, 512, nullptr, infoLog);
      std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
   }

   glDeleteShader(vertex);
   glDeleteShader(fragment);

   return id;
}

const std::string parseShaderCode(const char *shaderPath) {
   std::ifstream shaderFile;
   std::stringstream shaderStream;

   shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

   try {
      shaderFile.open(shaderPath);
      shaderStream << shaderFile.rdbuf();
      shaderFile.close();

   } catch (std::ifstream::failure e) {
      std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
   }

   return shaderStream.str();
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
   // Parse
   const std::string vShaderCode = parseShaderCode(vertexPath);
   const std::string fShaderCode = parseShaderCode(fragmentPath);

   // Create
   unsigned int vertex = createShader(vShaderCode.c_str(), GL_VERTEX_SHADER);
   unsigned int fragment = createShader(fShaderCode.c_str(), GL_FRAGMENT_SHADER);
   id_ = createProgram(vertex, fragment);
}

void Shader::use() {
   glUseProgram(id_);
}

void Shader::set_bool(const std::string &name, bool value) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform1i(location, (int)value);
}

void Shader::set_int(const std::string &name, int value) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform1i(location, value);
}

void Shader::set_int(const std::string &name, int count, int *values) const
{
  unsigned int location = glGetUniformLocation(id_, name.c_str());
  glUniform1iv(location, count, values);
}

void Shader::set_float(const std::string &name, float v0) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform1f(location, v0);
}

void Shader::set_float(const std::string &name, float v0, float v1) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform2f(location, v0, v1);
}

void Shader::set_float(const std::string &name, float v0, float v1, float v2) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform3f(location, v0, v1, v2);
}

void Shader::set_float(const std::string &name, float v0, float v1, float v2, float v3) const {
   unsigned int location = glGetUniformLocation(id_, name.c_str());
   glUniform4f(location, v0, v1, v2, v3);
}

void Shader::set_matrix(const std::string &name, const glm::mat4 &matrix) const
{
   unsigned int transform_loc = glGetUniformLocation(id_, name.c_str());
   glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::set_matrix(const std::string &name, const glm::mat3 &matrix) const
{
   unsigned int transform_loc = glGetUniformLocation(id_, name.c_str());
   glUniformMatrix3fv(transform_loc, 1, GL_FALSE, glm::value_ptr(matrix));
}
