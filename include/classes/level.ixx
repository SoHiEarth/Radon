module;
#include <string>
#include <string_view>
#include <vector>

export module metal.level;
export import metal.object;

export class Level {
public:
  void Init();
  void Update();
  void Render();
  void Quit();
  void AddObject(Object* /*object*/, std::string_view /*name*/ = "Object");
  void RemoveObject(Object* /*object*/);
  std::string path_;
  std::vector<Object*> objects_;
};