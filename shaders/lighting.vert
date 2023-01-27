#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in uint aTexIndex;
layout (location = 4) in uint aSpecularIndex;
layout (location = 5) in uint aEmissionIndex;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
flat out uint TexIndex;
flat out uint SpecularIndex;
flat out uint EmissionIndex;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    FragPos = vec3(view * vec4(aPos, 1.0));
    Normal = normalize(aNormal);
    TexCoords = aTexCoords;
    TexIndex = aTexIndex;
    SpecularIndex = aSpecularIndex;
    EmissionIndex = aEmissionIndex;
}