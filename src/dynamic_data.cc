#include <classes/dynamic_data.h>
#include <classes/level.h>
#include <classes/object.h>

void DynamicData::Apply(Level* level) {
  if (level->path != target_level) return;
  for (int i = 0; i < level->objects.size(); i++) {
    for (int j = 0; j < dynamic_objects.size(); j++) {
      if (level->objects[i]->name == dynamic_objects[i]->name) {
        level->objects.insert(level->objects.begin() + i, dynamic_objects[i]);
      }
    }
  }
}
