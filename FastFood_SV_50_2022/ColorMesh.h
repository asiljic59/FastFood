#pragma once
#include "VAO.h"
#include "VBO.h"
#include "shaderClass.h"

class ColorMesh
{
public:
    VAO vao;
    VBO vbo;
    unsigned int vertexCount = 0;

    ColorMesh(float* vertices, size_t vertexSize);
    void Draw(Shader& shader, float x, float y, float scaleX, float scaleY);
    void UpdateVertices(float* verts)
    {
        vbo.Update(verts);
    }
};
