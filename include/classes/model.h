#ifndef MODEL_H
#define MODEL_H
#include <assimp/scene.h>
#include <classes/mesh.h>

class Model {
public:
  explicit Model(const char* path) {
    LoadModel(path);
  }
  void Draw(Shader& shader);

private:
  std::vector<Mesh> meshes_;
  std::vector<Texture*> textures_loaded_;
  std::string directory_;
  void LoadModel(std::string_view path);
  void ProcessNode(aiNode* node, const aiScene* scene);
  Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture*> LoadMaterialTextures(aiMaterial* material, aiTextureType type,
                                             std::string_view type_name);
};
#endif
