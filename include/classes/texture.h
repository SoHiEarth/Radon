#pragma once
#include <engine/asset_manager.h>

struct Texture : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeTexture;
  }
  unsigned int id_ = 0;
  int width_ = 0, height_ = 0, channels_ = 0;
  const char* name_ = "";
  explicit Texture(const char* path) {
    path_ = std::string(path);
  }
};
