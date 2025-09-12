#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D scene;

void main() {
  const float gamma = 2.2;
  FragColor = vec4(texture(scene, coord).rgb, 1.0);
}
