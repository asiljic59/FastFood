#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include "stb/stb_image.h"


#include "shaderClass.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "callbacks.h"
#include "Texture.h"
#include "Mesh.h"
#include "ColorMesh.h"


const int ORDER_COUNT = 9;

//za dugme na pocetku
struct Button {
	Mesh mesh;
	Texture texture;
	float x, y;     // center position
	float w, h;     // width and height
	bool clicked = false;
};

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

bool PattyOverStove()
{
	float stoveMinX = -1.0f;
	float stoveMaxX = 0.0f;
	float stoveMinY = -1.0f;
	float stoveMaxY = 0.0f;

	// patty final drawn size in NDC
	float pattyWidthHalf = 0.1f; // 0.2 total width
	float pattyHeightHalf = 0.1f; // 0.2 total height

	float pattyMinX = uX - pattyWidthHalf;
	float pattyMaxX = uX + pattyWidthHalf;
	float pattyMinY = uY - pattyHeightHalf;
	float pattyMaxY = uY + pattyHeightHalf;

	bool overlapX = !(pattyMaxX < stoveMinX || pattyMinX > stoveMaxX);
	bool overlapY = !(pattyMaxY < stoveMinY || pattyMinY > stoveMaxY);

	return overlapX && overlapY;
}
bool ItemOverArea(float itemMinX, float itemMaxX,
	float prevMinX, float prevMaxX)
{
	// true ako postoji makar malo preklapanje
	bool overlapX = !(itemMaxX < prevMinX || itemMinX > prevMaxX);
	return overlapX;
};

float GetHalfWidth(const Mesh* m)
{
	float minX = 9999;
	float maxX = -9999;

	// XY XY XY XY (we read only X coordinates)
	for (int i = 0; i < 8; i += 2)
	{
		float x = m->verticesCopy[i];
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
	}

	return (maxX - minX) / 2.0f;
}

