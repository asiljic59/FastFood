#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include "stb/stb_image.h"
#include "shaderClass.h"
#include "VAO.h"


class Texture {
public:
    GLuint ID;
    GLenum unit;
    GLenum unitIndex;


    Texture();
    Texture(const char* image, GLenum texUnit);
    void Draw(Shader& shader, VAO& vao, float x, float y, float scale);

    void Bind();
    void Unbind();
    void Delete();
};

#endif
