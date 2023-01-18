cmake_minimum_required(VERSION 3.25)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectId)
string(REPLACE " " "_" ProjectId ${ProjectId})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
project(${ProjectId})

add_subdirectory(vendor/glfw-3.3.8 SYSTEM)
add_subdirectory(vendor/assimp SYSTEM)
add_subdirectory(vendor/glm-master SYSTEM)

aux_source_directory("src" Src)
aux_source_directory("vendor/imgui-1.89.2/imgui" ImGui)
aux_source_directory("vendor/glad-3.3core/src" Glad)

add_executable(${PROJECT_NAME}
  ${Src}
  ${ImGui}
  ${Glad}
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_include_directories(${PROJECT_NAME} PRIVATE
  "src/headers"
  "vendor/glfw-3.3.8/include"
  "vendor/assimp/include"
  "vendor/glad-3.3core/include"
  "vendor/glm-master"
  "vendor/imgui-1.89.2/"
  "vendor/stb-image"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw
  assimp
  opengl32
  gdi32
)
