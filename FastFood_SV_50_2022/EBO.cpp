#include"EBO.h"
#include "vector"
// GENERISE SE NOVI BAFER I LINKUJE SE SA VEKTORIMA
EBO::EBO(std::vector<GLuint>& indices)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

// Binding
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

// Unbindining
void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Brisanje VBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}