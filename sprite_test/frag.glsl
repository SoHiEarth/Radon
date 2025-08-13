#version 330 core
out vec4 FragColor;

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
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight directional_light;
uniform int NUM_POINT_LIGHTS;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform int NUM_SPOT_LIGHTS;
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
  vec4 texDiffuse = texture(material.diffuse, TexCoord);
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 result = CalculateDirectionalLight(directional_light, norm, viewDir);
  for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
    result += CalculatePointLight(point_lights[i], norm, FragPos, viewDir);
  }
  for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
    result += CalculateSpotLight(spot_lights[i], norm, FragPos, viewDir);
  }
  FragColor = vec4(result, texDiffuse.a);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-light.direction);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
  return (ambient + diffuse + specular);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  float distance    = length(light.position - fragPos);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic
      * (distance * distance));
  vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
  vec3 lightDir = normalize(light.position - fragPos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  float distance = length(light.position - fragPos);
  float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic
      * (distance*distance));
  float theta = dot(lightDir, normalize(-light.direction));
  float epsilon = light.cutoff - light.outerCutoff;
  float intensity = clamp((theta - light.outerCutoff)/epsilon, 0.0, 1.0);
  vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
  vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
  vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  return (ambient + diffuse + specular);
}
