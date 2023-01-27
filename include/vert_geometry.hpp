#ifndef __VERT_GEOMETRY_H__
#define __VERT_GEOMETRY_H__

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <structs.hpp>

class VertGeometry {
public:
  VertGeometry() = default;

  auto create_cube(glm::mat4 transform, glm::mat4 view, unsigned int tex_id) -> std::array<Vertex, 36> {
    const uint16_t VERTS = {36};

    std::array<Vertex, VERTS> output{};
    std::array<glm::vec3, VERTS> pos_coords = cube_positions();
    std::array<glm::vec3, VERTS> norm_coords = cube_normals();
    std::array<glm::vec2, VERTS> tex_coords = cube_tex();

    auto normal_model_view = glm::mat3{glm::transpose(glm::inverse(view * transform))};

    for (size_t i{0}; i < VERTS; i++) {
      auto position = glm::vec3{transform * glm::vec4{pos_coords[i], 1.0f}};
      auto normal = normal_model_view * norm_coords[i];
      auto tex = tex_coords[i];

      output[i] = Vertex{position, normal, tex, tex_id};
    }

    return output;
  }

  auto cubes_indeces(uint32_t last) -> std::array<uint32_t, 36> {
    std::array<uint32_t, 36> output{};

    for (size_t i{0}; i < output.size(); i++) {
      output[i] = last + i;
    }

    return output;
  }

private:
  auto cube_positions() -> std::array<glm::vec3, 36> {
    return {glm::vec3{-0.5f, -(-0.5), -0.5f}, glm::vec3{0.5f, -(-0.5), -0.5f}, glm::vec3{0.5f, -(0.5), -0.5f},
            glm::vec3{0.5f, -(0.5), -0.5f},   glm::vec3{-0.5f, -(0.5), -0.5f}, glm::vec3{-0.5f, -(-0.5), -0.5f},
            glm::vec3{-0.5f, -(-0.5), 0.5f},  glm::vec3{0.5f, -(-0.5), 0.5f},  glm::vec3{0.5f, -(0.5), 0.5f},
            glm::vec3{0.5f, -(0.5), 0.5f},    glm::vec3{-0.5f, -(0.5), 0.5f},  glm::vec3{-0.5f, -(-0.5), 0.5f},
            glm::vec3{-0.5f, -(0.5), 0.5f},   glm::vec3{-0.5f, -(0.5), -0.5f}, glm::vec3{-0.5f, -(-0.5), -0.5f},
            glm::vec3{-0.5f, -(-0.5), -0.5f}, glm::vec3{-0.5f, -(-0.5), 0.5f}, glm::vec3{-0.5f, -(0.5), 0.5f},
            glm::vec3{0.5f, -(0.5), 0.5f},    glm::vec3{0.5f, -(0.5), -0.5f},  glm::vec3{0.5f, -(-0.5), -0.5f},
            glm::vec3{0.5f, -(-0.5), -0.5f},  glm::vec3{0.5f, -(-0.5), 0.5f},  glm::vec3{0.5f, -(0.5), 0.5f},
            glm::vec3{-0.5f, -(-0.5), -0.5f}, glm::vec3{0.5f, -(-0.5), -0.5f}, glm::vec3{0.5f, -(-0.5), 0.5f},
            glm::vec3{0.5f, -(-0.5), 0.5f},   glm::vec3{-0.5f, -(-0.5), 0.5f}, glm::vec3{-0.5f, -(-0.5), -0.5f},
            glm::vec3{-0.5f, -(0.5), -0.5f},  glm::vec3{0.5f, -(0.5), -0.5f},  glm::vec3{0.5f, -(0.5), 0.5f},
            glm::vec3{0.5f, -(0.5), 0.5f},    glm::vec3{-0.5f, -(0.5), 0.5f},  glm::vec3{-0.5f, -(0.5), -0.5f}};
  }

  auto cube_normals() -> std::array<glm::vec3, 36> {
    return {
        glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f},
        glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, -1.0f}, glm::vec3{0.0f, 0.0f, 1.0f},  glm::vec3{0.0f, 0.0f, 1.0f},
        glm::vec3{0.0f, 0.0f, 1.0f},  glm::vec3{0.0f, 0.0f, 1.0f},  glm::vec3{0.0f, 0.0f, 1.0f},  glm::vec3{0.0f, 0.0f, 1.0f},
        glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f},
        glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{-1.0f, 0.0f, 0.0f}, glm::vec3{1.0f, 0.0f, 0.0f},  glm::vec3{1.0f, 0.0f, 0.0f},
        glm::vec3{1.0f, 0.0f, 0.0f},  glm::vec3{1.0f, 0.0f, 0.0f},  glm::vec3{1.0f, 0.0f, 0.0f},  glm::vec3{1.0f, 0.0f, 0.0f},
        glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f},
        glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, -1.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f},  glm::vec3{0.0f, 1.0f, 0.0f},
        glm::vec3{0.0f, 1.0f, 0.0f},  glm::vec3{0.0f, 1.0f, 0.0f},  glm::vec3{0.0f, 1.0f, 0.0f},  glm::vec3{0.0f, 1.0f, 0.0f}};
  }

  auto cube_tex() -> std::array<glm::vec2, 36> {
    return {glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
            glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{1.0f, 1.0f},
            glm::vec2{0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{0.0f, 1.0f},
            glm::vec2{0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 1.0f},
            glm::vec2{0.0f, 1.0f}, glm::vec2{0.0f, 1.0f}, glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{0.0f, 1.0f},
            glm::vec2{1.0f, 1.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 1.0f},
            glm::vec2{0.0f, 1.0f}, glm::vec2{1.0f, 1.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{1.0f, 0.0f}, glm::vec2{0.0f, 0.0f},
            glm::vec2{0.0f, 1.0f}};
  }
};

#endif // __VERT_GEOMETRY_H__