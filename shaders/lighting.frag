#version 330 core

#define NR_DIR_LIGHTS 1
#define NR_SPOT_LIGHTS 1
#define NR_POINT_LIGHTS 4
#define NR_TEXTURES_SLOTS 192

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
flat in uint TexIndex;
flat in uint SpecularIndex;
flat in uint EmissionIndex;

out vec4 FragColor;

struct DirLight {
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    bool enabled;
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    float cutoff;
    float outerCutoff;
};

uniform sampler2D textures[NR_TEXTURES_SLOTS];
uniform DirLight dirLights[NR_DIR_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform float time;
uniform bool emissive;
uniform bool specular;
uniform bool diffuse;
uniform float emissionSpeed;
uniform float emissionStrength;
uniform float shininess;

// Definitions

vec3 diffuseMap = vec3(texture(textures[TexIndex], TexCoords));
vec3 specularMap = vec3(texture(textures[SpecularIndex], TexCoords));
vec3 emissionMap = vec3(texture(textures[EmissionIndex], TexCoords + vec2(0.0, time * emissionSpeed)));

// Function Prototypes

vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light);
vec3 CalcSpotLight(SpotLight light);
vec3 ComputeAmbient(vec3 color);
vec3 ComputeDiffuse(vec3 color, vec3 lightDir);
vec3 ComputeSpecular(vec3 color, vec3 lightDir);
float ComputeAttenuation(vec3 position, float c, float l, float q);
float ComputeIntensity(SpotLight light, vec3 lightDir);

// Program

void main() {
    vec3 result = vec3(0);

    for(int i = 0; i < NR_DIR_LIGHTS; i++) {
        if (!dirLights[i].enabled) continue;
        result += CalcDirLight(dirLights[i]);
    }
    for(int i = 0; i < NR_SPOT_LIGHTS; i++) {
        if (!spotLights[i].enabled) continue;
        result += CalcSpotLight(spotLights[i]);
    }
    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        if (!pointLights[i].enabled) continue;
        result += CalcPointLight(pointLights[i]);
    }

    FragColor = vec4(result, 1.0);
}

// Light Casters

vec3 CalcDirLight(DirLight light) {
    vec3 ambient  = ComputeAmbient(light.ambient);
    vec3 diffuse  = ComputeDiffuse(light.diffuse, -light.direction);
    vec3 specular = ComputeSpecular(light.specular, -light.direction);

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light) {
    vec3 lightDir = normalize(light.position - FragPos);
    float attenuation = ComputeAttenuation(light.position, light.constant, light.linear, light.quadratic);

    vec3 ambient  = ComputeAmbient(light.ambient);
    vec3 diffuse  = ComputeDiffuse(light.diffuse, lightDir);
    vec3 specular = ComputeSpecular(light.specular, lightDir);

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light) {
    vec3 lightDir = normalize(light.position - FragPos);
    float attenuation = ComputeAttenuation(light.position, light.constant, light.linear, light.quadratic);
    float intensity = ComputeIntensity(light, -lightDir);

    vec3 ambient  = ComputeAmbient(light.ambient);
    vec3 diffuse = ComputeDiffuse(light.diffuse, lightDir);
    vec3 specular = ComputeSpecular(light.specular, lightDir);

    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}

// Helpers

float ComputeIntensity(SpotLight light, vec3 lightDir) {
    float theta = dot(lightDir, light.direction);
    float epsilon = light.cutoff - light.outerCutoff;
    return clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
}

float ComputeAttenuation(vec3 position, float c, float l, float q) {
    float dist = length(position - FragPos);
    return 1.0 / (c + l * dist + q * (dist * dist));
}

vec3 ComputeDiffuse(vec3 color, vec3 lightDir) {
    if (!diffuse)
      return vec3(0);

    float diffuseAngle = max(dot(lightDir, Normal), 0.0);
    vec3 emissivenes = emissionMap * (specularMap == vec3(0) ? vec3(1) : vec3(0));
    vec3 emission = emissive ? emissivenes * (color * emissionStrength) * diffuseAngle : vec3(0);

    return color * diffuseAngle * diffuseMap + emission;
}

vec3 ComputeSpecular(vec3 color, vec3 lightDir) {
    if (!specular)
      return vec3(0);

    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    return color * shininess * specularMap;
}

vec3 ComputeAmbient(vec3 color) {
  vec3 output = diffuse ? color * diffuseMap : color;

  return output;
}