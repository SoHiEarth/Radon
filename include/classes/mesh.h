#pragma once
#include <vector>
#include <glm/glm.hpp>

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
  Mesh(std::vector<Vertex> vertices,
       std::vector<unsigned int> indices,
       std::vector<Texture*> textures)
      : vertices_(std::move(vertices)),
        indices_(std::move(indices)),
        textures_(std::move(textures)) {
      SetupMesh();
  }
  unsigned int VAO, VBO, EBO;
 private:
  void SetupMesh();
};

class Model {
 public:
  std::vector<Texture*> loaded_textures_;
  std::vector<Mesh*> meshes_;
  const char* kPath, *kDirectory;
  Model(const char* path, const char* directory) {
    kPath = strcpy(new char[strlen(path) + 1], path);
    kDirectory = strcpy(new char[strlen(directory) + 1], directory);
  }
};