#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <glm/glm.hpp>
#include <vector>

class Vertex {
public:
  glm::vec3 position_;
  glm::vec3 normal_;
  glm::vec2 tex_coords_;
};

class Texture;
class Mesh {
public:
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<std::shared_ptr<Texture>> textures_;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<std::shared_ptr<Texture>> textures)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        textures_(std::move(textures)) {
    SetupMesh();
  }
  unsigned int vao_, vbo_, ebo_;

private:
  void SetupMesh();
};

class Model : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeModel;
  }
  std::vector<std::unique_ptr<Mesh>> meshes_;
  std::string directory_;
  Model(Engine* engine, std::string_view path, std::string_view directory)
      : Asset(engine, path), directory_(directory) {}
};
