#include <classes/transform.h>
#include <engine/io.h>

void Transform::Load(pugi::xml_node& node) {
  position_ = IIO::LoadVec3(node, position_.i_label_);
  scale_ = IIO::LoadVec2(node, scale_.i_label_);
  rotation_ = IIO::LoadVec3(node, rotation_.i_label_);
}

void Transform::Save(pugi::xml_node& node) const {
  IIO::SaveVec3(position_.i_value_, node, position_.i_label_);
  IIO::SaveVec2(scale_.i_value_, node, scale_.i_label_);
  IIO::SaveVec3(rotation_.i_value_, node, rotation_.i_label_);
}