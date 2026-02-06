// ColorMesh3D.cpp
#include "ColorMesh.h"

/*

ColorMesh::ColorMesh(float* vertices, size_t vertexSize, GLuint* indices, size_t indexSize)
    : vbo(vertices, vertexSize), ebo(indices, indexSize)
{
    vao.Bind();

    // Format: X, Y, Z (3 floats for position)
    unsigned int stride = 3 * sizeof(float);

    // Position attribute (location = 0)
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, stride, (void*)0);

    indexCount = indexSize / sizeof(GLuint);

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void ColorMesh::Draw()
{
    vao.Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
*/
