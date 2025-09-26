#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <classes/editable.h>
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
class Object;

namespace io {
extern std::unique_ptr<Level> g_level;
extern std::string g_engine_directory;
extern std::unordered_map<std::string, std::function<std::unique_ptr<Component>()>>
    g_component_factory;

void Init();
std::unique_ptr<Shader> LoadShader(std::string_view /*vertex_path*/,
                                   std::string_view /*fragment_path*/);
std::unique_ptr<Texture> LoadTexture(std::string_view /*path*/);
std::shared_ptr<Material> LoadMaterial(std::string_view diffuse, std::string_view specular,
                                       std::string_view vertex, std::string_view fragment,
                                       float shininess);

// Namespace for handling XML
namespace xml {
std::unique_ptr<Level> LoadLevel(std::string_view /*path*/);
std::shared_ptr<Object> LoadObject(pugi::xml_node& /*base_node*/);
std::shared_ptr<Material> LoadMaterial(pugi::xml_node& /*base_node*/);
glm::vec3 LoadVec3(pugi::xml_node& /*base_node*/, std::string /*name*/);
glm::vec2 LoadVec2(pugi::xml_node& /*base_node*/, std::string /*name*/);
std::string LoadString(pugi::xml_node& /*base_node*/, std::string /*name*/);
int LoadInt(pugi::xml_node& /*base_node*/, std::string /*name*/);
float LoadFloat(pugi::xml_node& /*base_node*/, std::string /*name*/);

void SaveLevel(const Level& /*level*/, std::string_view /*path*/);
void SaveObject(const Object& /*object*/, pugi::xml_node& /*base_node*/);
void SaveMaterial(const Material& /*material*/, pugi::xml_node& /*base_node*/);
void SaveVec3(const glm::vec3& /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveVec2(const glm::vec2& /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveString(std::string_view /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveInt(const int* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveFloat(const float* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
}  // namespace xml
}  // namespace io

#endif  // FILESYSTEM_H
