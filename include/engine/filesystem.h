#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <classes/editable.h>
#include <functional>
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <unordered_map>

class Object;
class Texture;
class Level;
class Shader;
class Level;

namespace filesystem {
extern Level* g_level;
extern std::unordered_map<std::string, std::function<Object*()>> g_object_factory;

void FreeLevel(Level* /*level*/);

void FreeObject(Object*& /*object*/);

Shader* LoadShader(std::string_view /*vertex_path*/, std::string_view /*fragment_path*/);
void FreeShader(Shader*& /*shader*/);

// Loads a texture from a image file.
Texture* LoadTexture(std::string_view /*path*/);
void FreeTexture(Texture*& /*texture*/);

Material* LoadMaterial(std::string_view diffuse, std::string_view specular, std::string_view vertex,
                       std::string_view fragment, float shininess);
void FreeMaterial(Material*& /*material*/);

// Namespace for handling XML
namespace serialized {
Level* LoadLevel(std::string_view /*path*/);
Object* LoadObject(pugi::xml_node& /*base_node*/);
Material* LoadMaterial(pugi::xml_node& /*base_node*/);
glm::vec3 LoadVec3(pugi::xml_node& /*base_node*/, std::string /*name*/);
glm::vec2 LoadVec2(pugi::xml_node& /*base_node*/, std::string /*name*/);
std::string LoadString(pugi::xml_node& /*base_node*/, std::string /*name*/);
int LoadInt(pugi::xml_node& /*base_node*/, std::string /*name*/);
float LoadFloat(pugi::xml_node& /*base_node*/, std::string /*name*/);

void SaveLevel(const Level* /*level*/, std::string_view /*path*/);
void SaveObject(const Object* /*object*/, pugi::xml_node& /*base_node*/);
void SaveMaterial(const Material* /*material*/, pugi::xml_node& /*base_node*/);
void SaveVec3(const glm::vec3* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveVec2(const glm::vec2* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveString(const std::string* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveInt(const int* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
void SaveFloat(const float* /*value*/, pugi::xml_node& /*base_node*/, std::string /*name*/);
}  // namespace serialized
}  // namespace filesystem

#endif  // FILESYSTEM_H
