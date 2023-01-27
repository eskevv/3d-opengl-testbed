#ifndef __UTILS_H__
#define __UTILS_H__

#include "light_sources.hpp"
#include "model.hpp"
#include <glad/glad.h>
#include <iostream>

auto load_texture(char const *path) -> unsigned int;
auto texture_from_file(const char *path, const std::string &directory) -> unsigned int;

void render_cube(Shader &shader, float angle, glm::vec3 position, glm::mat4 view);
void render_lamp(Shader &shader, LightSource light, glm::vec3 pos);
void render_model(Model &obj_model, const Shader &shader, glm::vec3 pos);

#endif // __UTILS_H__