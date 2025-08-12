#ifndef EDITABLE_H
#define EDITABLE_H
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <classes/material.h>
void MaterialView(Material* material);

class IEditable {
  public:
    virtual void RenderInterface() = 0;
    IEditable(std::vector<IEditable*>& registry) {
      registry.push_back(this);
    }
};

template <typename T> class Editable : public IEditable {
  public:
    T i_value;
    const char* i_label;
    Editable(const T& value, const char* label, std::vector<IEditable*>& registry) : i_value(value), i_label(label), IEditable(registry) {}
    inline bool operator==(const T& rhs) const { return (i_value == rhs); }
    inline bool operator==(const Editable<T>& rhs) const { return (i_value == rhs.i_value); }
    inline operator T() const { return i_value; }
    T& operator*() { return i_value; }
    const T& operator*() const { return i_value; }
    T* operator->() { return &i_value; }
    const T* operator->() const { return &i_value; }
    void RenderInterface() override {
      if constexpr (std::is_same_v<T, bool>) {
        ImGui::Checkbox(i_label, &i_value);
      }
      else if constexpr (std::is_same_v<T, int>) {
        ImGui::DragInt(i_label, &i_value);
      }
      else if constexpr (std::is_same_v<T, float>) {
        ImGui::DragFloat(i_label, &i_value);
      }
      else if constexpr (std::is_same_v<T, std::string>) {
        ImGui::InputText(i_label, &i_value);
      }
      else if constexpr (std::is_same_v<T, glm::vec2>) {
        ImGui::DragFloat2(i_label, glm::value_ptr(i_value), 0.1f);
      }
      else if constexpr (std::is_same_v<T, glm::vec3>) {
        ImGui::DragFloat3(i_label, glm::value_ptr(i_value), 0.1f);
      }
      else if constexpr (std::is_same_v<T, Material*>) {
        MaterialView(i_value);
      }
    }
};

#endif // EDITABLE_H
