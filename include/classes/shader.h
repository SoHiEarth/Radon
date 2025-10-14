#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <glm/glm.hpp>

class Shader : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeShader;
  }
  const char* vertex_path_;
  const char* fragment_path_;
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(const char* /*name*/, int /*value*/) const;
  void SetFloat(const char* /*name*/, float /*value*/) const;
  void SetVec2(const char* /*name*/, const glm::vec2& /*value*/) const;
  void SetVec3(const char* /*name*/, const glm::vec3& /*value*/) const;
  void SetMat4(const char* /*name*/, const glm::mat4& /*value*/) const;
  Shader(const char* vert, const char* frag) {
    vertex_path_ = strcpy(new char[strlen(vert) + 1], vert);
    fragment_path_ = strcpy(new char[strlen(frag) + 1], frag);
    path_ = std::string(vert) + ";" + std::string(frag);
  };
};
