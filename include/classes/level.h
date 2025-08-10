#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
class Object;

enum class LevelAttribute {
  LEVEL_ATTRIB_FLOW = 0,
  LEVEL_ATTRIB_NORMAL = 0 << 1
};

class Level {
  public:
    void Init();
    void Update();
    void Quit();
    std::vector<Object*> objects;
  private:
};

#endif // LEVEL_H
