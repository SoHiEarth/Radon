#pragma once
#include <vector>

class Object;
class Level {
public:
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object*, const char* /*name*/ = "Object");
  void RemoveObject(const Object*);
  const char* path_;
  std::vector<Object*> objects_;
};