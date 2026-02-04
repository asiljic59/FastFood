#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "stb/stb_image.h"

#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "callbacks.h"
#include "Texture.h"
#include "Mesh.h"
#include "ColorMesh.h"
#include "cameraClass.h"


const int ORDER_COUNT = 10;

//za dugme na pocetku
struct Button {
	Mesh mesh;
	Texture texture;
	float x, y;     // center position
	float w, h;     // width and height
	bool clicked = false;
};
/// <summary>
/// Stavke koje se stavljaju na burger
/// texture -> tekstura obuhvacena stavkom
/// mesh -> VAO + VBO
/// x -> koordinata koja se pamti
/// y -> koordinata koja se pamti
/// cooked -> sluzi samo za patty
/// </summary>
struct BurgerPart {
	Texture* texture;
	Mesh* mesh;
	float x;
	float y;
	bool cooked;
};
//za ketchup i senf
struct Spill {
	Texture* texture;
	float x;
	float y;
};
//provera da li je cursor na dugmetu

bool IsInsideButton(Button b, float mx, float my)
{
	return mx > (b.x - b.w) &&
		mx < (b.x + b.w) &&
		my >(b.y - b.h) &&
		my < (b.y + b.h);
}
//da li je burger na sporetu??
bool PattyOverStove()
{
	float stoveMinX = -1.0f;
	float stoveMaxX = 0.0f;
	float stoveMinY = -1.0f;
	float stoveMaxY = 0.0f;

	float pattyWidthHalf = 0.1f; // 0.2 total width
	float pattyHeightHalf = 0.1f; // 0.2 total height

	//uzimamo koordinate patty tako sto x,y umanjimo/povercamo sa poloviom duzine/visine i dobijemo granice
	float pattyMinX = uX - pattyWidthHalf;
	float pattyMaxX = uX + pattyWidthHalf;
	float pattyMinY = uY - pattyHeightHalf;
	float pattyMaxY = uY + pattyHeightHalf;

	bool overlapX = !(pattyMaxX < stoveMinX || pattyMinX > stoveMaxX);
	bool overlapY = !(pattyMaxY < stoveMinY || pattyMinY > stoveMaxY);

	return overlapX && overlapY;
}
//loadovanje curosra
GLFWcursor* loadImageToCursor(const char* filePath) {
	int TextureWidth;
	int TextureHeight;
	int TextureChannels;

	unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);

	if (ImageData != NULL)
	{
		GLFWimage image;
		image.width = TextureWidth;
		image.height = TextureHeight;
		image.pixels = ImageData;

		// Tacka na površini slike kursora koja se ponaša kao hitboks
		int hotspotX = TextureWidth / 5;
		int hotspotY = TextureHeight / 6;

		GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);
		stbi_image_free(ImageData);
		return cursor;
	}
	else {
		std::cout << "Kursor nije ucitan! Putanja kursora: " << filePath << std::endl;
		stbi_image_free(ImageData);

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

//delovi burgera
const int MAX_PARTS = 10;

//vec postavljeni delovi burgera koji ima maksimalno koliko i delova
BurgerPart placed[MAX_PARTS];
int placedCount = 0;
int currentItem = 0;

//za ketchup i senf imamo i spill gde se pamti i iscrtava svaki put kad ne pritisnemo space iznad burgera
Spill spills[50];
int spillCount = 0;

GLFWcursor* cursor;


int main()
{
	// Initialize GLFW
	if (!glfwInit()) {
		std::cout << "GLFW Biblioteka se nije ucitala! :(\n";
		return 1;
	};



	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get the primary monitor
	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	// Get monitor video mode (resolution + refresh rate)
	const GLFWvidmode* mode = glfwGetVideoMode(primary);

	// Create FULLSCREEN window
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "YoutubeOpenGL", primary, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	//vezivanje callbackova
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	cursor = loadImageToCursor("spatula.png");
	glfwSetCursor(window, cursor);

	Shader shaderProgram = Shader("default.vert","default.frag");

    // ===== GORNJA PLOČA ŠPORETA (samo top face) =====
    float stoveTopVertices[] = {
        // X,     Y,     Z,      R,    G,    B,     U,    V
        // Top face - glavna površina za kuvanje
        -1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
         1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        -1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        -1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
    };

    // ===== BOČNE STRANE ŠPORETA (sve osim top face) =====
    float stoveSidesVertices[] = {
        // X,     Y,     Z,      R,    G,    B,     U,    V

        // Front face
        -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
         1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        -1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
        -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

        // Back face
        -1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        -1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
        -1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,

        // Left face
        -1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
        -1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
        -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
        -1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
        -1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

        // Right face
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
         1.0f,  0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
         1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         1.0f,  0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

         // Bottom face
         -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
          1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
          1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
          1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
         -1.0f, -0.1f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f,
         -1.0f, -0.1f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f
    };

    // ===== SVE 4 NOGE (male kocke na uglovima) =====
    float allLegsVertices[] = {
        // X,     Y,     Z,      R,    G,    B,     U,    V

        // ===== NOGA 1: Levo-napred (-0.9, -0.5, -0.4) =====
        // Front face
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Back face
        -0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Left face
        -0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        // Right face
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Bottom face
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Top face
        -0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,

        // ===== NOGA 2: Desno-napred (0.9, -0.5, -0.4) =====
        // Front face
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Back face
        0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Left face
        0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        // Right face
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Bottom face
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.5f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Top face
        0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.1f, -0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.1f, -0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,

        // ===== NOGA 3: Levo-pozadi (-0.9, -0.5, 0.4) =====
        // Front face
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Back face
        -0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Left face
        -0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        // Right face
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Bottom face
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Top face
        -0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        -0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        -0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        -0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,

        // ===== NOGA 4: Desno-pozadi (0.9, -0.5, 0.4) =====
        // Front face
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Back face
        0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        // Left face
        0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        // Right face
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Bottom face
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.5f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.5f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        // Top face
        0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f,
        0.95f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  1.0f, 1.0f,
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.95f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f,
        0.85f, -0.1f, 0.45f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f,
        0.85f, -0.1f, 0.35f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f
    };
    Texture stoveTopTex("sporet_vrh.png", GL_TEXTURE0);
    Texture stoveSideTex("sporet_bocna.png", GL_TEXTURE1);
    Texture legTex("metal.png", GL_TEXTURE2);


    Mesh stoveTopMesh(stoveTopVertices, sizeof(stoveTopVertices));
    Mesh stoveSidesMesh(stoveSidesVertices, sizeof(stoveSidesVertices));
    Mesh legsAll(allLegsVertices, sizeof(allLegsVertices));

    
    glEnable(GL_DEPTH_TEST);



	Camera camera((float) mode->width,(float) mode->height, glm::vec3(0.0f, 0.0f, 2.0f));

	// Add this debug check
	std::cout << "Camera position: " << camera.Position.x << ", "
		<< camera.Position.y << ", " << camera.Position.z << std::endl;

	int frameCount = 0;

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Check if shader is active
		GLint currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		if (frameCount == 0) {
			std::cout << "Shader ID: " << shaderProgram.ID << ", Current: " << currentProgram << std::endl;
		}
		

        // Crtaj gornju ploču sa svojom teksturom
        stoveTopTex.Bind();  // ← koristi stoveTopTex umesto smederevac
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 0);  // texture unit 0
        stoveTopMesh.Draw();

        // Crtaj bočne strane sa svojom teksturom
        stoveSideTex.Bind();  // ← nova tekstura
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 1);  // texture unit 1
        stoveSidesMesh.Draw();

        // Crtaj noge sa svojom teksturom
        legTex.Bind();  // ← nova tekstura
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "tex0"), 2);  // texture unit 2
        legsAll.Draw();

		// Check for OpenGL errors each frame
		GLenum err = glGetError();
		if (err != GL_NO_ERROR && frameCount < 5) {
			std::cout << "OpenGL error in render loop: " << err << std::endl;
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		frameCount++;
		if (frameCount == 1) {
			std::cout << "First frame rendered" << std::endl;
		}
	}



	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}