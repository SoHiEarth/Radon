#pragma once
#include <engine/asset_manager.h>
class Texture;
class Shader;
struct Material : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeMaterial;
  }
  Texture* diffuse_ = nullptr;
  Texture* specular_ = nullptr;
  Shader* shader_ = nullptr;
  float shininess_ = 0;
  explicit Material(Engine* engine, std::string_view path) : Asset(engine, path) {}
};
