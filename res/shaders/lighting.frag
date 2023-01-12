#version 330 core

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
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

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

uniform float emissionSpeed;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform float time;

// Definitions

vec3 specularMap = vec3(texture(material.specular, TexCoords));
vec3 diffuseMap = vec3(texture(material.diffuse, TexCoords));
vec3 emissionMap = vec3(texture(material.emission, TexCoords + vec2(0.0, time * emissionSpeed)));

// Function Prototypes

vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light);
vec3 CalcSpotLight(SpotLight light);

vec3 ComputeDiffuse(vec3 color, vec3 lightDir);
vec3 ComputeSpecular(vec3 color, vec3 lightDir);
float ComputeAttenuation(vec3 position, float c, float l, float q);
float ComputeIntensity(SpotLight light, vec3 lightDir);

// Program

void main() {
    vec3 result = vec3(0);

    // result += CalcDirLight(dirLight);

    for(int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalcPointLight(pointLights[i]);
    }

    // result += CalcSpotLight(spotLight);

    FragColor = vec4(result, 1.0);
}

// Light Casters

vec3 CalcDirLight(DirLight light) {
    vec3 lightDir = normalize(-light.direction);

    vec3 ambient  = light.ambient * diffuseMap;
    vec3 diffuse  = ComputeDiffuse(light.diffuse, lightDir);
    vec3 specular = ComputeSpecular(light.specular, lightDir);

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light) {
    vec3 lightDir = normalize(light.position - FragPos);
    float attenuation = ComputeAttenuation(light.position, light.constant, light.linear, light.quadratic);

    vec3 ambient  = light.ambient * diffuseMap;
    vec3 diffuse  = ComputeDiffuse(light.diffuse, lightDir);
    vec3 specular = ComputeSpecular(light.specular, lightDir);

    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light) {
    vec3 lightDir = normalize(light.position - FragPos);
    float attenuation = ComputeAttenuation(light.position, light.constant, light.linear, light.quadratic);
    float intensity = ComputeIntensity(light, lightDir);

    vec3 ambient  = light.ambient * diffuseMap;
    vec3 diffuse = ComputeDiffuse(light.diffuse, lightDir);
    vec3 specular = ComputeSpecular(light.specular, lightDir);

    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
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
    float diffuseAngle = max(dot(Normal, lightDir), 0.0);
    vec3 emissive = specularMap == vec3(0) ? vec3(1) : vec3(0);
    vec3 emission = emissionMap * emissive * diffuseAngle;
    return color * diffuseAngle * diffuseMap + emission;
}

vec3 ComputeSpecular(vec3 color, vec3 lightDir) {
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float shininess = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    return color * shininess * specularMap;
}