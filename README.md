# 3D-OpenGL-TestBed
An OpenGL rendering experiment.
This project is used to test OpenGL features and rendering techniques in hopes of creating something more modular for learning purposes.

In this project I am using modern OpenGL 3.3+ and soon to use DSA(Direct-State-Access) functions.

**What you can currently see in this test bed:**
* 3D transformations: [ Eular Angles, Panning , Movement ]
* Camera Class & Projections: local space -> world space -> view space -> clip space -> screen space
* Texture Loading
* Lighting with fragment shaders (Phong lightning model)
* Materials & Diffuse maps
* Interfaces to test settings  

**Running:** Build from source and grab the dependencies or download the binary from Releases (requires the res folder assets).

---

**Dependencies**  
*Add these to your search directories:*
* GLFW3
* GLAD
* OpenGL 3.3
* GLM  

*Already included as header files:*  
* Dear ImGui
* stb_image
