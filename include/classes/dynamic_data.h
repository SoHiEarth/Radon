#ifndef DYNAMIC_DATA_H
#define DYNAMIC_DATA_H

#include <vector>
class Object;
class Level;

struct DynamicData {
  std::string target_level;
  std::vector<Object*> dynamic_objects;
  void Apply(Level* level);
};

#endif // DYNAMIC_DATA_H
