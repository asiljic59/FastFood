#pragma once
#include "VAO.h"
#include "VBO.h"
#include "shaderClass.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "VAO.h"
#include "EBO.h"
#include "shaderClass.h"
#include "cameraClass.h"
#include "Texture.h"
class Mesh {
public:
    std::vector <Vertex> vertices;
    std::vector <GLuint> indices;
    std::vector <Texture> textures;


    VAO VAO;

    Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

    void Draw(Shader& shader, Camera& camera);
};
