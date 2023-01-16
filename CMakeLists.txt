cmake_minimum_required(VERSION 3.25)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME ProjectId)
string(REPLACE " " "_" ProjectId ${ProjectId})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
project(${ProjectId})

aux_source_directory("src" Src)
aux_source_directory("src/gl_state" GlState)
aux_source_directory("src/models" Models)
aux_source_directory("src/transformations" Transformations)
aux_source_directory("vendor/include/imgui" ImGui)
add_executable(${ProjectId}
   ${Src}
   ${ImGui}
   ${GlState}
   ${Models}
   ${Transformations}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
target_include_directories(${PROJECT_NAME} PRIVATE
   "vendor/include"
   "src"
   "src/gl_state"
   "src/models"
   "src/transformations"
)
target_link_directories(${PROJECT_NAME} PRIVATE
   "vendor/lib"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
   glad
   glfw3
   opengl32
   gdi32
   assimp_shared
)