#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <vector>

class Object;
class Level : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeLevel;
  }
  std::vector<Object*> objects_;
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object* /*object*/, const char* /*name*/ = "Object");
  void RemoveObject(const Object* /*object*/);
  explicit Level(const char* path) {
    path_ = std::string(path);
  }
};
