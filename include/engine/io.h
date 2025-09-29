#pragma once
#include <engine/interface.h>
#include <functional>
#include <string>
#include <map>
#include <unordered_map>
#include <glm/glm.hpp>
#include <pugixml.hpp>
#include <classes/texture.h>

class Component;
class Level;
class Shader;
class Level;
class Model;
class Material;
class Object;

class IIO : public Interface {
  Level* g_level = nullptr;
  std::string g_engine_directory{};
  std::map<std::string, Texture*> g_loaded_textures{};
  static std::unordered_map<std::string_view, std::function<Component*()>> g_component_factory;

public:
  const char* name() override {
    return "IO";
  }
  Level*& GetLevel() {
    return g_level;
  }
  void SetLevel(Level* level) {
    g_level = level;
  }
  std::string GetEngineDirectory() const {
    return g_engine_directory;
  }
  std::map<std::string, Texture*>& GetLoadedTextures() {
    return g_loaded_textures;
  }
  std::unordered_map<std::string_view, std::function<Component*()>>& GetComponentFactory() {
    return g_component_factory;
  }

  void Init() override;
  bool CheckFile(std::string_view path);
  Shader* LoadShader(std::string_view /*vertex_path*/, std::string_view /*fragment_path*/);
  Texture* LoadTexture(std::string_view /*path*/);
  Model* LoadModel(std::string_view /*path*/);
  Material* LoadMaterial(std::string_view /*diffuse*/, std::string_view /*specular*/,
                         std::string_view /*vertex*/, std::string_view /*fragment*/,
                         float /*shininess*/);
  Level* LoadLevel(std::string_view /*path*/);
  Object* LoadObject(pugi::xml_node& /*base_node*/);
  Material* LoadMaterial(pugi::xml_node& /*node*/);
  Model* LoadModel(pugi::xml_node& /*base_node*/);
  glm::vec3 LoadVec3(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  glm::vec2 LoadVec2(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  std::string LoadString(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  int LoadInt(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  float LoadFloat(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  void SaveLevel(const Level* /*level*/, std::string_view /*path*/);
  void SaveObject(Object*& /*object*/, pugi::xml_node& /*base_node*/);
  void SaveModel(const Model* /*model*/, pugi::xml_node& /*base_node*/);
  void SaveMaterial(const Material* /*material*/, pugi::xml_node& /*base_node*/);
  void SaveVec3(const glm::vec3& /*value*/, pugi::xml_node& /*base_node*/,
                std::string_view /*name*/);
  void SaveVec2(const glm::vec2& /*value*/, pugi::xml_node& /*base_node*/,
                std::string_view /*name*/);
  void SaveString(std::string_view /*value*/, pugi::xml_node& /*base_node*/,
                  std::string_view /*name*/);
  void SaveInt(const int* /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  void SaveFloat(const float* /*value*/, pugi::xml_node& /*base_node*/, std::string_view /*name*/);
};