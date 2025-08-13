#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
  vec3 position;
  float constant;
  float linear;
  float quadratic;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
#define MAX_POINT_LIGHTS 16

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutoff;
  float outerCutoff;
  float constant;
  float linear;
  float quadratic;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
#define MAX_SPOT_LIGHTS 16

in VS_OUT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} fs_in;

uniform vec3 viewPos;
uniform Material material;
uniform int NUM_DIRECTIONAL_LIGHTS;
uniform DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
uniform int NUM_POINT_LIGHTS;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int NUM_SPOT_LIGHTS;
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
  vec4 texDiffuse = texture(material.diffuse, fs_in.TexCoords);
  if (texDiffuse.a < 0.1) {
    discard;
  }
  vec3 norm = normalize(fs_in.Normal);
  vec3 viewDir = normalize(viewPos - fs_in.FragPos);
  vec3 result = vec3(0.0, 0.0, 0.0);
  for (int i = 0; i < min(NUM_DIRECTIONAL_LIGHTS, MAX_DIRECTIONAL_LIGHTS); i++) {
    result += CalculateDirectionalLight(directional_lights[i], norm, viewDir);
  }
  for (int i = 0; i < min(NUM_POINT_LIGHTS, MAX_POINT_LIGHTS); i++) {
    result += CalculatePointLight(point_lights[i], norm, fs_in.FragPos, viewDir);
  }
  for (int i = 0; i < min(NUM_SPOT_LIGHTS, MAX_SPOT_LIGHTS); i++) {
    result += CalculateSpotLight(spot_lights[i], norm, fs_in.FragPos, viewDir);
  }
  
  float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
  if (brightness > 1.0) {
    BrightColor = vec4(result, 1.0);
  } else {
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
  }

  FragColor = vec4(result, texDiffuse.a);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
