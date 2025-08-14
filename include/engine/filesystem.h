#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <pugixml.hpp>
#include <glm/glm.hpp>
#include <fmt/core.h>
#include <classes/editable.h>
#include <classes/material.h>
#include <stdexcept>
#include <format>

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

Shader* LoadShader(std::string_view vert_path, std::string_view frag_path);
void FreeShader(Shader* shader);

template <typename T>
void LoadSerialized(T* value, pugi::xml_node& base_node, const char* name) {
  if (value == nullptr) {
    throw std::runtime_error(std::format("Null pointer passed as argument to load. Details: {}", name));
  }
  pugi::xml_node node = base_node.child(name);
  if constexpr (std::is_same_v<T, glm::vec3>) {
    value->x = node.attribute("x").as_float(0.0F);
    value->y = node.attribute("y").as_float(0.0F);
    value->z = node.attribute("z").as_float(0.0F);
  }
  else if constexpr (std::is_same_v<T, glm::vec2>) {
    value->x = node.attribute("x").as_float(0.0F);
    value->y = node.attribute("y").as_float(0.0F);
  }
  else if constexpr (std::is_same_v<T, std::string>) {
    *value = node.text().as_string();
  }
  else if constexpr (std::is_same_v<T, int>) {
    *value = node.text().as_int(0);
  }
  else if constexpr (std::is_same_v<T, float>) {
    *value = node.text().as_float(0.0F);
  }
  else {
    throw std::logic_error("Critical: Load for requested class not implemented.");
  }
}

template <typename T>
void LoadEditableSerialized(Editable<T>* value, pugi::xml_node& base_node) {
  if (value == nullptr) {
    return;
  }
  LoadSerialized(&value->i_value_, base_node, value->i_label_);
}

template <typename T>
void SaveSerialized(const T* value, pugi::xml_node& base_node, const char* name) {
  pugi::xml_node node = base_node.child(name);
  if (!node) {
    node = base_node.append_child(name);
  }
  if constexpr (std::is_same_v<T, glm::vec3>) {
    node.append_attribute("x").set_value(value->x);
    node.append_attribute("y").set_value(value->y);
    node.append_attribute("z").set_value(value->z);
  }else if constexpr (std::is_same_v<T, glm::vec2>) {
    node.append_attribute("x").set_value(value->x);
    node.append_attribute("y").set_value(value->y);
  }
  else if constexpr (std::is_same_v<T, std::string>) {
    node.text().set(value->c_str());
  }
  else if constexpr (std::is_same_v<T, int>) {
    node.text().set(*value);
  }
  else if constexpr (std::is_same_v<T, float>) {
    node.text().set(*value);
  }
  else {
    throw std::logic_error("Critical: Save for requested class not implemented.");
  }
}

template <typename T>
void SaveEditableSerialized(const Editable<T>& value, pugi::xml_node& base_node) {
  SaveSerialized(&value.i_value_, base_node, value.i_label_);
}

void LoadMaterial(Material* material, pugi::xml_node& base_node);
void SaveMaterial(const Material* material, pugi::xml_node& base_node);

}  // namespace f

#endif  // FILESYSTEM_H
