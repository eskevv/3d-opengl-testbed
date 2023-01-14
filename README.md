# 3D-OpenGL-TestBed
An OpenGL rendering experiment.
This project is used to test OpenGL features and rendering techniques in hopes of creating something more modular for learning purposes.

In this project I am using modern OpenGL 3.3+ and soon to use DSA(Direct-State-Access) functions.

**What's currently in the build:**
* 3D transformations:  

   *1. [ Eular Angles, Panning , Movement ]*  
   
* Camera Class & Projections: local space -> world space -> view space -> clip space -> screen space
* Texture Loading
* Lighting with fragment shaders (Phong lightning model):  

   *1. [ Directional, Point lights, Spot lights]*  
   *2. Attenuation (light perception across distances)*  
   
* Smoothed light boundaries
* Materials & Diffuse maps
* Interface editor 

**To-Implement:**  
* Models
* Skeletal Animations
* SkyBoxes
* Shadows

![alt](https://github.com/eskevv/3D-OpenGL-TestBed/blob/main/preview.png?raw=true)


---
### Running: 
*Option 1.* Build from source and grab the dependencies (builds with CMake).  
>*1. (optional) adjust the CMakeLists where to search for dependencies.*  
>*2. from root dir (using Ninja): ```cmake -B build -G Ninja && cmake --build build```*  

*Option 2.* (not always up to date) Download the binary from Releases 
>*1. requires the res folder in the same directory.*

**Dependencies (already linked)**  
* *OpenGL 3.3+*
* *GLFW3*    
* *GLAD*  
* *GLM*  
* *Dear ImGui*  
* *stb_image*  
