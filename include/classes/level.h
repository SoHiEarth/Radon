#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>

class Object;
class Level {
public:
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object* /*object*/, std::string_view /*name*/ = "Object");
  void RemoveObject(Object* /*object*/);
  std::string_view path_;
  std::vector<Object*> objects_;
};

#endif  // LEVEL_H
