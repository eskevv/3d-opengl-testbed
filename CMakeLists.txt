# SETUP
cmake_minimum_required(VERSION 3.20)
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_ID)
string(REPLACE " " "_" PROJECT_ID ${PROJECT_ID})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(${PROJECT_ID})
option(CMAKE_BUILD_TYPE Release)

# SOURCE
aux_source_directory("src" PROJECT_SRC)
aux_source_directory("vendor/glad/src" GLAD)

# TARGETS
add_executable(${PROJECT_NAME}
  ${PROJECT_SRC}
  ${GLAD}
)

add_subdirectory(vendor/assimp)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)

add_dependencies(${PROJECT_NAME} imgui)

# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
# target_compile_options(${PROJECT_NAME} PRIVATE
#   $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
#   $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
# )

# INCLUDES
target_include_directories(${PROJECT_NAME} PRIVATE
  "include"
  "vendor/glfw/include"
  "vendor/assimp/include"
  "vendor/glad/include"
  "vendor/imgui/include"
  "vendor/glm"
  "vendor/stb-image"
)
target_include_directories(imgui PRIVATE vendor/glfw/include)

target_link_libraries(imgui PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp glfw imgui opengl32 gdi32)
