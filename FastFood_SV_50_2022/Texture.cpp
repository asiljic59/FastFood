    #include "Texture.h"
    #include <iostream>

    Texture::Texture(){}

    Texture::Texture(const char* image, GLenum texUnit)
    {
        unit = texUnit;

        int width, height, numColCh;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* bytes = stbi_load(image, &width, &height, &numColCh, 0);

        if (!bytes) {
            std::cout << "Failed to load texture: " << image << std::endl;
            return;
        }

        glGenTextures(1, &ID);
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        GLenum internalFormat;
        GLenum dataFormat;

        if (numColCh == 4) {
            internalFormat = GL_RGBA;
            dataFormat = GL_RGBA;
        }
        else if (numColCh == 3) {
            internalFormat = GL_RGB;
            dataFormat = GL_RGB;
        }
        else {
            std::cout << "Unsupported channel count: " << numColCh << std::endl;
            stbi_image_free(bytes);
            return;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(bytes);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Draw(Shader& shader, VAO& vao, float x, float y, float scale)
    {
        glUniform2f(glGetUniformLocation(shader.ID, "uPos"), x, y);
        glUniform2f(glGetUniformLocation(shader.ID, "uScale"), scale, scale);
        glUniform1i(glGetUniformLocation(shader.ID, "tex0"), unit - GL_TEXTURE0);

        Bind();
        vao.Bind();

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }



    void Texture::Bind()
    {
        glActiveTexture(unit);
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    void Texture::Unbind()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::Delete()
    {
        glDeleteTextures(1, &ID);
    }
