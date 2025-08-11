#include <classes/level.h>
#include <classes/object.h>

void Level::Init() {
  for (const auto& object : objects) {
    object->Init();
  }
}

void Level::Update() {
  for (const auto& object : objects) {
    object->Update();
  }
}

void Level::Render() {
  for (const auto& object : objects) {
    object->Render();
  }
}

void Level::Quit() {
  for (const auto& object : objects) {
    object->Quit();
  }
}

void Level::AddObject(Object* object) {
  if (!object) return;
  objects.push_back(object);
  object->Init();
}
