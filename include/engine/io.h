#pragma once
#include <classes/texture.h>
#include <engine/interface.h>
#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <pugixml.hpp>
#include <string>
#include <unordered_map>

class Component;
class Level;
class Shader;
class Level;
class Model;
class Material;
class Object;

class IIO : public Interface {
private:
  Level* g_level_ = nullptr;
  std::string g_engine_directory_;
  std::map<std::string, Texture*> g_loaded_textures_;
  static std::unordered_map<std::string_view, std::function<Component*()>> g_component_factory_;
  static bool CheckFile(std::string_view path);

protected:
  void IInit() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "IO";
  }
  Level*& GetLevel() {
    return g_level_;
  }
  void SetLevel(Level* level) {
    g_level_ = level;
  }
  [[nodiscard]] std::string GetEngineDirectory() const {
    return g_engine_directory_;
  }
  std::map<std::string, Texture*>& GetLoadedTextures() {
    return g_loaded_textures_;
  }
  static std::unordered_map<std::string_view, std::function<Component*()>>& GetComponentFactory() {
    return g_component_factory_;
  }

  static Shader* LoadShader(std::string_view /*vertex_path*/, std::string_view /*fragment_path*/);
  Texture* LoadTexture(std::string_view /*path*/);
  static Model* LoadModel(std::string_view /*path*/);
  Material* LoadMaterial(std::string_view /*diffuse*/, std::string_view /*specular*/,
                         std::string_view /*vertex*/, std::string_view /*fragment*/,
                         float /*shininess*/);
  static Level* LoadLevel(std::string_view /*path*/);
  static Object* LoadObject(pugi::xml_node& /*base_node*/);
  Material* LoadMaterial(pugi::xml_node& /*node*/);
  static Model* LoadModel(pugi::xml_node& /*base_node*/);
  static glm::vec3 LoadVec3(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  static glm::vec2 LoadVec2(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  static std::string LoadString(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  static int LoadInt(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  static float LoadFloat(pugi::xml_node& /*base_node*/, std::string_view /*name*/);
  static void SaveLevel(const Level* /*level*/, std::string_view /*path*/);
  static void SaveObject(Object*& /*object*/, pugi::xml_node& /*base_node*/);
  static void SaveModel(const Model* /*model*/, pugi::xml_node& /*base_node*/);
  static void SaveMaterial(const Material* /*material*/, pugi::xml_node& /*base_node*/);
  static void SaveVec3(const glm::vec3& /*value*/, pugi::xml_node& /*base_node*/,
                       std::string_view /*name*/);
  static void SaveVec2(const glm::vec2& /*value*/, pugi::xml_node& /*base_node*/,
                       std::string_view /*name*/);
  static void SaveString(std::string_view /*value*/, pugi::xml_node& /*base_node*/,
                         std::string_view /*name*/);
  static void SaveInt(const int* /*value*/, pugi::xml_node& /*base_node*/,
                      std::string_view /*name*/);
  static void SaveFloat(const float* /*value*/, pugi::xml_node& /*base_node*/,
                        std::string_view /*name*/);
};