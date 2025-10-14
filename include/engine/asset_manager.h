#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <engine/debug.h>
#include <engine/interface.h>

enum AssetType : std::uint8_t {
  kAssetTypeUnknown = 0x000,
  kAssetTypeLevel = 0x001,
  kAssetTypeModel = 0x002,
  kAssetTypeTexture = 0x003,
  kAssetTypeShader = 0x004,
  kAssetTypeMaterial = 0x005,
  kAssetTypeAudio = 0x006
};

class Asset;
class IAssetManager : public Interface {
private:
  std::vector<Asset*> assets_;

public:
  const char* Name() override {
    return "AssetManager";
  }

  void AddAsset(Asset* asset) {
    assets_.push_back(asset);
  }

  void RemoveAsset(Asset* asset) {
    assets_.erase(std::remove(assets_.begin(), assets_.end(), asset), assets_.end());
  }

  const std::vector<Asset*>& GetAssets() {
    return assets_;
  }
};

class Asset {
protected:
  boost::uuids::uuid id_;
  bool loaded_ = false;
  std::string path_;
  
public:
  virtual AssetType GetType() const {
    return AssetType::kAssetTypeUnknown;
  }
  void SetLoaded(bool l) {
    loaded_ = l;
  }
  bool GetLoaded() const {
    return loaded_;
  }
  boost::uuids::uuid GetID() const {
    return id_;
  }
  void SetPath(const std::string& p) {
    path_ = p;
  }
  const std::string& GetPath() const {
    return path_;
  }
  Asset() :
    id_(boost::uuids::random_generator()()) {
    Interface::Get<IDebug>().Log("Created asset " + boost::uuids::to_string(id_));
    Interface::Get<IAssetManager>().AddAsset(this);
  }
  ~Asset() {
    Interface::Get<IDebug>().Log("Destroyed asset " + boost::uuids::to_string(id_));
    Interface::Get<IAssetManager>().RemoveAsset(this);
  }
};