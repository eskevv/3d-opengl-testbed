# 3D-OpenGL-TestBed
An OpenGL rendering experiment.
This project is used to test OpenGL features and rendering techniques in hopes of creating something more modular for learning purposes.

In this project I am using modern OpenGL 3.3+ and soon to use DSA(Direct-State-Access) functions.

**What's currently in the build:**
* 3D transformations:  

   1. *[ Eular Angles, Panning , Movement ]*  
   
* Camera Class & Projections:
   1. *local space -> world space -> view space -> clip space -> screen space*
* Texture Loading
* Mesh Loading
* Lighting (Phong lightning model):  

   1. *[ Directional, Point lights, Spot lights]*  
   2. *Attenuation (light perception across distances)*  
   3. *Smoothed lighting*
   4. *Materials & Texture mapping*
   
* Editor (Immediate-State GUI)

**To-Implement:**  
- [x] ~~Interface editor~~ (01/12/2023)  
- [x] ~~Materials & Diffuse maps~~ (01/14/2023)  
- [x] ~~Models~~ (01/15/2023)  
- [ ] Skeletal Animations  
- [ ] SkyBoxes  
- [ ] Shadows  

![alt](https://github.com/eskevv/3D-OpenGL-TestBed/blob/main/preview.gif?raw=true)

---
### Running: 
*Option 1.* Build from source and grab the dependencies (builds with CMake).  
>-> *(optional) adjust the CMakeLists where to search for dependencies.*  
>-> *from root dir (using Ninja): ```cmake -B build -G Ninja && cmake --build build```*   

*Option 2.* Download the binary from Releases 
>-> *not always up to date*  
>-> *requires the res folder in the same directory.*

**Dependencies (already linked)**  
* *OpenGL 3.3+*  (API specification)
* *GLFW3*  (os facilitations)
* *GLAD*  (opengl functions)
* *assimp* (mesh parser)
* *stb_image* (texture loading)
* *GLM*  (optimized math library)
* *Dear ImGui*  (immediate mode GUI) 

