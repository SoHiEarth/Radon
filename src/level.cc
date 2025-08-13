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

void Level::AddObject(Object* object) {
  if (object == nullptr) {
    return;
  }
  objects_.push_back(object);
  object->Init();
}
