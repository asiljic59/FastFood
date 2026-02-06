// ColorMesh3D.h
#ifndef COLORMESH3D_CLASS_H
#define COLORMESH3D_CLASS_H

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

class ColorMesh {
public:
    VAO vao;
    VBO vbo;
    EBO ebo;
    unsigned int indexCount;

    ColorMesh(float* vertices, size_t vertexSize, GLuint* indices, size_t indexSize);
    void Draw();
};

#endif