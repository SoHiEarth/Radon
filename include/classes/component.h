#ifndef COMPONENT_H
#define COMPONENT_H
#define ATTR_NO_MATERIAL                                      \
  virtual const bool HasMaterial() const {                    \
    return false;                                             \
  }                                                           \
  virtual Material*& GetMaterial() {          \
    static Material* null_material = nullptr; \
    return null_material;                                     \
  }
#define ATTR_HAS_MATERIAL                             \
  const bool HasMaterial() const override {           \
    return true;                                      \
  }                                                   \
  Material*& GetMaterial() override { \
    return material_;                                 \
  }
#define ATTR_NO_MESH                                  \
  virtual const bool HasMesh() const {                  \
    return false;                                       \
  }                                                     \
  virtual Model*& GetMesh() {           \
    static Model* null_mesh = nullptr;  \
    return null_mesh;                                   \
  }
#define ATTR_HAS_MESH   \
  const bool HasMesh() const override { \
    return true;                        \
  }                                     \
  Model*& GetMesh() override { \
    return mesh_;                       \
  }

#include <classes/editable.h>
#include <memory>
#include <pugixml.hpp>

class Model;
class Object;
class Material;
class Component : public std::enable_shared_from_this<Component> {
public:
  ATTR_NO_MATERIAL;
  ATTR_NO_MESH;
  virtual void Init() {}
  virtual void Update() {}
  virtual void Render() {}
  virtual void Quit() {}
  Object* parent_ = nullptr;
  std::vector<IEditable*> reg_;
  bool has_initialized_ = false;
  bool has_quit_ = false;
  virtual void Load(pugi::xml_node& /*unused*/) {};
  virtual void Save(pugi::xml_node& /*unused*/) const {};
  [[nodiscard]] virtual std::string GetTypeName() const = 0;
  virtual ~Component() = default;
};

#endif  // COMPONENT_H