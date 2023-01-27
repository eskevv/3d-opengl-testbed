#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in int id;

flat out int ID;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    ID = id;
}
