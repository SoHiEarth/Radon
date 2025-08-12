#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screen_texture;
void main() {
  vec3 col = texture(screen_texture, TexCoord).rgb;
  FragColor = vec4(col, 1.0);
} 
