#pragma once
#include <cstring>
#include <vector>
class Object;
class Level {
  const char* path_;

public:
  std::vector<Object*> objects_;
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object* /*object*/, const char* /*name*/ = "Object");
  void RemoveObject(const Object* /*object*/);
  const char* GetPath() {
    return path_;
  }
  explicit Level(const char* path) {
    path_ = strcpy(new char[strlen(path) + 1], path);
  }
};
