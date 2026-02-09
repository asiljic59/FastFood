#ifndef VBO_CLASS_H
#define VBO_CLASS_H
#include<glad/glad.h>
#include<glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

class VBO {
	public:
		GLuint ID;
		size_t size;

		VBO();
		VBO(std::vector <Vertex>& vertices);
		
		void Bind();
		void Unbind();
		void Delete();
		void Update(GLfloat* vertices);
};

#endif // !VBO_CLASS_H

