#pragma once
#include <classes/material.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <utility>
#include <vector>
constexpr float kFloatStep = 0.1F;

class IEditable {
public:
  virtual void RenderInterface() = 0;
  IEditable(std::vector<IEditable*>& registry) {
    registry.push_back(this);
  }
};
template <typename T>
class Editable : public IEditable {
public:
  T i_value_;
  const char* i_label_;
  Editable(T value, const char* label, std::vector<IEditable*>& registry)
      : i_value_(std::move(value)), i_label_(label), IEditable(registry) {}
  bool operator==(const T& rhs) const {
    return (i_value_ == rhs);
  }
  bool operator==(const Editable<T>& rhs) const {
    return (i_value_ == rhs.i_value_);
  }
  operator T() const {
    return i_value_;
  }
  T& operator*() {
    return i_value_;
  }
  const T& operator*() const {
    return i_value_;
  }
  T* operator->() {
    return &i_value_;
  }
  const T* operator->() const {
    return &i_value_;
  }
  T operator=(T&& rhs) {
    i_value_ = std::move(rhs);
    return i_value_;
  }
  T operator=(const T& rhs) {
    i_value_ = rhs;
    return i_value_;
  }
  operator T*() {
    return &i_value_;
  }
  operator const T*() const {
    return &i_value_;
  }

  template <typename U = T>
  operator std::enable_if_t<std::is_same_v<U, std::string>, const char*>() const {
    return i_value_.c_str();
  }

  void RenderInterface() override {
    if constexpr (std::is_same_v<T, bool>) {
      ImGui::Checkbox(i_label_, &i_value_);
    } else if constexpr (std::is_same_v<T, int>) {
      ImGui::DragInt(i_label_, &i_value_);
    } else if constexpr (std::is_same_v<T, float>) {
      ImGui::DragFloat(i_label_, &i_value_);
    } else if constexpr (std::is_same_v<T, std::string>) {
      ImGui::InputText(i_label_, &i_value_);
    } else if constexpr (std::is_same_v<T, char*>) {
      std::string str = i_value_;
      ImGui::InputText(i_label_, &str);
      i_value_ = str.c_str();
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
      ImGui::DragFloat2(i_label_, glm::value_ptr(i_value_), kFloatStep);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
      ImGui::DragFloat3(i_label_, glm::value_ptr(i_value_), kFloatStep);
    }
  }
};