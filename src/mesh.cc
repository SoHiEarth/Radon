#include <classes/mesh.h>
#include <glad/glad.h>
#include "classes/texture.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
           std::vector<Texture*> textures) {
  this->vertices_ = std::move(vertices);
  this->indices_ = std::move(indices);
  this->textures_ = std::move(textures);
  SetupMesh();
}

void Mesh::SetupMesh() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &ebo_);

  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(),
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *) offsetof(Vertex, normal_));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *) offsetof(Vertex, texcoords_));
  glBindVertexArray(0);
}

void Mesh::Draw(Shader &shader) {
  unsigned int diffuse_nr = 1;
  unsigned int specular_nr = 1;
  for (unsigned int i = 0; i < textures_.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    const std::string kName = textures_[i]->type_;
    if (kName == "texture_diffuse") {
      number = std::to_string(diffuse_nr++);
    } else if (kName == "texture_specular") {
      number = std::to_string(specular_nr++);
    }

    shader.SetInt(("material." + kName += number), i);
    glBindTexture(GL_TEXTURE_2D, textures_[i]->id_);
  }
  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
