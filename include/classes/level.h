#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

class Object;
class Level {
  public:
    void Init();
    void Update();
    void Render();
    void Quit();
    void AddObject(Object* object);
    std::vector<Object*> objects;
};

#endif // LEVEL_H
