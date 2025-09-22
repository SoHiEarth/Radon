#ifndef COMPONENT_H
#define COMPONENT_H
#define ATTR_NO_MATERIAL  virtual const bool HasMaterial() const { return false; } \
                          virtual Material*& GetMaterial() { static Material* temp = nullptr; return temp; }
#define ATTR_HAS_MATERIAL const bool HasMaterial() const override { return true; } \
                          Material*& GetMaterial() override { return material_; }

#include <classes/editable.h>
#include <pugixml.hpp>

class Object;
class Material;
class Component {
 public:
  ATTR_NO_MATERIAL;
  virtual void Init() {}
  virtual void Update() {}
  virtual void Render() {}
  virtual void Quit() {}
  Object* parent_ = nullptr;
  std::vector<IEditable*> reg_;
  bool has_initialized_ = false;
  bool has_quit_ = false;
  virtual void Load(pugi::xml_node&) = 0;
  virtual void Save(pugi::xml_node&) const = 0;
  [[nodiscard]] virtual std::string GetTypeName() const = 0;
  virtual ~Component() = default;
};

#endif  // COMPONENT_H