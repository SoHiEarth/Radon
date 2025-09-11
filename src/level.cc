#include <classes/level.h>
#include <classes/object.h>

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
  if (name != "Object")
    object->name_ = std::string(name);
  objects_.push_back(object);
  object->Init();
}

void Level::RemoveObject(Object* object) {
  if (object == nullptr) {
    return;
  }
  auto it = std::find(objects_.begin(), objects_.end(), object);
  if (it != objects_.end()) {
    (*it)->Quit();
    delete *it;
    objects_.erase(it);
  }
}