float GetHalfHeight(const Mesh* m)
{
	float minY = 9999;
	float maxY = -9999;

	// XY XY XY XY (read only Y coordinates)
	for (int i = 1; i < 8; i += 2)
	{
		float y = m->verticesCopy[i];
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	return (maxY - minY) / 2.0f;
}



//Pomeraj uX, uY za aktivnu teksturu
float uX = 0.0f;
float uY = 0.0f;

float cookingProgress = 0.0f;

bool orderStarted = false;
bool pattyCooked = false;
bool spacePressed = false;
bool spaceWasDown = false;



const int MAX_PARTS = 10;

BurgerPart placed[MAX_PARTS];
int placedCount = 0;
int currentItem = 0;

Spill spills[50];
int spillCount = 0;

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

	//vezivanje callbackova
	glfwSetKeyCallback(window, key_callback);



	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float ndcX = (float(mouseX) / mode->width) * 2.0f - 1.0f;
	float ndcY = 1.0f - (float(mouseY) / mode->height) * 2.0f;


	//Load GLAD so it configures OpenGL
	gladLoadGL();
	;

	glViewport(0, 0, mode->width, mode->height);



	Shader shaderProgram("default.vert", "default.frag");
	Shader colorShader("color.vert", "color.frag");
	Shader pattyShader("patty.vert", "patty.frag");

	float buttonVertices[] = {
		// X,Y        U,V
		-0.2f, -0.1f, 0.0f, 0.0f,  // bottom left
		-0.2f,  0.1f, 0.0f, 1.0f,  // top left
		 0.2f, -0.1f, 1.0f, 0.0f,  // bottom right
		 0.2f,  0.1f, 1.0f, 1.0f   // top right
	};
	Button myButton = {
		Mesh(buttonVertices, sizeof(buttonVertices)),
		Texture("start_button.png", GL_TEXTURE0),
		0.0f, 0.0f,     // centered
		0.3f,0.2f      // dimensions
	};

	Mesh buttonMesh(buttonVertices, sizeof(buttonVertices));

	float nameVertices[] = {
		// X      Y      U     V
		0.8f,  0.8f,    0.0f,  0.0f,  // bottom-left
		0.8f,  1.0f,    0.0f,  1.0f,  // top-left
		1.0f,  0.8f,    1.0f,  0.0f,  // bottom-right
		1.0f,  1.0f,    1.0f,  1.0f   // top-right
	};
	Mesh nameMesh(nameVertices, sizeof(nameVertices));

	float stoveVertices[] = {
		//   X       Y       U     V
		-1.0f, -1.0f,   0.0f, 0.0f, // bottom-left
		-1.0f, 0.0f,   0.0f, 1.0f, // top-left
		 0.0f,  -1.0f,   1.0f, 0.0f, // bottom-right
		0.0f,  0.0f,   1.0f, 1.0f  // top-right
	};

	Mesh stoveMesh = Mesh::Mesh(stoveVertices, sizeof(stoveVertices));

	float rectVertices[] = {
		-0.1f,  0.8f,  0.0f, 0.0f,
		-0.1f,  1.0f,  0.0f, 1.0f,
		 0.1f, 0.8f,  1.0f, 0.0f,
		 0.1f,  1.0f,  1.0f, 1.0f
	};

	Mesh rectMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));


	float progressBarColor[4] = {0.0f, 1.0f, 0.0f, 1.0f};
	float progressBarVertices[] = {
		-1.0f, -0.9f,   0,1,0,1,
		-1.0f, -1.0f,   0,1,0,1,
		-1.0f, -0.9f,   0,1,0,1,
		-1.0f, -1.0f,   0,1,0,1
	};


	ColorMesh progressBar(progressBarVertices, sizeof(progressBarVertices));

	Texture name("name.png", GL_TEXTURE1);
	Texture smederevac("smederevac.png", GL_TEXTURE2);
	Texture patty("patty.png", GL_TEXTURE3);

	//patty cooked textures
	float tableVertices[] = {
		// X      Y      U   V
		-1.0f, -1.0f,    0.0f, 0.0f, // bottom-left
		-1.0f, 0.1f,    0.0f, 1.0f, // top-left
		 1.0f, -1.0f,    1.0f, 0.0f, // bottom-right
		 1.0f, 0.1f,    1.0f, 1.0f  // top-right
	};

	Mesh tableMesh = Mesh::Mesh(tableVertices, sizeof(tableVertices));


	float plateVertices[] = {
		// X      Y      U   V
		 -0.5f, -0.7f,    0.0f, 0.0f, // bottom-left
		 -0.5f, -0.3f,    0.0f, 1.0f, // top-left
		 0.5f,  -0.7f,    1.0f, 0.0f, // bottom-right
		 0.5f,  -0.3f,    1.0f, 1.0f  // top-right
	};

	Mesh plateMesh = Mesh::Mesh(plateVertices, sizeof(plateVertices));


	//bottom bun
	float bottomBunVertices[] = {
		-0.1f, -0.1f, 0,0,
		-0.1f,  0.1f, 0,1,
		 0.1f, -0.1f, 1,0,
		 0.1f,  0.1f, 1,1
	};

	float greetingsVertices[] = {
		-0.4f, 0.4f, 0,0,
		-0.4f,  0.8f, 0,1,
		 0.4f, 0.4f, 1,0,
		 0.4f,  0.8f, 1,1
	};
	Mesh bottomBunMesh(bottomBunVertices, sizeof(bottomBunVertices));
	Mesh ketchupBottleMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh mustardBottleMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh slicedPicklesMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh onionMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh lettuceMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh cheseeSliceMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh topBunMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh ketchupSplashMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));
	Mesh mustardSplashMesh = Mesh::Mesh(rectVertices, sizeof(rectVertices));

	Mesh greetingsMesh(greetingsVertices, sizeof(greetingsVertices));


	Texture table("table.png", GL_TEXTURE1);
	Texture plate("plate.png", GL_TEXTURE2);

	//zato sto je patty 3, a pojavice se kasnije, preskacemo 3
	Texture bottomBun("bottom_bun.png", GL_TEXTURE4);
	Texture ketchup("ketchup.png", GL_TEXTURE5);
	Texture mustard("mustard.png", GL_TEXTURE6);
	Texture pickles("pickles.png", GL_TEXTURE7);
	Texture onion("onion.png", GL_TEXTURE8);
	Texture lettuce("lettuce.png", GL_TEXTURE9);
	Texture cheese("cheese.png", GL_TEXTURE10);
	Texture topBun("top_bun.png", GL_TEXTURE11);
	Texture ketchupSplash("ketchup_splash.png", GL_TEXTURE12);
	Texture mustardSplash("mustard_splash.png", GL_TEXTURE13);
	Texture ketchupLayer("ketchup_layer.png", GL_TEXTURE14);
	Texture mustardLayer("mustard_layer.png", GL_TEXTURE15);
	Texture greetings("greetings.png", GL_TEXTURE3);


	BurgerPart order[] = {
		{ &bottomBun, &bottomBunMesh },
		{ &patty,  &rectMesh },
		{ &ketchup,  &rectMesh },
		{ &mustard,  &rectMesh },
		{ &pickles,  &rectMesh },
		{ &onion,  &rectMesh },
		{ &lettuce,  &rectMesh },
		{ &cheese,  &rectMesh },
		{ &topBun,  &rectMesh },
	};

		

	glClearColor(0.95f, 0.80f, 0.30f, 1.0f);

	// Cistimo bafer da bi stavili novu boju
	glClear(GL_COLOR_BUFFER_BIT);
	//Menjamo back sa front
	glfwSwapBuffers(window);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();


		float speed = 0.01f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) uY += speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) uY -= speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) uX -= speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) uX += speed;


	



		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.Activate();
		name.Draw(shaderProgram,nameMesh.vao,0.0f,0.0f,1.0f);

		if (PattyOverStove())
		{
			cookingProgress -= 0.005f;  // or heat transfer
		}
		else
		{
			cookingProgress += 0.005f; // cool down
		}


		if (cookingProgress < 0.0f) cookingProgress = 0.0f;
		if (cookingProgress >= 2.0f && !pattyCooked) {
			cookingProgress = 2.0f;
			pattyCooked = true;
			orderStarted = false;

			placedCount = 0;
			currentItem = 0;

			uX = 0.0f;
			uY = 0.9f;
		}


		if (orderStarted)
		{
			// texturing shader active
			shaderProgram.Activate();

			smederevac.Draw(shaderProgram, stoveMesh.vao, 0.0f, 0.0f, 1.0f);

			pattyShader.Activate();
			glUniform1f(glGetUniformLocation(pattyShader.ID, "cook"), cookingProgress/2.0f);
			patty.Draw(pattyShader, rectMesh.vao, uX, uY, 1.0f);

			float leftX = -1.0f;
			float rightX = leftX + (cookingProgress * 1.0f); // moves from -1 → 0

			progressBarVertices[2 * 6 + 0] = rightX;   // top-right X
			progressBarVertices[3 * 6 + 0] = rightX;   // bottom-right X

			// Upload to GPU
			progressBar.UpdateVertices(progressBarVertices);

			colorShader.Activate();
			progressBar.Draw(colorShader,
				0.0f,
				0.0f,
				1.0f,    // IMPORTANT: scale stays 1!
				1.0f
			);

	

		}
		else if (pattyCooked)
		{
			shaderProgram.Activate();
			table.Draw(shaderProgram, tableMesh.vao, 0, 0, 1);
			plate.Draw(shaderProgram, plateMesh.vao, 0, 0, 1);

			Texture* texture;
			Mesh* mesh;

			// nacrtaj već postavljene delove
			for (int i = 0; i < placedCount; i++)
			{
				if (placed[i].texture == &patty) {
					// draw with shader that uses cooked uniform
					pattyShader.Activate();
					glUniform1f(
						glGetUniformLocation(pattyShader.ID, "cook"),
						placed[i].cooked ? 1.0f : 0.0f
					);
					placed[i].texture->Draw(pattyShader,
						placed[i].mesh->vao,
						placed[i].x,
						placed[i].y,
						1.5f);
				}
				else {
					shaderProgram.Activate();
					placed[i].texture->Draw(shaderProgram,
						placed[i].mesh->vao,
						placed[i].x,
						placed[i].y,
						1.5f);
				}
			}


			// ako još imamo sastojke u porudžbini
			if (currentItem < ORDER_COUNT)
			{
				for (int s = 0; s < spillCount; s++)
				{
	
					spills[s].texture->Draw(shaderProgram,
						rectMesh.vao,
						spills[s].x,
						spills[s].y,
						1.5f);
				}

				if (currentItem == 1) {
					pattyShader.Activate();
					glUniform1f(
						glGetUniformLocation(pattyShader.ID, "cook"),
						1.0f
					);

					order[currentItem].texture->Draw(
						pattyShader,
						order[currentItem].mesh->vao,
						uX,
						uY,
						1.5f
					);
				}
				else {
					shaderProgram.Activate();
					order[currentItem].texture->Draw(
						shaderProgram,
						order[currentItem].mesh->vao,
						uX,
						uY,
						1.5f
					);
				}



				Texture* texture = order[currentItem].texture;
				Mesh* mesh = order[currentItem].mesh;

				float halfW = GetHalfWidth(order[currentItem].mesh) * 1.5f;
				float halfH = GetHalfHeight(order[currentItem].mesh) * 1.5f;

				bool canPlace = false;
				float newX = uX;
				float newY = uY;
				bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

				if (spaceNow && !spaceWasDown)  // <-- triggers ONLY once per press
				{
					if (placedCount == 2 || placedCount == 3)
					{
						Texture* splash = (placedCount == 2) ? &ketchupLayer : &mustardLayer;

						BurgerPart prev = placed[placedCount - 1];

						float prevHalfW = GetHalfWidth(prev.mesh) * 1.5f;
						float currHalfW = GetHalfWidth(order[currentItem].mesh) * 1.5f;

						float currMinX = uX - currHalfW;
						float currMaxX = uX + currHalfW;

						float prevMinX = prev.x - prevHalfW;
						float prevMaxX = prev.x + prevHalfW;

						bool aboveBurger = !(currMaxX < prevMinX || currMinX > prevMaxX);

						if (aboveBurger)
						{
							canPlace = true;
							newX = prev.x;
							newY = prev.y + 0.03f;

							texture = splash;
							mesh = &rectMesh;
						}
						else
						{
							spills[spillCount] = { splash, uX, -1.8f };
							spillCount++;
						}
					}
				}

				spaceWasDown = spaceNow; // update for next frame



				if (placedCount == 0)
				{
					bool xOverlap =
						(uX + halfW > -0.5f) &&
						(uX - halfW < 0.5f);

					bool yJustAbovePlate =
						(uY <= -0.45f);  // sada mnogo realnije

					if (xOverlap && yJustAbovePlate)
					{
						canPlace = true;
						newX = uX;
						newY = uY;
						std::cout <<"Bun X" << newX << std::endl;
						std::cout << "Bun Y" << newY << std::endl;
					}
				}
				else if(placedCount != 2 && placedCount != 3)
				{
					BurgerPart prev = placed[placedCount - 1];

					// DO NOT use half heights anymore
					float snapY = prev.y + 0.03f;

					// Only require that patty passes close vertically
					bool yCloseEnough;
					if (placedCount == 1) {
						yCloseEnough = (uY <= snapY - 1.3f);
					}
					else {
						yCloseEnough = (uY <= snapY);

					}

					// Horizontal proximity
					bool xCloseEnough = fabs(uX - prev.x) <= 0.2f;

					if (xCloseEnough && yCloseEnough)
					{
						canPlace = true;
						newX = prev.x;     // must align
						if (placedCount == 1) {
							newY = prev.y - 1.3f;      // perfect stacking height
						}
						else {
							newY = prev.y + 0.05f;
						}

						std::cout << "Item"<<currentItem<<":" << newX << std::endl;
						std::cout << "Item"<<currentItem<<":" << newY << std::endl;
					}
				}

				if (canPlace)
				{
					placed[placedCount] = {
						texture,
						mesh,
						newX,
						newY,
						(currentItem == 1) // cooked patties only
					};

					placedCount++;
					currentItem++;

					if (currentItem < ORDER_COUNT)
					{
						uX = 0.0f;
						uY = -0.5f;
					}
				}
			}
			else {
				greetings.Draw(shaderProgram, greetingsMesh.vao, 0.0f, 0.0f, 1.0f);
			}
			
		}


		else {
			// texturing shader active
			shaderProgram.Activate();
			myButton.texture.Draw(shaderProgram, myButton.mesh.vao, 0.0f, 0.0f, 1.0f);
		}


		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			float ndcX = (float(mouseX) / mode->width) * 2.0f - 1.0f;
			float ndcY = 1.0f - (float(mouseY) / mode->height) * 2.0f;

			if (IsInsideButton(myButton, ndcX, ndcY))
			{
				orderStarted = true;
			}
		}



		glfwSwapBuffers(window);
	}




	// Delete window before ending the program
	glfwDestroyWindow(window);
	smederevac.Delete();
	patty.Delete();
	bottomBun.Delete();
	table.Delete();
	plate.Delete();
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}