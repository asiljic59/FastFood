#include "Mesh.h"
#include <cmath>

Mesh::Mesh() {}

Mesh::Mesh(float* vertices, size_t vertexSize)
    : vbo(vertices, vertexSize)
{
    // Copy first 8 floats (4 verts x XY only)
    for (int i = 0; i < 8; i++)
        verticesCopy[i] = vertices[i];

    vao.Bind();

    unsigned int stride = 4 * sizeof(float);

    vao.LinkAttrib(vbo, 0, 2, GL_FLOAT, stride, (void*)0);
    vao.LinkAttrib(vbo, 1, 2, GL_FLOAT, stride, (void*)(2 * sizeof(float)));

    vertexCount = vertexSize / (4 * sizeof(float));
}

void Mesh::Bind()
{
    vao.Bind();
}

void Mesh::Draw()
{
    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);
}

void Mesh::DrawColored(Shader& shader, float x, float y, float scaleX, float scaleY, float* color)
{
    shader.Activate();

    glUniform1i(glGetUniformLocation(shader.ID, "useSolidColor"), 1);

    glUniform4f(
        glGetUniformLocation(shader.ID, "solidColor"),
        color[0], color[1], color[2], color[3]
    );

    glUniform2f(glGetUniformLocation(shader.ID, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader.ID, "uScale"), scaleX, scaleY);

    vao.Bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexCount);

    glUniform1i(glGetUniformLocation(shader.ID, "useSolidColor"), 0);
}
