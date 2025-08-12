#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <classes/object.h>

class Shader;
class Light : public Object {
  public:
    Editable<glm::vec3> ambient = {glm::vec3(0.0f), "Ambient", reg},
                        diffuse = {glm::vec3(0.0f), "Diffuse", reg},
                        specular = {glm::vec3(0.0f), "Specular", reg};
    void Update() override {};
    void Render() override {};
};

class DirectionalLight : public Light {
  public:
    Editable<glm::vec3> direction = {glm::vec3(0.0f), "Direction", reg};
    void Init() override;
    void Quit() override;
    void SetUniforms(const Shader* shader);
};

class PointLight : public Light {
  public:
    Editable<float> constant = {1.0f, "Constant", reg},
                    linear = {0.0f, "Linear", reg},
                    quadratic = {0.0f, "Quadratic", reg};
    void Init() override;
    void Quit() override;
    void SetUniforms(const Shader* shader, const int pos);
};

class SpotLight : public Light {
  public:
    Editable<glm::vec3> direction = {glm::vec3(0.0f), "Direction", reg};
    Editable<float> constant = {1.0f, "Constant", reg},
                    linear = {0.0f, "Linear", reg},
                    quadratic = {0.0f, "Quadratic", reg},
                    cutOff = {20.0f, "Cutoff", reg},
                    outerCutOff = {25.0f, "Outer Cutoff", reg};
    void Init() override;
    void Quit() override;
    void SetUniforms(const Shader* shader, const int pos);
};

#endif // LIGHT_H
