#ifndef MESH_H
#define MESH_H
#include <classes/shader.h>
#include <classes/texture.h>
#include <classes/vertex.h>
#include <vector>
class Mesh {
public:
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture*> textures_;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture*> textures);
  void Draw(Shader& shader);
private:
  unsigned int vao_, vbo_, ebo_;
  void SetupMesh();
};
#endif
