#include <classes/editable.h>
#include <classes/material.h>
#include <classes/texture.h>
#include <classes/shader.h>

void MaterialView(Material* material) {
  if (!material) {
    ImGui::Text("Material is not valid");
    return;
  }
  if (!material->IsValid()) return;
  ImGui::SeparatorText("Material Info");
  ImGui::Text("Diffuse");
  ImGui::Image(material->diffuse->id, ImVec2(100, 100));
  ImGui::Text("Specular");
  ImGui::Image(material->specular->id, ImVec2(100, 100));
  ImGui::DragFloat("Shininess", &material->shininess);
  ImGui::Text("Shader");
  ImGui::Text("Vert: %s", material->shader->vert_path.data());
  ImGui::Text("Frag: %s", material->shader->frag_path.data());
}
