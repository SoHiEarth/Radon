#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <glm/glm.hpp>

class Shader : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeShader;
  }
  const std::string vertex_path_;
  const std::string fragment_path_;
  unsigned int id_ = 0;
  void Use() const;
  void SetInt(const std::string& /*name*/, int /*value*/) const;
  void SetFloat(const std::string& /*name*/, float /*value*/) const;
  void SetVec2(const std::string& /*name*/, const glm::vec2& /*value*/) const;
  void SetVec3(const std::string& /*name*/, const glm::vec3& /*value*/) const;
  void SetMat4(const std::string& /*name*/, const glm::mat4& /*value*/) const;
  Shader(Engine* engine, std::string_view vert, std::string_view frag) :
    Asset(engine, (std::string(vert) + ";" + std::string(frag))),
    vertex_path_(vert),
    fragment_path_(frag) {};
};
