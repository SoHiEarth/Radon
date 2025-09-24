#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <memory>

class Object;
class Level {
public:
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(std::shared_ptr<Object> /*object*/, std::string_view /*name*/ = "Object");
  void RemoveObject(std::shared_ptr<Object> /*object*/);
  std::string path_;
  std::vector<std::shared_ptr<Object>> objects_;
};

#endif  // LEVEL_H
