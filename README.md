# 3D-OpenGL-TestBed
An OpenGL rendering experiment.
This project is used to test OpenGL features and rendering techniques in hopes of creating something more modular for learning purposes.

In this project I am using modern OpenGL 3.3+ and soon to use DSA(Direct-State Access) functions.

**What you can currently see in this test bed:**
* 3D transformations: [ Eular Angles, Panning , Movement ]
* Camera Class & Projections: local space -> world space -> view space -> clip space -> screen space
* Texture Loading
* Lighting with fragment shaders (Phong lightning model)
* Materials & Diffuse maps
* Interfaces for test settings


**Dependencies**
* GLFW3
* GLAD
* OpenGL 3.3
* GLM
* Dear ImGui
* stb_image
