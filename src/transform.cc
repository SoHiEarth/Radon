#include <classes/transform.h>
#include <engine/io.h>

void Transform::Load(pugi::xml_node& node) {
  position_ = IIO::Load<glm::vec3>(node, position_.i_label_);
  scale_ = IIO::Load<glm::vec2>(node, scale_.i_label_);
  rotation_ = IIO::Load<glm::vec3>(node, rotation_.i_label_);
}

void Transform::Save(pugi::xml_node& node) const {
  IIO::Save(position_.i_value_, node, position_.i_label_);
  IIO::Save(scale_.i_value_, node, scale_.i_label_);
  IIO::Save(rotation_.i_value_, node, rotation_.i_label_);
}