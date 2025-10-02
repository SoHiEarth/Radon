#pragma once
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
  std::vector<Texture*> textures_;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture*> textures)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        textures_(std::move(textures)) {
    SetupMesh();
  }
  unsigned int vao_, vbo_, ebo_;

private:
  void SetupMesh();
};

class Model {
public:
  std::vector<Mesh*> meshes_;
  const char *path_, *directory_;
  Model(const char* path, const char* directory) {
    path_ = strcpy(new char[strlen(path) + 1], path);
    directory_ = strcpy(new char[strlen(directory) + 1], directory);
  }
};
