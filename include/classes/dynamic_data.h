#ifndef DYNAMIC_DATA_H
#define DYNAMIC_DATA_H

#include <string>
#include <vector>
class Object;
class Level;

struct DynamicData {
  std::string target_level_;
  std::vector<Object*> dynamic_objects_;
  void Apply(Level*);
};

#endif  // DYNAMIC_DATA_H
