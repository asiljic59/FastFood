#include "Mesh.h"
#include <cmath>

Mesh::Mesh() {}
Mesh::Mesh(float* vertices, size_t vertexSize)
    : vbo(vertices, vertexSize)
{
    vao.Bind();

    unsigned int stride = 8; // 3 pos + 3 color/normal + 2 UV
    unsigned int strideBytes = stride * sizeof(float);

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, strideBytes, (void*)0);           // position
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, strideBytes, (void*)(3 * sizeof(float))); // normal/color
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, strideBytes, (void*)(6 * sizeof(float))); // UV

    vertexCount = vertexSize / (stride * sizeof(float)); // broj vertiksa
}

void Mesh::Draw()
{
    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Mesh::DrawColored(Shader& shader, float x, float y, float scaleX, float scaleY, float* color)
{
    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, "useSolidColor"), 1);
    glUniform4f(glGetUniformLocation(shader.ID, "solidColor"),
        color[0], color[1], color[2], color[3]);
    glUniform2f(glGetUniformLocation(shader.ID, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader.ID, "uScale"), scaleX, scaleY);

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertexCount); // TRIANGLES, ne STRIP

    glUniform1i(glGetUniformLocation(shader.ID, "useSolidColor"), 0);
}
