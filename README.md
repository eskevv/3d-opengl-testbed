# 3D-OpenGL-TestBed
An OpenGL rendering experiment.
This project is used to test OpenGL features and rendering techniques in hopes of creating something more modular for learning purposes.

In this project I am using modern OpenGL 3.3+ and soon to use DSA(Direct-State-Access) functions.

**What you can currently see in this test bed:**
* 3D transformations:  

   *1. [ Eular Angles, Panning , Movement ]*  
   
* Camera Class & Projections: local space -> world space -> view space -> clip space -> screen space
* Texture Loading
* Lighting with fragment shaders (Phong lightning model):  

   *1. [ Directional, Point lights, Spot lights]*  
   *2. Attenuation (light perception across distances)*  
   
* Smoothed light boundaries
* Materials & Diffuse maps
* Interfaces to test settings  

**To-Implement:**  
* Models
* Skeletal Animations
* SkyBoxes
* Shadows

**Running:**  
*Option 1.* Build from source and grab the dependencies (builds with CMake).  
>*1. you need to adjust the CMakeLists where to search for dependencies.*  
>*2. from root dir: ```cmake -B build -G Ninja && cmake --build build```*  

*Option 2.* Download the binary from Releases (requires the res folder in the same directory).

---

![alt](https://github.com/eskevv/3D-OpenGL-TestBed/blob/main/preview.png?raw=true)

**Dependencies**  
*Requires:*
* OpenGL 3.3+

*Add these to your search directories:*
* GLFW3
* GLAD
* GLM  

*Already included as header files:*  
* Dear ImGui
* stb_image
