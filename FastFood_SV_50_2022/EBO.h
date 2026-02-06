#ifndef EBO_CLASS_H
#define EBO_CLASS_H
#include<glad/glad.h>
#include "VBO.h"

class EBO {
public:
	GLuint ID;
	EBO(std::vector <GLuint>& indices);
	void Bind();
	void Unbind();
	void Delete();
};

#endif // !VBO_CLASS_H

