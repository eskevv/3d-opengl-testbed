#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light {
    vec4 transmission;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material;
uniform Light light;
uniform float time;

out vec4 FragColor;

void main()
{

    // view space pre calculations
    vec3 specularMap = vec3(texture(material.specular, TexCoords));
    vec3 diffuseMap = vec3(texture(material.diffuse, TexCoords));

    float attenuation = 1.0;
    vec3 lightDir = vec3(light.transmission);

    if(light.transmission.w == 1.0) {
        lightDir = normalize(vec3(light.transmission) - FragPos);
        float dist = length(vec3(light.transmission) - FragPos);
        attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    }

    // ambient
    vec3 ambient = light.ambient * diffuseMap * attenuation;

    // diffuse
    float diffuseAngle = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diffuseAngle * diffuseMap * attenuation;

    // specular
    vec3 viewDir = normalize(-FragPos);
    vec3 reflectDir = reflect(-lightDir, Normal);
    float specularAngle = max(dot(viewDir, reflectDir), 0.0);
    float shininess = pow(specularAngle, material.shininess);
    vec3 specular = light.specular * shininess * specularMap * attenuation;

    // emission
    vec3 emissive = specularMap == vec3(0) ? vec3(1) : vec3(0);
    vec3 emissiveness = emissive * shininess * diffuseAngle * light.specular;
    vec3 emissionCoords = vec3(texture(material.emission, TexCoords + vec2(0.0,time / 2)));
    vec3 emission = emissionCoords * emissiveness * attenuation;

    // result
    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}