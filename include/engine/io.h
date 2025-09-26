#ifndef IO_H
#define IO_H

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <pugixml.hpp>
#include <unordered_map>

class Component;
class Texture;
class Level;
class Shader;
class Level;
class Material;
class Object;

namespace io {
extern std::unique_ptr<Level> g_level;
extern std::string g_engine_directory;
extern std::unordered_map<std::string_view, std::function<std::unique_ptr<Component>()>>
    g_component_factory;

void Init();
std::unique_ptr<Shader> LoadShader(std::string_view, std::string_view);
std::unique_ptr<Texture> LoadTexture(std::string_view);
std::shared_ptr<Material> LoadMaterial(std::string_view, std::string_view, std::string_view, std::string_view, float);

namespace xml {
std::unique_ptr<Level> LoadLevel(std::string_view);
std::shared_ptr<Object> LoadObject(pugi::xml_node&);
std::shared_ptr<Material> LoadMaterial(pugi::xml_node&);
glm::vec3 LoadVec3(pugi::xml_node&, std::string_view);
glm::vec2 LoadVec2(pugi::xml_node&, std::string_view);
std::string LoadString(pugi::xml_node&, std::string_view);
int LoadInt(pugi::xml_node&, std::string_view);
float LoadFloat(pugi::xml_node&, std::string_view);

void SaveLevel(const Level&, std::string_view);
void SaveObject(const Object&, pugi::xml_node&);
void SaveMaterial(const Material&, pugi::xml_node&);
void SaveVec3(const glm::vec3&, pugi::xml_node&, std::string_view);
void SaveVec2(const glm::vec2&, pugi::xml_node&, std::string_view);
void SaveString(std::string_view, pugi::xml_node&, std::string_view);
void SaveInt(const int*, pugi::xml_node&, std::string_view);
void SaveFloat(const float*, pugi::xml_node&, std::string_view);
}  // namespace xml
}  // namespace io

#endif  // IO_H
