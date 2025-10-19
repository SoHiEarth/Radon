#include <engine/asset_manager.h>
#include <engine/engine.h>

Asset::Asset(Engine* engine, std::string_view path)
    : id_(boost::uuids::random_generator()()), engine_(engine), path_(path) {
  if (engine_) {
    engine_->GetDebug().Log("Created asset " + boost::uuids::to_string(id_));
  }
}

Asset::~Asset() {
  if (engine_) {
    engine_->GetDebug().Log("Destroyed asset " + boost::uuids::to_string(id_));
  }
}