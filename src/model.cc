#include <classes/model.h>
#include <engine/filesystem.h>
#include <fmt/core.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

void Model::Draw(Shader& shader) {
  for (auto& mesh : meshes_) {
    mesh.Draw(shader);
  }
}

void Model::LoadModel(std::string_view path) {
  Assimp::Importer import;
  const aiScene* scene = import.ReadFile(path.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
  if ((scene == nullptr) || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) ||
      (scene->mRootNode == nullptr)) {
    fmt::print("Failed to load model: {}\n", import.GetErrorString());
    return;
  }
  directory_ = path.substr(0, path.find_last_of('/'));

  ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes_.push_back(ProcessMesh(mesh, scene));
  }
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture*> textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;
    glm::vec3 vector;
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
    // texture coordinates
    if (mesh->mTextureCoords[0] != nullptr) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      vertex.texcoords_ = vec;
      vector.x = mesh->mTangents[i].x;
      vector.y = mesh->mTangents[i].y;
      vector.z = mesh->mTangents[i].z;
      vertex.tangent_ = vector;
      vector.x = mesh->mBitangents[i].x;
      vector.y = mesh->mBitangents[i].y;
      vector.z = mesh->mBitangents[i].z;
      vertex.bitangent_ = vector;
    } else {
      vertex.texcoords_ = glm::vec2(0.0F, 0.0F);
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
  std::vector<Texture*> diffuse_maps =
    LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
  textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
  std::vector<Texture*> specular_maps =
      LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
  textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
  std::vector<Texture*> normal_maps =
      LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
  textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
  std::vector<Texture*> height_maps =
      LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
  textures.insert(textures.end(), height_maps.begin(), height_maps.end());
  return {vertices, indices, textures};
}

std::vector<Texture*> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
                                                 std::string_view type_name) {
  std::vector<Texture*> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    bool skip = false;
    for (auto& texture : textures_loaded_) {
      if (std::strcmp(texture->path_, str.C_Str()) == 0) {
        textures.push_back(texture);
        skip = true;
        break;
      }
    }
    if (!skip) {
      auto texture = Texture();
      texture.id_ = filesystem::LoadTexture(std::string(str.data))->id_;
      texture.path_ = str.C_Str();
      texture.type_ = type_name.data();
      textures.push_back(&texture);
      textures_loaded_.push_back(&texture);
    }
  }
  return textures;
}
