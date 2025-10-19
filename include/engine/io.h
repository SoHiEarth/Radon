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
class Engine;

class IIO : public Interface {
private:
  std::shared_ptr<Level> g_level_ = nullptr;
  std::string g_engine_directory_;
  std::map<std::string, std::shared_ptr<Texture>> g_loaded_textures_;
  static std::unordered_map<std::string_view, std::function<std::unique_ptr<Component>()>> g_component_factory_;
  static bool CheckFile(std::string_view path);

protected:
  void IInit() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "IO";
  }
  std::shared_ptr<Level>& GetLevel() {
    return g_level_;
  }
  void SetLevel(std::shared_ptr<Level> level) {
    g_level_ = level;
  }
  [[nodiscard]] std::string GetEngineDirectory() const {
    return g_engine_directory_;
  }
  std::map<std::string, std::shared_ptr<Texture>>& GetLoadedTextures() {
    return g_loaded_textures_;
  }
  static std::unordered_map<std::string_view, std::function<std::unique_ptr<Component>()>>& GetComponentFactory() {
    return g_component_factory_;
  }

  std::shared_ptr<Shader> LoadShader(std::string_view /*vertex_path*/, std::string_view /*fragment_path*/);
  std::shared_ptr<Texture> LoadTexture(std::string_view /*path*/);
  Model* LoadModel(std::string_view /*path*/);
  Material* LoadMaterial(std::string_view /*diffuse*/, std::string_view /*specular*/,
    std::string_view /*vertex*/, std::string_view /*fragment*/,
    float /*shininess*/);
  std::shared_ptr<Level> LoadLevel(std::string_view /*path*/);
  std::unique_ptr<Object> LoadObject(pugi::xml_node& /*base_node*/);
  Material* LoadMaterial(pugi::xml_node& /*node*/);
  Model* LoadModel(pugi::xml_node& /*base_node*/);

  template <typename T>
  static T Load(pugi::xml_node& base_node, std::string_view name) {
    auto child = base_node.child(name.data());
    if (!child) {
      return T{};
    }

    if constexpr (std::is_same_v<T, int>) {
      return child.text().as_int();
    }
    else if constexpr (std::is_same_v<T, float>) {
      return child.text().as_float();
    }
    else if constexpr (std::is_same_v<T, bool>) {
      return child.text().as_bool();
    }
    else if constexpr (std::is_same_v<T, std::string>) {
      return child.text().as_string();
    }
    else if constexpr (std::is_same_v<T, glm::vec2>) {
      glm::vec2 result;
      result.x = child.attribute("x").as_float();
      result.y = child.attribute("y").as_float();
      return result;
    }
    else if constexpr (std::is_same_v<T, glm::vec3>) {
      glm::vec3 result;
      result.x = child.attribute("x").as_float();
      result.y = child.attribute("y").as_float();
      result.z = child.attribute("z").as_float();
      return result;
    }
    return T{};
  }
  template <typename T>
  static void Save(const T& value, pugi::xml_node& base_node, std::string_view name) {
    auto child = base_node.append_child(name.data());
    if constexpr (std::is_same_v<T, int> ||
      std::is_same_v<T, float> || std::is_same_v<T, bool>) {
      child.text().set(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
      child.text().set(value.c_str());
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
      child.append_attribute("x").set_value(value.x);
      child.append_attribute("y").set_value(value.y);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
      child.append_attribute("x").set_value(value.x);
      child.append_attribute("y").set_value(value.y);
      child.append_attribute("z").set_value(value.z);
    } else {
      static_assert(sizeof(T) == 0, "Unsupported type");
    }
  }

  void SaveLevel(const std::shared_ptr<Level> /*level*/, std::string_view /*path*/);
  void SaveObject(const std::unique_ptr<Object>& /*object*/, pugi::xml_node& /*base_node*/);
  void SaveModel(const Model* /*model*/, pugi::xml_node& /*base_node*/);
  void SaveMaterial(const Material* /*material*/, pugi::xml_node& /*base_node*/);
  std::string ReadFile(std::string_view path);
  unsigned int CompileShader(std::string_view code, int type);
};