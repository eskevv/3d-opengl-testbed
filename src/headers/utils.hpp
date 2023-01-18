#ifndef __UTILS_H__
#define __UTILS_H__

#include <glad/glad.h>
#include <stb_image.hpp>
#include <iostream>

unsigned int load_texture(char const *path);
unsigned int texture_from_file(const char *path, const std::string &directory);

#endif // __UTILS_H__