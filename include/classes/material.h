#pragma once
#include <engine/asset_manager.h>
class Texture;
class Shader;
struct Material : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeMaterial;
  }
  std::shared_ptr<Texture> diffuse_ = nullptr;
  std::shared_ptr<Texture> specular_ = nullptr;
  std::shared_ptr<Shader> shader_ = nullptr;
  float shininess_ = 0;
  explicit Material(Engine* engine, std::string_view path) : Asset(engine, path) {}
};
