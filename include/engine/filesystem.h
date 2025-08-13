#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <pugixml.hpp>

class Object;
class Texture;
class Level;
class Shader;

namespace f {
  Texture* LoadTexture(const std::string_view path);
  void FreeTexture(Texture* texture);

  Level* LoadLevel(const std::string_view path);
  void LoadLevelDynamicData(Level* level, const std::string_view path);
  void SaveLevel(const Level* level, const std::string_view path);
  void SaveLevelDynamicData(const Level* level, const std::string_view path);

  void LoadObject(Object* object, const pugi::xml_node& base_node);
  void SaveObject(const Object* object, const pugi::xml_node& base_node);

  Shader* LoadShader(const std::string_view vert_path, const std::string_view frag_path);
  void FreeShader(Shader* shader);
}

#endif // FILESYSTEM_H
