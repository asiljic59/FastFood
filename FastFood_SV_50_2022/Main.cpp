#include<filesystem>
namespace fs = std::filesystem;
#include "Mesh.h"
#include "Model.h"
#include "ColorMesh.h"
#include <numbers>

#include <chrono>
#include <thread>

class DrawableItem {
public:
	virtual void Draw(Shader& shader, Camera& camera, glm::mat4 transform) = 0;
	virtual ~DrawableItem() = default;
};

class ModelItem : public DrawableItem {
private:
	Model* model;
	glm::vec3 color;
public:
	ModelItem(Model* m, glm::vec3 col = glm::vec3(1.0f)) : model(m), color(col) {}

	void Draw(Shader& shader, Camera& camera, glm::mat4 transform) override {
		shader.Activate();
		glUniform3f(glGetUniformLocation(shader.ID, "colorTint"), color.x, color.y, color.z);
		model->Draw(shader, camera, transform);
	}
};

class MeshItem : public DrawableItem {
private:
	Mesh* mesh;
	glm::vec3 color;
public:
	MeshItem(Mesh* m, glm::vec3 col) : mesh(m), color(col) {}

	void Draw(Shader& shader, Camera& camera, glm::mat4 transform) override {
		shader.Activate();
		glUniform3f(glGetUniformLocation(shader.ID, "colorTint"), color.x, color.y, color.z);
		mesh->Draw(shader, camera, transform);
	}
};




unsigned int width = 800;
unsigned int height = 800;


