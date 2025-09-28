#include <engine/io.h>
#include <classes/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

//////////////////////////
/// Model IO functions ///
//////////////////////////

std::vector<Texture*> LoadMaterialTextures(Model* model, aiMaterial* mat, aiTextureType type,
                                           std::string_view type_name) {
  std::vector<Texture*> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (auto& texture : model->loaded_textures_) {
      if (texture->kPath == std::string(str.C_Str())) {
        textures.push_back(texture);
        skip = true;
        break;
      }
    }
    if (!skip) {
      std::string texture_path = model->kDirectory + "/" + std::string(str.C_Str());
      Texture* texture = io::LoadTexture(texture_path);
      if (texture != nullptr) {
        texture->name = std::string(type_name);
        textures.push_back(texture);
        model->loaded_textures_.push_back(texture);
      }
    }
  }
  return textures;
}

Mesh* ProcessMesh(Model* model, aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture*> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector{};
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    vertex.position_ = vector;
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[i].x;
      vector.y = mesh->mNormals[i].y;
      vector.z = mesh->mNormals[i].z;
      vertex.normal_ = vector;
    }
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec{};
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.tex_coords_ = vec;
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.tangent_ = vector;
      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.bitangent_ = vector;
    } else {
      vertex.tex_coords_ = glm::vec2(0.0F, 0.0F);
    }
    vertices.push_back(vertex);
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }
  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<Texture*> diffuse_textures =
      LoadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuse_textures.begin(), diffuse_textures.end());
  std::vector<Texture*> specular_textures =
      LoadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specular_textures.begin(), specular_textures.end());
  return new Mesh(vertices, indices, textures);
}

void ProcessNode(Model* model, aiNode* node, const aiScene* scene, int depth = 0) {
  if (depth > 1000) {  // safeguard
    debug::Warning("Max recursion depth hit in ProcessNode. Potential cyclic scene graph.");
    return;
  } else {
    debug::Log(std::format("Processing node: {} at depth {}", node->mName.C_Str(), depth));
  }
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    model->meshes_.push_back(ProcessMesh(model, mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(model, node->mChildren[i], scene, depth + 1);
  }
}

Model* io::LoadModel(std::string_view path) {
  if (!CheckFile(path)) {
    debug::Throw(std::format("Requested model file does not exist. {}", path));
  }
  // Model path should be the directory of the model
  auto model = new Model(path, std::filesystem::path(path).parent_path().string());
  debug::Log(std::format("Loading model at path: {}", model->kPath));
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                         aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    debug::Throw(std::format("Failed to load model. {}, {}", path, importer.GetErrorString()));
  }
  ProcessNode(model, scene->mRootNode, scene);
  return model;
}

Model* io::xml::LoadModel(pugi::xml_node& base_node) {
  std::string path = base_node.child("model").attribute("path").as_string();
  return io::LoadModel(path);
}

void io::xml::SaveModel(const Model* model, pugi::xml_node& base_node) {
  pugi::xml_node model_node = base_node.append_child("model");
  model_node.append_attribute("path") = model->kPath.c_str();
}