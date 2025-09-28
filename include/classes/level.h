#ifndef LEVEL_H
#define LEVEL_H

#include <memory>
#include <string>
#include <vector>

class Object;
class Level {
public:
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object*, std::string_view /*name*/ = "Object");
  void RemoveObject(const Object*);
  std::string path_;
  std::vector<Object*> objects_;
};

#endif  // LEVEL_H
