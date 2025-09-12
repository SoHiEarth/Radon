#include <classes/level.h>
#include <classes/object.h>
#include <algorithm>

void Level::Init() {
  for (const auto& object : objects_) {
    object->Init();
  }
}

void Level::Update() {
  for (const auto& object : objects_) {
    object->Update();
  }
}

void Level::Render() {
  for (const auto& object : objects_) {
    object->Render();
  }
}

void Level::Quit() {
  for (const auto& object : objects_) {
    object->Quit();
  }
}

void Level::AddObject(Object* object, std::string_view name) {
  if (object == nullptr) {
    return;
  }
  if (name != "Object") {
    object->name_ = std::string(name);
  }
  objects_.push_back(object);
  if (!object->has_initialized_) {
    object->Init();
  }
}

void Level::RemoveObject(Object* object) {
  if (object == nullptr) {
    return;
  }

  auto it = std::ranges::find(objects_.begin(), objects_.end(), object);
  if (it != objects_.end()) {
    if (!(*it)->has_quit_ && (*it)->has_initialized_) {
      (*it)->Quit();
    }
    delete *it;
    objects_.erase(it);
  }
}
