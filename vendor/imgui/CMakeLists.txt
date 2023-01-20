cmake_minimum_required(VERSION 3.20)
project(imgui-1.89)

aux_source_directory(src ALL_SRC)
aux_source_directory(include HEADERS)

add_library(imgui SHARED ${ALL_SRC} ${HEADERS})
target_include_directories(imgui PRIVATE include/imgui)