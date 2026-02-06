//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------
#include "Mesh.h"



const unsigned int width = 800;
const unsigned int height = 800;


Vertex vertices[] =
{
	// ===== Front (+Z) =====
	{ {-0.5f, 0.0f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,0.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f,0.0f} },

	// ===== Back (-Z) =====
	{ {-0.5f, 0.0f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f,0.0f} },
	{ { 0.5f, 0.0f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f,0.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f,0.0f} },
	{ {-0.5f, 0.8f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,0.0f} },

	// ===== Left (-X) =====
	{ {-0.5f, 0.0f, -0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f,0.0f} },
	{ {-0.5f, 0.0f,  0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f,0.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,0.0f} },
	{ {-0.5f, 0.8f, -0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f,0.0f} },

	// ===== Right (+X) =====
	{ { 0.5f, 0.0f, -0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f,0.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,0.0f} },

	// ===== Bottom (-Y) =====
	{ {-0.5f, 0.0f, -0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f,0.0f} },
	{ { 0.5f, 0.0f, -0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f,0.0f} },
	{ {-0.5f, 0.0f,  0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f,0.0f} },

	// ===== Top (+Y) =====
	{ {-0.5f, 0.8f, -0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {0.0f,0.0f,0.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {1.0f,0.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {1.0f,1.0f,0.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {0.0f,1.0f,0.0f} }
};

GLuint indices[] =
{
	 0,  1,  2,   0,  2,  3,   // Front
	 4,  5,  6,   4,  6,  7,   // Back
	 8,  9, 10,   8, 10, 11,   // Left
	12, 13, 14,  12, 14, 15,   // Right
	16, 17, 18,  16, 18, 19,   // Bottom
	20, 21, 22,  20, 22, 23    // Top
};



Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};


GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};


void CreatePljeskavica(
	float radius,
	float height,
	int segments,
	std::vector<Vertex>& vertices,
	std::vector<GLuint>& indices
) {
	float halfH = height * 0.5f;

	// Center vertices
	vertices.push_back({ {0,  halfH, 0}, {0,1,0}, {0.6f,0.3f,0.2f}, {0,0,0} }); // top center
	vertices.push_back({ {0, -halfH, 0}, {0,-1,0}, {0.6f,0.3f,0.2f}, {0,0,0} }); // bottom center

	int topCenter = 0;
	int bottomCenter = 1;

	// Circle vertices
	for (int i = 0; i <= segments; i++) {
		float angle = 2.0f * glm::pi<float>() * i / segments;
		float x = radius * cos(angle);
		float z = radius * sin(angle);

		vertices.push_back({ {x,  halfH, z}, {0,1,0}, {0.6f,0.3f,0.2f}, {0,0,0} });
		vertices.push_back({ {x, -halfH, z}, {0,-1,0}, {0.6f,0.3f,0.2f}, {0,0,0} });
	}

	// Indices
	for (int i = 0; i < segments; i++) {
		int t1 = 2 + i * 2;
		int b1 = t1 + 1;
		int t2 = t1 + 2;
		int b2 = b1 + 2;

		// Top
		indices.push_back(topCenter);
		indices.push_back(t1);
		indices.push_back(t2);

		// Bottom
		indices.push_back(bottomCenter);
		indices.push_back(b2);
		indices.push_back(b1);

		// Side
		indices.push_back(t1);
		indices.push_back(b1);
		indices.push_back(b2);

		indices.push_back(t1);
		indices.push_back(b2);
		indices.push_back(t2);
	}

}

//Pomeraj uX, uY za aktivnu teksturu
float uX = 0.0f;
float uY = 0.0f;

//pratimo progres kuhanja
float cookingProgress = 0.0f;


//porudzbina pocela, burger pecen
bool orderStarted = false;
bool pattyCooked = false;

//kao neka vrsta cooldown metode jer kad se pritisne za ketcuhp on odmah doda i za senf
bool spacePressed = false;
bool spaceWasDown = false;

int main()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);



	Texture textures[]{
		Texture("metal.png","diffuse",0,GL_RGBA,GL_UNSIGNED_BYTE),
		Texture("spec.png","specular",1,GL_RED,GL_UNSIGNED_BYTE)
	};

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> inds(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	 
	Mesh stove(verts, inds, tex);

	Shader lightShader("light.vert", "light.frag");
	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector<GLuint> lightInd(
		lightIndices,
		lightIndices + sizeof(lightIndices) / sizeof(GLuint)
	);
	std::vector<Texture> noTextures;
	Mesh light(lightVerts, lightInd, noTextures);

	std::vector<Vertex> burgerVerts;
	std::vector<GLuint> burgerInds;
	Texture pljeskavicaTex[] = {
		Texture("meat_diffuse.png", "diffuse", 0, GL_RGB, GL_UNSIGNED_BYTE),
		Texture("meat_spec.png",    "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	std::vector<Texture> meatTex(
		pljeskavicaTex,
		pljeskavicaTex + 2
	);
	CreatePljeskavica(0.5f, 0.15f, 32, burgerVerts, burgerInds);
	Mesh burger(burgerVerts, burgerInds, meatTex);


	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.7f, 0.7f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 stovePos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 stoveModel = glm::mat4(1.0f);
	stoveModel = glm::translate(stoveModel, stovePos);


	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(stoveModel));
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		stove.Draw(shaderProgram,camera);
		light.Draw(lightShader, camera);
		burger.Draw(shaderProgram, camera);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	shaderProgram.Delete();
	lightShader.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}