Vertex buttonVertices[] = {
	{ {-0.2f, -0.1f, 0.0f}, {0,0,1}, {1,1,1}, {0.0f, 0.0f} },
	{ {-0.2f,  0.1f, 0.0f}, {0,0,1}, {1,1,1}, {0.0f, 1.0f} },
	{ { 0.2f, -0.1f, 0.0f}, {0,0,1}, {1,1,1}, {1.0f, 0.0f} },
	{ { 0.2f,  0.1f, 0.0f}, {0,0,1}, {1,1,1}, {1.0f, 1.0f} }
};
GLuint buttonIndices[] = {
	0, 1, 2,
	2, 1, 3
};
Vertex vertices[] =
{
	{ {-0.5f, 0.0f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {0.0f,0.0f, 1.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f} },
	{ {-0.5f, 0.0f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f} },
	{ { 0.5f, 0.0f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f} },
	{ {-0.5f, 0.8f, -0.5f}, {0.0f,0.0f,-1.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f} },
	{ {-0.5f, 0.0f, -0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f} },
	{ {-0.5f, 0.0f,  0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f} },
	{ {-0.5f, 0.8f, -0.5f}, {-1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f} },
	{ { 0.5f, 0.0f, -0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {1.0f,0.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f} },
	{ {-0.5f, 0.0f, -0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,0.0f} },
	{ { 0.5f, 0.0f, -0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,0.0f} },
	{ { 0.5f, 0.0f,  0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {1.0f,1.0f} },
	{ {-0.5f, 0.0f,  0.5f}, {0.0f,-1.0f,0.0f}, {0.8f,0.8f,0.8f}, {0.0f,1.0f} },
	{ {-0.5f, 0.8f, -0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {0.0f,0.0f} },
	{ { 0.5f, 0.8f, -0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {1.0f,0.0f} },
	{ { 0.5f, 0.8f,  0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {1.0f,1.0f} },
	{ {-0.5f, 0.8f,  0.5f}, {0.0f,1.0f,0.0f}, {0.9f,0.9f,0.9f}, {0.0f,1.0f} }
};
GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2,

	4, 5, 6,
	4, 6, 7,

	8, 10, 9,
	8, 11, 10,

	12, 13, 14,
	12, 14, 15,

	16, 19, 18,
	16, 18, 17,

	20, 21, 22,
	20, 22, 23
};

std::vector<Vertex> progressBarVertices = {
	{
		glm::vec3(-1.0f, -0.9f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec2(0.0f,  1.0f)
	},

	{
		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec2(0.0f,  0.0f)
	},

	{
		glm::vec3(1.0f, -0.9f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec2(1.0f,  1.0f)
	},

	{
		glm::vec3(1.0f, -1.0f, 0.0f),
		glm::vec3(0.0f,  0.0f, 1.0f),
		glm::vec3(0.0f,  1.0f, 0.0f),
		glm::vec2(1.0f,  0.0f)
	}
};
std::vector<GLuint> progressBarIndices = {
0, 1, 2,
2, 1, 3
};


// ------------------- LIGHT -----------------------------------


Vertex lightVertices[] =
{
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

// ---------------------- NAME --------------------------------
Vertex nameVertices[] = {
	// top-left
	{ { 0.6f,  1.0f, 0.0f}, {0,0,1}, {1,1,1}, {0.0f, 1.0f} },
	// bottom-left
	{ { 0.6f,  0.8f, 0.0f}, {0,0,1}, {1,1,1}, {0.0f, 0.0f} },
	// top-right (ANCHOR POINT)
	{ { 1.0f,  1.0f, 0.0f}, {0,0,1}, {1,1,1}, {1.0f, 1.0f} },
	// bottom-right
	{ { 1.0f,  0.8f, 0.0f}, {0,0,1}, {1,1,1}, {1.0f, 0.0f} }
};

GLuint nameIndices[] = {
	0, 1, 2,
	2, 1, 3
};


std::vector<Vertex> generateBottleVertices(int segments = 16) {
	std::vector<Vertex> vertices;
	float cylinderHeight = 0.3f;
	float cylinderRadius = 0.05f;
	float coneHeight = 0.1f;

	for (int i = 0; i <= segments; i++) {
		float angle = (float)i / segments * 2.0f * glm::pi<float>();
		float x = cos(angle) * cylinderRadius;
		float z = sin(angle) * cylinderRadius;

		vertices.push_back({
			glm::vec3(x, 0.0f, z),
			glm::vec3(x, 0.0f, z),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec2((float)i / segments, 0.0f)
			});

		vertices.push_back({
			glm::vec3(x, cylinderHeight, z),
			glm::vec3(x, 0.0f, z),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec2((float)i / segments, 1.0f)
			});
	}

	vertices.push_back({
		glm::vec3(0.0f, -coneHeight, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec2(0.5f, 0.0f)
		});

	for (int i = 0; i <= segments; i++) {
		float angle = (float)i / segments * 2.0f * glm::pi<float>();
		float x = cos(angle) * cylinderRadius;
		float z = sin(angle) * cylinderRadius;

		vertices.push_back({
			glm::vec3(x, 0.0f, z),
			glm::vec3(x, -1.0f, z),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec2((float)i / segments, 1.0f)
			});
	}

	return vertices;
}

std::vector<GLuint> generateBottleIndices(int segments = 16) {
	std::vector<GLuint> indices;

	for (int i = 0; i < segments; i++) {
		int bottom1 = i * 2;
		int top1 = i * 2 + 1;
		int bottom2 = (i + 1) * 2;
		int top2 = (i + 1) * 2 + 1;

		indices.push_back(bottom1);
		indices.push_back(top1);
		indices.push_back(bottom2);

		indices.push_back(bottom2);
		indices.push_back(top1);
		indices.push_back(top2);
	}

	int tipIndex = (segments + 1) * 2;
	int coneBaseStart = tipIndex + 1;

	for (int i = 0; i < segments; i++) {
		indices.push_back(tipIndex);
		indices.push_back(coneBaseStart + i);
		indices.push_back(coneBaseStart + i + 1);
	}

	return indices;
}


struct BurgerPart {
	DrawableItem* item;
	float x, y, z;
	float scale;
	float height;
	bool cooked;
	glm::vec3 color;
};

const int MAX_PARTS = 10;
BurgerPart placed[MAX_PARTS];
int placedCount = 0;
int currentItem = 0;

const float BURGER_BASE_SCALE = 0.08f;


struct Spill {
	DrawableItem* item;
	float x, y, z;
	float scale;
};

const int MAX_SPILLS = 50;
Spill spills[MAX_SPILLS];
int spillCount = 0;

bool mouseButtonPressed = false;
bool orderStarted = false;
void mousePressedCallback(GLFWwindow* window) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouseButtonPressed)
	{
		mouseButtonPressed = true;

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float ndcX = (2.0f * mouseX) / width - 1.0f;
		float ndcY = 1.0f - (2.0f * mouseY) / height;

		float buttonMinX = -0.2f;
		float buttonMaxX = 0.2f;
		float buttonMinY = -0.1f;
		float buttonMaxY = 0.1f;

		if (ndcX >= buttonMinX && ndcX <= buttonMaxX &&
			ndcY >= buttonMinY && ndcY <= buttonMaxY)
		{
			std::cout << "Button clicked!" << std::endl;
			orderStarted = true;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		mouseButtonPressed = false;
	}
}
bool isPattyOverStove(glm::vec3 pattyPos, glm::vec3 stovePos) {
	float stoveMinX = stovePos.x - 0.5f;
	float stoveMaxX = stovePos.x + 0.5f;
	float stoveMinZ = stovePos.z - 0.5f;
	float stoveMaxZ = stovePos.z + 0.5f;

	float cookingY = 1.2f;

	bool overX = (pattyPos.x >= stoveMinX && pattyPos.x <= stoveMaxX);
	bool overZ = (pattyPos.z >= stoveMinZ && pattyPos.z <= stoveMaxZ);
	bool overY = (pattyPos.y >= stovePos.y - 0.2f && pattyPos.y <= stovePos.y + 0.1f);

	bool result = overX && overZ && overY;


	return result;
}
bool isAboveTable(float x, float z, glm::vec3 tablePos, float tableWidth = 3.5f, float tableDepth = 3.5f) {
	float halfWidth = tableWidth / 2.0f;
	float halfDepth = tableDepth / 2.0f;

	bool overX = (x >= tablePos.x - halfWidth) && (x <= tablePos.x + halfWidth);
	bool overZ = (z >= tablePos.z - halfDepth) && (z <= tablePos.z + halfDepth);

	return overX && overZ;
}
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
float uX = 0.0f;
float uY = 1.3f;
float uZ = 0.0f;

float stackY = -1.8f;

float calculateNextYPosition(const BurgerPart placedItems[], int placedCount, float itemHeight) {
	float topOfStack = -1.8f;

	for (int i = 0; i < placedCount; i++) {
		topOfStack += placedItems[i].height;
	}

	return topOfStack + itemHeight / 2.0f;
}

bool canPlaceItem(
	const glm::vec3& currentPos,
	const BurgerPart placedItems[],
	int placedCount,
	const BurgerPart& currentOrderItem,
	float xzTolerance = 0.2f
) {
	if (placedCount == 0) {
		return true;
	}

	const glm::vec3 prevPos(
		placedItems[placedCount - 1].x,
		placedItems[placedCount - 1].y,
		placedItems[placedCount - 1].z
	);

	float xDiff = fabs(currentPos.x - prevPos.x);
	float zDiff = fabs(currentPos.z - prevPos.z);

	return (xDiff <= xzTolerance && zDiff <= xzTolerance);
}

float getSpillYPos(float uX, float uZ) {
	if (uZ >= -4.52f && uZ < 4.56f && uX >= -3.48f && uX <= 3.11f) {
		return -0.6f;
	}
	else {
		return 3.1f;
	}
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	uY += yoffset * 0.2f;
}


float cookingProgress = 0.0f;


bool pattyCooked = false;

bool spacePressed = false;
bool spaceWasDown = false;

bool showLight = false;
bool lKeyWasDown = false;

bool depthTestEnabled = true;
bool zKeyWasDown = false;

bool cullingEnabled = true;
bool cKeyWasDown = false;

bool requiresSpace(int itemIndex) {
	return (itemIndex == 2 || itemIndex == 3);
}

GLFWcursor* cursor;


int main()
{
	std::cout << "About to load model..." << std::endl;

	try {
		std::cout << "Testing file read..." << std::endl;
		std::ifstream file("model/krabby_patty_burger/scene.gltf");
		if (!file.is_open()) {
			std::cerr << "Cannot open file!" << std::endl;
			return -1;
		}
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
		std::cout << "File read successfully, size: " << content.size() << std::endl;

		std::cout << "Parsing JSON..." << std::endl;
		json testJSON = json::parse(content);
		std::cout << "JSON parsed successfully" << std::endl;

	}
	catch (const std::exception& e) {
		std::cerr << "Error in test: " << e.what() << std::endl;
		return -1;
	}

	std::cout << "Creating Model object..." << std::endl;


	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWmonitor* primary = glfwGetPrimaryMonitor();

	const GLFWvidmode* mode = glfwGetVideoMode(primary);

	width = mode->width;
	height = mode->height;

	GLFWwindow* window = glfwCreateWindow(width, height, "FastFood", primary, NULL);

	glfwSetScrollCallback(window, scroll_callback);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	cursor = loadImageToCursor("spatula.png");
	glfwSetCursor(window, cursor);

	gladLoadGL();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glViewport(0, 0, width, height);


	Shader shaderProgram("default.vert", "default.frag");

	Texture textures[]{
		Texture("metal_diff.jpg","diffuse",0),
		Texture("metal_spec.jpg","specular",1)
	};

	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> inds(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	Mesh stove(verts, inds, tex);

	std::vector<Vertex> bottleVerts = generateBottleVertices();
	std::vector<GLuint> bottleInds = generateBottleIndices();
	std::vector<Texture> noTextures;

	Mesh ketchupBottle(bottleVerts, bottleInds, noTextures);
	Mesh mustardBottle(bottleVerts, bottleInds, noTextures);

	std::vector<Vertex> splashVerts = {
		{{-0.05f, 0.0f, -0.05f}, {0,1,0}, {1,1,1}, {0,0}},
		{{-0.05f, 0.0f,  0.05f}, {0,1,0}, {1,1,1}, {0,1}},
		{{ 0.05f, 0.0f, -0.05f}, {0,1,0}, {1,1,1}, {1,0}},
		{{ 0.05f, 0.0f,  0.05f}, {0,1,0}, {1,1,1}, {1,1}}
	};
	std::vector<GLuint> splashInds = { 0, 1, 2, 2, 1, 3 };

	Mesh ketchupSplash(splashVerts, splashInds, noTextures);
	Mesh mustardSplash(splashVerts, splashInds, noTextures);

	MeshItem ketchupSplashItem(&ketchupSplash, glm::vec3(4.0f, 0.5f, 0.5f)); 
	MeshItem mustardSplashItem(&mustardSplash, glm::vec3(4.5f, 4.0f, 0.5f));





	Shader uiShader("ui.vert", "ui.frag");

	Texture buttonTexture[]{
		Texture("start_button.png","diffuse",0),
	};

	Texture greetingsTexture[]{
		Texture("greetings.png","diffuse",0),
	};

	Texture nameTexture[]{
		Texture("name.png","diffuse",0),
	};
	std::vector <Vertex> buttonVerts(buttonVertices, buttonVertices + sizeof(buttonVertices) / sizeof(Vertex));
	std::vector <GLuint> buttonInds(buttonIndices, buttonIndices + sizeof(buttonIndices) / sizeof(GLuint));
	std::vector <Texture> buttonTex(buttonTexture, buttonTexture + sizeof(buttonTexture) / sizeof(Texture));

	std::vector <Texture> greetingsTex(greetingsTexture, greetingsTexture + sizeof(greetingsTexture) / sizeof(Texture));
	Mesh startButton(buttonVerts, buttonInds, buttonTex);
	Mesh greetings(buttonVerts, buttonInds, greetingsTex);


	std::vector <Vertex> nameVerts(nameVertices, nameVertices + sizeof(nameVertices) / sizeof(Vertex));
	std::vector <GLuint> nameInds(nameIndices, nameIndices+ sizeof(nameIndices) / sizeof(GLuint));
	std::vector<Texture> nameTex(
		nameTexture,
		nameTexture + sizeof(nameTexture) / sizeof(Texture)
	);
	Mesh name(nameVerts, nameInds, nameTex);


	Shader lightShader("light.vert", "light.frag");


	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector<GLuint> lightInd(
		lightIndices,
		lightIndices + sizeof(lightIndices) / sizeof(GLuint)
	);
	Mesh light(lightVerts, lightInd, noTextures);


	glm::vec4 lightColor = glm::vec4(1.8f, 1.8f, 1.8f, 1.0f);
	glm::vec3 lightPos = glm::vec3(100.0f, -200.0f, 100.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 stovePos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat stoveRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 stoveScale = glm::vec3(3.5f, 3.5f, 3.5f);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	float progressBarColor[4] = { 0.0f, 1.0f, 0.0f, 1.0f };


	ColorMesh progressBar(progressBarVertices, progressBarIndices);


	Shader colorShader("color.vert", "color.frag");


	glEnable(GL_DEPTH_TEST);


	Camera camera(width, height, glm::vec3(-10.0f, 6.0f, 10.0f));
	camera.LookAt(glm::vec3(0.0f, 4.0f, 0.0f));

	Model table("model/table/scene.gltf");
	Model plate("model/plate/scene.gltf");

	Model tiles("model/tile_floor/scene.gltf");


	Model bottomBun("model/bottom_bun/scene.gltf");
	Model patty("model/patty/scene.gltf");
	Model tomato("model/tomato/scene.gltf");
	Model lettuce("model/lettuce/scene.gltf");
	Model onion("model/onion/scene.gltf");
	Model cheese("model/cheese/scene.gltf");
	Model topBun("model/top_bun/scene.gltf");

	ModelItem bottomBunItem(&bottomBun, glm::vec3(1.0f, 0.5f, 0.3f));
	ModelItem pattyItem(&patty, glm::vec3(0.3f, 0.2f, 0.1f));
	ModelItem onionItem(&onion);

	ModelItem lettuceItem(&lettuce, glm::vec3(0.1f, 1.0f, 0.1f));
	ModelItem pickleItem(&tomato, glm::vec3(0.1f, 1.0f, 0.1f));
	ModelItem cheeseItem(&cheese, glm::vec3(1.0f, 0.2f, 0.2f));
	ModelItem tomatoItem(&tomato, glm::vec3(1.0f, 0.2f, 0.2f));
	ModelItem topBunItem(&topBun, glm::vec3(1.0f, 0.5f, 0.3f));

	MeshItem ketchupItem(&ketchupBottle, glm::vec3(0.8f, 0.1f, 0.1f));
	MeshItem mustardItem(&mustardBottle, glm::vec3(0.9f, 0.8f, 0.1f));
	BurgerPart order[] = {
		{ &bottomBunItem, 0, 0, 0, BURGER_BASE_SCALE * 0.2f, 0.08f, false, glm::vec3(1.0f, 0.5f, 0.3f) },
		{ &pattyItem,     0, 0, 0, BURGER_BASE_SCALE * 7.5f, 0.008f, true,  glm::vec3(1.0f, 0.5f, 0.3f) },
		{ &ketchupItem,   0, 0, 0, 4.0f,                   0.01f, false, glm::vec3(0.8f, 0.1f, 0.1f) },
		{ &mustardItem,   0, 0, 0, 4.0f,                   0.01f, false, glm::vec3(0.9f, 0.8f, 0.1f) },
		{ &pickleItem,   0, 0, 0, 8.0f,                   0.01f, false, glm::vec3(0.1f, 1.0f, 0.1f) },
		{  &onionItem,      0, 0, 0, BURGER_BASE_SCALE * 0.1f,0.01f, false, glm::vec3(0.9f, 0.8f, 0.1f) },
		{ &lettuceItem,   0, 0, 0, BURGER_BASE_SCALE * 0.2f, 0.05f, true,  glm::vec3(0.1f, 1.0f, 0.1f) },
		{ &cheeseItem,    0, 0, 0, BURGER_BASE_SCALE * 2.0f, 0.03f, true,  glm::vec3(1.0f, 0.2f, 0.2f) },
		{ &tomatoItem,    0, 0, 0, 8.0f, 0.03f, true,  glm::vec3(1.0f, 0.2f, 0.2f) },
		{ &topBunItem,    0, 0, 0, BURGER_BASE_SCALE * 0.2f, 0.07f, true,  glm::vec3(1.0f, 0.2f, 0.2f) },
	};


	std::cout << "Model loaded successfully!" << std::endl;

	const double TARGET_FPS = 75.0;
	const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

	auto lastFrameTime = std::chrono::high_resolution_clock::now();



	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float speed = 0.01f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) uZ += speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) uZ -= speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) uX -= speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) uX += speed;

		if (orderStarted || pattyCooked) {
			camera.Inputs(window);
		}
		camera.updateMatrix(60.0f, 0.1f, 100.0f);

		glm::mat4 identity = glm::mat4(1.0f);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		name.DrawUI(uiShader);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		shaderProgram.Activate();
		if (showLight) {
			glm::vec4 sunColor = glm::vec4(2.0f, 2.0f, 1.9f, 1.0f);
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), sunColor.x, sunColor.y, sunColor.z, sunColor.w);
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		}
		else {
			glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), 0.2f, 0.2f, 0.25f, 1.0f);
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		}


		glm::mat4 tilesTransform = glm::mat4(1.0f);
		tilesTransform = glm::translate(tilesTransform, glm::vec3(0.0f, 3.0f, 0.0f));
		tilesTransform = glm::scale(tilesTransform, glm::vec3(0.5f, 0.5f, 0.5f));

		shaderProgram.Activate();
		glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"), 1.0f, 1.0f, 1.0f);
		tiles.Draw(shaderProgram, camera, tilesTransform);

		if (orderStarted) {
			glm::vec3 pattyPos = glm::vec3(uX, uY - 1.8f, uZ);

			glm::mat4 pattyTransform = glm::mat4(1.0f);
			pattyTransform = glm::translate(pattyTransform, glm::vec3(uX, uY - 1.8f, uZ));
			pattyTransform = glm::scale(pattyTransform, glm::vec3(0.5f));


			if (isPattyOverStove(pattyPos, stovePos) && cookingProgress < 1.0f) {
				cookingProgress += 0.005f;
				if (cookingProgress > 1.0f) {
					cookingProgress = 1.0f;
					pattyCooked = true;
					orderStarted = false;

					uX = 0.0f;
					uY = -10.0f;
					uZ = 0.0f;
				}
				std::cout << "Cooking: " << (int)(cookingProgress * 100) << "%" << std::endl;
			}

			float leftX = -1.0f;
			float width = 2.0f;
			float rightX = leftX + cookingProgress * width;

			progressBarVertices[2].position.x = rightX;
			progressBarVertices[3].position.x = rightX;

			progressBar.UpdateVertices(progressBarVertices);

			glDisable(GL_DEPTH_TEST);

			colorShader.Activate();
			progressBar.Draw();

			glEnable(GL_DEPTH_TEST);

			glm::vec3 rawColor = glm::vec3(1.0f, 0.8f, 0.8f);
			glm::vec3 cookedColor = glm::vec3(0.4f, 0.25f, 0.15f);
			glm::vec3 currentColor = glm::mix(rawColor, cookedColor, cookingProgress);

			glDisable(GL_BLEND);

			stove.Draw(shaderProgram, camera, identity, stovePos, stoveRot, stoveScale);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);

			bool wasCullingEnabled = glIsEnabled(GL_CULL_FACE);

			if (wasCullingEnabled) {
				glFrontFace(GL_CW);
			}
			shaderProgram.Activate();
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"),
				currentColor.x, currentColor.y, currentColor.z);
			patty.Draw(shaderProgram, camera, pattyTransform);
			if (wasCullingEnabled) {
				glFrontFace(GL_CCW);
			}
		}

		else if (pattyCooked) {

			bool wasCullingEnabled = glIsEnabled(GL_CULL_FACE);

			if (wasCullingEnabled) {
				glFrontFace(GL_CW);
			}
			glm::mat4 tableTransform = glm::mat4(1.0f);
			tableTransform = glm::translate(tableTransform, glm::vec3(0.0f, 0.0f, 0.0f));
			tableTransform = glm::rotate(tableTransform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			tableTransform = glm::scale(tableTransform, glm::vec3(3.5f));
			table.Draw(shaderProgram, camera, tableTransform);
			if (wasCullingEnabled) {
				glFrontFace(GL_CCW);
			}

			if (wasCullingEnabled) {
				glFrontFace(GL_CW);
			}
			shaderProgram.Activate();
			glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"), 1.0f, 1.0f, 1.0f);
			glm::mat4 plateTransform = glm::mat4(1.0f);
			plateTransform = glm::translate(plateTransform, glm::vec3(0.0f, -0.5f, 0.0f));
			plateTransform = glm::rotate(plateTransform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			plateTransform = glm::scale(plateTransform, glm::vec3(4.8f));
			plate.Draw(shaderProgram, camera, plateTransform);
			if (wasCullingEnabled) {
				glFrontFace(GL_CCW);
			}

			glDisable(GL_BLEND);

			for (int i = 0; i < placedCount; i++) {
				glm::mat4 transform = glm::mat4(1.0f);
				transform = glm::translate(transform, glm::vec3(placed[i].x, placed[i].y, placed[i].z));
				transform = glm::scale(transform, glm::vec3(placed[i].scale));

				if (i == 0 || i == 1 || i == 9) {
					if (wasCullingEnabled) {
						glFrontFace(GL_CW);
					}
					if (i == 1) {

					}
				}

				if (i == 9) {
					transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				}

				shaderProgram.Activate();
				glm::vec3 color = placed[i].cooked ? glm::vec3(0.55f, 0.35f, 0.20f) : glm::vec3(1.0f, 0.5f, 0.3f);
				glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"), color.x, color.y, color.z);


			
				placed[i].item->Draw(shaderProgram, camera, transform);


				if (i == 0 || i == 1) {
					if (wasCullingEnabled) {
						glFrontFace(GL_CCW);
					}
				}
			}

			for (int s = 0; s < spillCount; s++) {
				glm::mat4 spillTransform = glm::mat4(1.0f);
				spillTransform = glm::translate(spillTransform,
					glm::vec3(spills[s].x, spills[s].y, spills[s].z));
				spillTransform = glm::scale(spillTransform, glm::vec3(spills[s].scale));

				shaderProgram.Activate();
				spills[s].item->Draw(shaderProgram, camera, spillTransform);
			}


			if (currentItem < MAX_PARTS) {

				glm::mat4 currentTransform = glm::mat4(1.0f);
				currentTransform = glm::translate(
					currentTransform,
					glm::vec3(uX, uY, uZ)
				);
				currentTransform = glm::scale(currentTransform, glm::vec3(order[currentItem].scale));


				if (currentItem == 0 || currentItem == 1) {
					if (wasCullingEnabled) {
						glFrontFace(GL_CW);
					}
				}

				if (currentItem == 2 || currentItem == 3 || currentItem == 9) {
					if (wasCullingEnabled) {
						glFrontFace(GL_CW);
					}
					currentTransform = glm::rotate(currentTransform, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				}

				shaderProgram.Activate();
				if (currentItem == 2 || currentItem == 3) {
					glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"), 1.0f, 1.0f, 1.0f);
				}
				else {
					glm::vec3 col = order[currentItem].color;
					glUniform3f(glGetUniformLocation(shaderProgram.ID, "colorTint"), col.x, col.y, col.z);
				}


				order[currentItem].item->Draw(shaderProgram, camera, currentTransform);
				
				if (currentItem == 2 || currentItem == 3 || currentItem == 9 || currentItem == 0 || currentItem == 1) {
					if (wasCullingEnabled) {
						glFrontFace(GL_CCW);
					}
				}

				bool triggerPlace = false;

				if (currentItem == 2 || currentItem == 3) {
					if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressed) {
						triggerPlace = true;
						spacePressed = true;
					}
				}
				else {
					triggerPlace = true;
				}

				if (triggerPlace) {

					bool canPlace = false;
					bool skipRest = false;

					glm::vec3 newPos = glm::vec3(uX, uY, uZ);
					DrawableItem* itemToPlace = order[currentItem].item;
					float scaleToUse = order[currentItem].scale;

					if (currentItem == 2 || currentItem == 3) {

						if (placedCount > 0) {

							glm::vec3 prevPos(
								placed[placedCount - 1].x,
								placed[placedCount - 1].y,
								placed[placedCount - 1].z
							);

							float tolerance = 0.3f;
							bool aboveBurger =
								fabs(uX - prevPos.x) <= tolerance &&
								fabs(uZ - prevPos.z) <= tolerance;

							if (aboveBurger) {
								canPlace = true;
								itemToPlace = (currentItem == 2) ? &ketchupSplashItem : &mustardSplashItem;
								scaleToUse = 4.0f;

								newPos.x = prevPos.x;
								newPos.z = prevPos.z;
								if (currentItem == 2) newPos.y = prevPos.y - 0.2f;
								else newPos.y = prevPos.y - 0.05f;
							}
							else {
								if (spillCount < MAX_SPILLS) {
									float uY = getSpillYPos(uX,uZ);
									uY = getSpillYPos(uX, uZ);
									spills[spillCount++] = {
										(currentItem == 2) ?
										&ketchupSplashItem :
										&mustardSplashItem,
										uX,
										uY,
										uZ,
										3.0f };
								}

								skipRest = true;
							}
						}
					}

					if (!skipRest) {

						if (placedCount == 0) {
							canPlace = (uY >= -1.8f && uY <= -1.3f);
							if (canPlace) {
								newPos.x = 0.0f;
								newPos.z = 0.0f;
								newPos.y = -1.8f;
							}
						}
						else if (currentItem != 2 && currentItem != 3) {
							glm::vec3 prevPos = {
								placed[placedCount - 1].x,
								placed[placedCount - 1].y,
								placed[placedCount - 1].z
							};

							float expectedY;

							switch (currentItem) {
							case 1:
								expectedY = prevPos.y + 0.9f;
								break;
							case 4:
								expectedY = prevPos.y - 0.075f;
								break;
							case 5:
								expectedY = prevPos.y + 0.5f;
								break;
							case 6:
								expectedY = prevPos.y - 0.6f;
								break;
							case 7:
								expectedY = prevPos.y + 1.2f;
								break;
							case 8:
								expectedY = prevPos.y - 1.3f;
								break;
							case 9:
								expectedY = prevPos.y + -1.1f;
								break;
							default:
								expectedY = prevPos.y + 0.2f;
								break;
							}
							float yTolerance = 0.1f;

							if (canPlaceItem(newPos, placed, placedCount, order[currentItem])
								&& uY >= (expectedY - yTolerance)
								&& uY <= (expectedY + yTolerance)) {

								std::cout << "Current:  (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")\n";
								std::cout << "Previous: (" << prevPos.x << ", " << prevPos.y << ", " << prevPos.z << ")\n";
								std::cout << "Expected Y: " << expectedY << ", Actual uY: " << uY << "\n";

								canPlace = true;
								newPos.x = prevPos.x;
								newPos.z = prevPos.z;
								newPos.y = expectedY;
							}
						}

						if (canPlace) {
							placed[placedCount++] = {
								itemToPlace,
								newPos.x, newPos.y, newPos.z,
								scaleToUse,
								order[currentItem].height,
								order[currentItem].cooked
							};
							std::cout << "Item placed:  (" << newPos.x << ", " << newPos.y << ", " << newPos.z << ")\n";
							currentItem++;
							uX = 0.0f;
							uY = -10.0f;
							uZ = 0.0f;
						}
					}
				}

				if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
					spacePressed = false;
				}

			}
			else {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				greetings.DrawUI(uiShader);
				glFrontFace(GL_CCW);
			}
		}

		else {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDisable(GL_CULL_FACE);
			startButton.DrawUI(uiShader);
			mousePressedCallback(window);
		}

		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyWasDown) {
			showLight = !showLight;
			lKeyWasDown = true;
		}
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) {
			lKeyWasDown = false;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !zKeyWasDown)
		{
			depthTestEnabled = !depthTestEnabled;
			if (depthTestEnabled) {
				glEnable(GL_DEPTH_TEST);
				std::cout << "DEPTH ON" << "\n";
			}
			else {
				glDisable(GL_DEPTH_TEST);
				std::cout << "DEPTH OFF" << "\n";
			}
			zKeyWasDown = true;
		}

		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		{
			zKeyWasDown = false;
		}

		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyWasDown)
		{
			cullingEnabled = !cullingEnabled;
			if (cullingEnabled) {
				glEnable(GL_CULL_FACE);
				glFrontFace(GL_CCW);
				std::cout << "CULL OFF" << "\n";

			}
			else {
				glDisable(GL_CULL_FACE);
				std::cout << "CULL ON" << "\n";
			}

			cKeyWasDown = true;
		}

		if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE)
		{
			cKeyWasDown = false;
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}


		auto now = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double>(now - lastFrameTime).count();

		if (elapsed < TARGET_FRAME_TIME) {
			double sleepTime = TARGET_FRAME_TIME - elapsed;
			std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
		}

		lastFrameTime = std::chrono::high_resolution_clock::now();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}



	shaderProgram.Delete();
	lightShader.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
