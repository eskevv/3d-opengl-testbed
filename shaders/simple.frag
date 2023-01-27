#version 450 core

flat in int ID;

out vec4 FragColor;

// uniform sampler2D textures[4];

void main()
{
  if (ID == 1) {
    FragColor = vec4(0.1, 0.1, 0.53, 1.0);
  }
}