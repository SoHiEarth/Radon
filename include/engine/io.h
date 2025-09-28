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
class Model;
class Material;
class Object;

namespace io {
extern Level* g_level;
extern std::string g_engine_directory;
extern std::unordered_map<std::string_view, std::function<Component*()>>
    g_component_factory;

void Init();
Shader* LoadShader(std::string_view /*vertex_path*/,
                                   std::string_view /*fragment_path*/);
Texture* LoadTexture(std::string_view /*path*/);
Model* LoadModel(std::string_view /*path*/);
Material* LoadMaterial(std::string_view /*diffuse*/, std::string_view /*specular*/,
                                       std::string_view /*vertex*/, std::string_view /*fragment*/,
                                       float /*shininess*/);

namespace xml {
Level* LoadLevel(std::string_view /*path*/);
Object* LoadObject(pugi::xml_node& /*base_node*/);
Material* LoadMaterial(pugi::xml_node& /*node*/);
glm::vec3 LoadVec3(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
glm::vec2 LoadVec2(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
std::string LoadString(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
int LoadInt(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
float LoadFloat(pugi::xml_node& /*base_node*/, std::string_view /*name*/);

void SaveLevel(const Level* /*level*/, std::string_view /*path*/);
void SaveObject(Object*& /*object*/, pugi::xml_node& /*base_node*/);
void SaveMaterial(const Material* /*material*/, pugi::xml_node& /*base_node*/);
void SaveVec3(const glm::vec3& /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
void SaveVec2(const glm::vec2& /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
void SaveString(std::string_view /*value*/, pugi::xml_node& /*base_node*/,
                std::string_view /*name*/);
void SaveInt(const int* /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
void SaveFloat(const float* /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
}  // namespace xml
}  // namespace io

#endif  // IO_H
