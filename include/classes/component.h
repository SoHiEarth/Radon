#ifndef COMPONENT_H
#define COMPONENT_H
#define ATTR_NO_MATERIAL                      \
  virtual const bool HasMaterial() const {    \
    return false;                             \
  }                                           \
  virtual Material*& GetMaterial() {          \
    static Material* null_material = nullptr; \
    return null_material;                     \
  }
#define ATTR_HAS_MATERIAL                   \
  const bool HasMaterial() const override { \
    return true;                            \
  }                                         \
  Material*& GetMaterial() override {       \
    return material_;                       \
  }
#define ATTR_NO_MODEL                   \
  virtual const bool HasModel() const { \
    return false;                       \
  }                                     \
  virtual Model*& GetModel() {          \
    static Model* null_model = nullptr; \
    return null_model;                  \
  }
#define ATTR_HAS_MODEL                   \
  const bool HasModel() const override { \
    return true;                         \
  }                                      \
  Model*& GetModel() override {          \
    return model_;                       \
  }

#include <classes/editable.h>
#include <pugixml.hpp>

class Model;
class Object;
class Material;
class Component {
public:
  ATTR_NO_MATERIAL;
  ATTR_NO_MODEL;
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
  [[nodiscard]] virtual const char* GetTypeName() const = 0;
  virtual ~Component() = default;
};

#endif  // COMPONENT_H