#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <vector>

class Object;
class Level : public Asset {
private:
  Engine* engine_;

public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeLevel;
  }
  std::vector<Object*> objects_;
  void Init();
  void Update();
  void Render();
  void Quit();
  Object* NewObject(const std::string& /*name*/ = "Object");
  void RemoveObject(const Object* /*object*/);
  explicit Level(std::string_view path, Engine* engine) : engine_(engine) {
    path_ = path;
  }
};
