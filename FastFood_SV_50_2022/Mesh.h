#pragma once
#include "VAO.h"
#include "VBO.h"
#include "shaderClass.h"

class Mesh {
public:
    VAO vao;
    VBO vbo;
    unsigned int vertexCount;
    float verticesCopy[8];


    Mesh();
    Mesh(float* vertices, size_t vertexSize);

    void Bind();
    void Draw();
    void DrawColored(Shader& shader, float x, float y, float scaleX, float scaleY, float* color);

};
