#ifndef ENGINE_FILESYSTEM_H
#define ENGINE_FILESYSTEM_H

#include <classes/texture.h>
#include <classes/level.h>
#include <pugixml.hpp>

namespace f {
  Texture LoadTexture(const std::string_view path);
  Level LoadLevel(const std::string_view path);
  void LoadLevelDynamicData(const Level* level, const std::string_view path);
  void SaveLevel(const Level* level, const std::string_view path);
  void SaveLevelDynamicData(const Level* level, const std::string_view path);

  void LoadObject(Object* object, const pugi::xml_node& base_node);
  void SaveObject(const Object* object, const pugi::xml_node& base_node);
}

#endif
