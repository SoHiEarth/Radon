#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <pugixml.hpp>

class Object;
class Texture;
class Level;
class Shader;

namespace f {
Texture* LoadTexture(std::string_view path);
void FreeTexture(Texture* texture);

Level* LoadLevel(std::string_view path);
void LoadLevelDynamicData(Level* level, std::string_view path);
void SaveLevel(const Level* level, std::string_view path);
void SaveLevelDynamicData(const Level* level, std::string_view path);

Object* LoadObject(pugi::xml_node& base_node);
void SaveObject(const Object* object, pugi::xml_node& base_node);

Shader* LoadShader(std::string_view k_vert_path, std::string_view k_frag_path);
void FreeShader(Shader* shader);
}  // namespace f

#endif  // FILESYSTEM_H
