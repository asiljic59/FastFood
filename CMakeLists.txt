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


file(COPY
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/start_button.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/table.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/plate.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/smederevac.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/patty.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/bottom_bun.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/top_bun.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/name.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/lettuce.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/onion.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/pickles.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/ketchup.png
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/mustard.png 
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/ketchup_splash.png 
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/mustard_splash.png 
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/ketchup_layer.png 
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/mustard_layer.png 	
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/cheese.png 
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/greetings.png  
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/tomato.png 	
    DESTINATION ${CMAKE_BINARY_DIR}
)

file(COPY
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/default.vert
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/default.frag
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/patty.vert
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/patty.frag
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/color.vert
    ${CMAKE_SOURCE_DIR}/FastFood_SV_50_2022/color.frag
    DESTINATION ${CMAKE_BINARY_DIR}
)
