#include <classes/dynamic_data.h>
#include <classes/level.h>
#include <classes/object.h>

void DynamicData::Apply(Level* level) {
  if (level->path_ != target_level_) {
    return;
  }
  for (int i = 0; i < level->objects_.size(); i++) {
    for (int j = 0; j < dynamic_objects_.size(); j++) {
      if (level->objects_[i]->name_ == dynamic_objects_[i]->name_) {
        level->objects_.insert(level->objects_.begin() + i, dynamic_objects_[i]);
      }
    }
  }
}
