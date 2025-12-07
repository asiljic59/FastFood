#include "ColorMesh.h"

ColorMesh::ColorMesh(float* vertices, size_t vertexSize)
    : vbo(vertices, vertexSize)
{
    vao.Bind();

    /// Format of vertex array:
    /// X,Y,R,G,B,A  → 6 floats
    unsigned int stride = 6 * sizeof(float);

    // Position attribute
    vao.LinkAttrib(vbo, 0, 2, GL_FLOAT, stride, (void*)0);

    // Color attribute
    vao.LinkAttrib(vbo, 1, 4, GL_FLOAT, stride, (void*)(2 * sizeof(float)));

    vertexCount = vertexSize / (6 * sizeof(float));

    vao.Unbind();
    vbo.Unbind();
}

void ColorMesh::Draw(Shader& shader, float x, float y, float scaleX, float scaleY)
{
    shader.Activate();

    glUniform2f(glGetUniformLocation(shader.ID, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader.ID, "uScale"), scaleX, scaleY);

    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
}
