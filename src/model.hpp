#ifndef __MODEL_H__
#define __MODEL_H__

#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <vector>

class Model {
public:
  Model() = default;
  Model(const char *path);

  void draw(const Shader &shader);

private:
  void load_model(const std::string &path);
  void process_node(const aiNode *node, const aiScene *scene);
  Mesh process_mesh(const aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> load_material_textures(const aiMaterial *mat, const aiTextureType type, const std::string &type_name);

  std::vector<Mesh> meshes_;
  std::string directory_;
  std::vector<Texture> textures_loaded_;
};

#endif // __MODEL_H__