#pragma once
#include "VAO.h"
#include "VBO.h"
#include "shaderClass.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
class Mesh {
public:
    VAO vao;
    VBO vbo;
    unsigned int vertexCount;

    std::vector<glm::vec3> positions;



    Mesh();
    Mesh(float* vertices, size_t vertexSize);

    void Bind();
    void Draw();
    void DrawColored(Shader& shader, float x, float y, float scaleX, float scaleY, float* color);

};
