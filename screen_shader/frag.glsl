#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D scene;
uniform sampler2D blur;
uniform bool bloom;
uniform float exposure;
uniform float pixelation_factor;     // For pixel art look
uniform bool enable_dithering;       // For retro dithering
uniform float brightness_levels;     // For color quantization
uniform bool enable_scanlines;       // For CRT effect
uniform float scanline_intensity;

vec3 ApplyRetroEffects(vec3 color, vec2 texCoord);

void main() {
  const float gamma = 2.2;

  // Pixelation
  vec2 coord = TexCoords;
  if (pixelation_factor > 1.0) {
    coord = floor(coord * pixelation_factor) / pixelation_factor;
  }

  vec3 hdrColor = texture(scene, coord).rgb;
  vec3 bloomColor = texture(blur, coord).rgb;
  if (bloom)
    hdrColor += bloomColor;

//  vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
//  result = pow(result, vec3(1.0 / gamma));
//  result = ApplyRetroEffects(result, coord);
//  FragColor = vec4(result, 1.0);
  FragColor = vec4(hdrColor, 1.0);
}

vec3 ApplyRetroEffects(vec3 color, vec2 texCoord) {
  vec3 result = color;

  // Color quantization for retro look
  if (brightness_levels > 0.0) {
    result = floor(result * brightness_levels) / brightness_levels;
  }

  // Simple dithering pattern
  if (enable_dithering) {
    float dither = fract(sin(dot(texCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    result += (dither - 0.5) * (1.0 / max(brightness_levels, 2.0)); // scale to quantization step
    result = clamp(result, 0.0, 1.0);
  }

  // Scanlines for CRT effect
  if (enable_scanlines) {
    float scanline = sin(texCoord.y * 800.0) * scanline_intensity;
    result *= (1.0 - scanline);
  }

  return result;
}
