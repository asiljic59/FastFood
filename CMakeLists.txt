cmake_minimum_required(VERSION 3.10)

project(FastFood_SV_50_2022)

set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/FastFood_SV_50_2022)

include_directories(
    ${SRC_DIR}
    ${SRC_DIR}/Libraries/include
)

set(SOURCES
    ${SRC_DIR}/Main.cpp
    ${SRC_DIR}/callbacks.cpp
    ${SRC_DIR}/key_callback.cpp
    ${SRC_DIR}/stb.cpp
    ${SRC_DIR}/shaderClass.cpp
    ${SRC_DIR}/Texture.cpp
    ${SRC_DIR}/Mesh.cpp
    ${SRC_DIR}/ColorMesh.cpp
    ${SRC_DIR}/VAO.cpp
    ${SRC_DIR}/VBO.cpp
    ${SRC_DIR}/EBO.cpp
    ${SRC_DIR}/glad.c
)

add_executable(FastFood_SV_50_2022 ${SOURCES})

add_library(glfw3 STATIC IMPORTED)
set_target_properties(glfw3 PROPERTIES
    IMPORTED_LOCATION "${SRC_DIR}/Libraries/lib/glfw3.lib"
)

target_link_libraries(FastFood_SV_50_2022 glfw3 opengl32)
