#include "model.hpp"
#include "utils.hpp"

Model::Model(const char *path) {
  load_model(path);
}

void Model::draw(const Shader &shader) {
  for (size_t i{0}; i < meshes_.size(); i++) {
    meshes_[i].draw(shader, *this);
  }
}

void Model::load_model(const std::string &path) {
  Assimp::Importer importer{};
  const aiScene *scene = {importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs)};

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    return;
  }

  directory_ = path.substr(0, path.find_last_of('/'));
  process_node(scene->mRootNode, scene);
}

void Model::process_node(const aiNode *node, const aiScene *scene) {
  for (size_t i{0}; i < node->mNumMeshes; i++) {
    aiMesh *mesh = {scene->mMeshes[node->mMeshes[i]]};
    meshes_.push_back(process_mesh(mesh, scene));
  }

  for (size_t i{0}; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

Mesh Model::process_mesh(const aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices{};
  std::vector<unsigned int> indices{};
  std::vector<Texture> textures{};

  aiVector3D *texture_at = {mesh->mTextureCoords[0]};

  for (size_t i{0}; i < mesh->mNumVertices; i++) {
    aiVector3D position_at = {mesh->mVertices[i]};
    aiVector3D normals_at = {mesh->mNormals[i]};

    glm::vec3 position = {position_at.x, position_at.y, position_at.z};
    glm::vec3 normal = {normals_at.x, normals_at.y, normals_at.z};
    glm::vec2 tex_coords = {texture_at ? glm::vec2{texture_at[i].x, texture_at[i].y} : glm::vec2{0}};
    vertices.push_back(Vertex{position, normal, tex_coords});
  }

  for (size_t i{0}; i < mesh->mNumFaces; i++) {
    aiFace face = {mesh->mFaces[i]};
    for (size_t j{0}; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    aiMaterial *material = {scene->mMaterials[mesh->mMaterialIndex]};
    std::vector<Texture> diffuse_maps = {load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse")};
    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
    std::vector<Texture> specular_maps = {load_material_textures(material, aiTextureType_SPECULAR, "texture_specular")};
    textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
  }
  return Mesh{vertices, indices, textures};
}

std::vector<Texture> Model::load_material_textures(const aiMaterial *mat, aiTextureType type, const std::string &type_name) {
  std::vector<Texture> textures{};

  for (size_t i{0}; i < mat->GetTextureCount(type); i++) {
    aiString str{};
    mat->GetTexture(type, i, &str);

    bool skip = {false};

    for (size_t j{0}; j < textures_loaded_.size(); j++) {
      if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
        textures.push_back(textures_loaded_[j]);
        skip = true;
        break;
      }
    }
    if (!skip) {
      unsigned int id = {texture_from_file(str.C_Str(), directory_)};
      Texture texture = {id, type_name, str.C_Str()};
      textures.push_back(texture);
      textures_loaded_.push_back(texture);
    }
  }
  return textures;
}
