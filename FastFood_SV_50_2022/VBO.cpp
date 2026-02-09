#include"VBO.h"


VBO::VBO() {

}
// GENERISE SE NOVI BAFER I LINKUJE SE SA VEKTORIMA
VBO::VBO(std::vector <Vertex>& vertices)
{
	this->size = size;  // <-- save size

	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
}

// Binding
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

// Unbindining
void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Brisanje VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}

void VBO::Update(float* vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
}