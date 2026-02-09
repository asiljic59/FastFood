// ColorMesh3D.cpp
#include "ColorMesh.h"

ColorMesh::ColorMesh(
    std::vector<Vertex>& vertices,
    std::vector<GLuint>& indices
)
{
    indexCount = static_cast<GLuint>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_DYNAMIC_DRAW
    );

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    GLsizei stride = sizeof(Vertex);

    // position (location = 0)
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        stride,
        (void*)offsetof(Vertex, position)
    );
    glEnableVertexAttribArray(0);

    // normal (location = 1)
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        stride,
        (void*)offsetof(Vertex, normal)
    );
    glEnableVertexAttribArray(1);

    // color (location = 2)
    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE,
        stride,
        (void*)offsetof(Vertex, color)
    );
    glEnableVertexAttribArray(2);

    // uv (location = 3)
    glVertexAttribPointer(
        3, 2, GL_FLOAT, GL_FALSE,
        stride,
        (void*)offsetof(Vertex, texUV)
    );
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}
void ColorMesh::UpdateVertices(std::vector<Vertex>& vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        vertices.size() * sizeof(Vertex),
        vertices.data()
    );
}

void ColorMesh::Draw()
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}