#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <map>
#include <classes/object.h>

enum class LevelAttribute {
  LEVEL_ATTRIB_FLOW = 0, // Controls the flow of level. eg. loading screen etc.
  LEVEL_ATTRIB_NORMAL = 0 << 1
};

class Level {
  public:
    void Init();
    void Update();
    void Quit();
    const Object* GetObject(const std::string_view name) const {
        auto it = objects.find(std::string(name));
        if (it != objects.end()) {
            return it->second;
        }
        return nullptr;
    }

    const void AddObject(const std::string& name, Object* object) {
        objects.insert(std::make_pair(name, object));
    }

    const void RemoveObject(const std::string& name) {
        objects.erase(name);
    }
    std::map<std::string, Object*> objects;
  private:
};

#endif // LEVEL_H
