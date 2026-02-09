// ColorMesh3D.h
#ifndef COLORMESH3D_CLASS_H
#define COLORMESH3D_CLASS_H

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "shaderClass.h"

class ColorMesh
{
public:
    ColorMesh(std::vector<Vertex>& vertices,
        std::vector<GLuint>& indices);

    void UpdateVertices(std::vector<Vertex>& vertices);
    void Draw();

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    GLuint indexCount = 0;
};


#endif