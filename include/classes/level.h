#pragma once
#include <engine/asset_manager.h>
#include <cstring>
#include <vector>
#include <memory>
#include <classes/object.h>

class Level : public Asset {
public:
  AssetType GetType() const override {
    return AssetType::kAssetTypeLevel;
  }
  std::vector<std::unique_ptr<Object>> objects_;
  void Init();
  void Update();
  void Render();
  void Quit();
  Object* NewObject(const std::string& /*name*/ = "Object");
  void RemoveObject(const Object* /*object*/);
  void RemoveObject(const std::unique_ptr<Object>& /*object*/);
  explicit Level(Engine* engine, std::string_view path) : Asset(engine, path) {}
  ~Level() = default;
};