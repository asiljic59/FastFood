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


//funkcija za dobvijanje polovine sirine
float GetHalfWidth(const Mesh* m)
{
	float minX = 9999;
	float maxX = -9999;

	// XY XY XY XY
	for (int i = 0; i < 8; i += 2)
	{
		float x = m->verticesCopy[i];
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
	}

	return (maxX - minX) / 2.0f;
}

//funkcija za dobijanje pola visine
float GetHalfHeight(const Mesh* m)
{
	float minY = 9999;
	float maxY = -9999;

	// XY XY XY XY
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

	cursor = loadImageToCursor("spatula.png");
	glfwSetCursor(window, cursor);


	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	float ndcX = (float(mouseX) / mode->width) * 2.0f - 1.0f;
	float ndcY = 1.0f - (float(mouseY) / mode->height) * 2.0f;


	//Load GLAD so it configures OpenGL
	gladLoadGL();
	;

	glViewport(0, 0, mode->width, mode->height);


	/// <summary>
	/// Ucitavamo sejdere kojih imamo 3
	/// shaderProgram -> za sve teksture koriscen
	/// colorShader -> za progress bar koji se jedini iscrtava
	/// pattyShader -> za burger koji se boji dok je na sporetu
	/// </summary>
	Shader shaderProgram("default.vert", "default.frag");
	Shader colorShader("color.vert", "color.frag");
	Shader pattyShader("patty.vert", "patty.frag");


	/// <summary>
	///Prvo se iscrtava START dugme
	/// </summary>

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

	/// <summary>
	/// Ime stoji sve vreme
	/// </summary>

	float nameVertices[] = {
		// X      Y      U     V
		0.8f,  0.8f,    0.0f,  0.0f,  // bottom-left
		0.8f,  1.0f,    0.0f,  1.0f,  // top-left
		1.0f,  0.8f,    1.0f,  0.0f,  // bottom-right
		1.0f,  1.0f,    1.0f,  1.0f   // top-right
	};
	Mesh nameMesh(nameVertices, sizeof(nameVertices));


	/// <summary>
	/// PRVI DEO PROGRAMA GDE SE ISCRTAVAJU SPORET I PLJEKSAVICA,
	/// KAO I PROGRESS BAR DOK JE PLJEKSAVICA NA SPORETU
	/// </summary>

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

	/// <summary>
	/// DRUGI DEO PROGRAMA
	/// ISCRTAVA SE STO I TANJIR
	/// </summary>


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


	/// <summary>
	/// Za svaki element burgera kreiramo MESH -> VAO+VBO struktura
	/// Ucitravamo i teksture za sve elemente
	/// </summary>

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
	Texture tomato("tomato.png", GL_TEXTURE15);
	Texture greetings("greetings.png", GL_TEXTURE3);


	/// <summary>
	/// Sluzi kako bi po redu redjali nase elemente na burger
	/// </summary>


	BurgerPart order[] = {
		{ &bottomBun, &bottomBunMesh },
		{ &patty,  &rectMesh },
		{ &ketchup,  &rectMesh },
		{ &mustard,  &rectMesh },
		{ &pickles,  &rectMesh },
		{ &onion,  &rectMesh },
		{ &lettuce,  &rectMesh },
		{ &cheese,  &rectMesh },
		{ &tomato,  &rectMesh },
		{ &topBun,  &rectMesh },
	};

		
	// zuta boja pozadine
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

		//listener za WASD (kretanje)
		float speed = 0.01f;

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) uY += speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) uY -= speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) uX -= speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) uX += speed;


	



		glClear(GL_COLOR_BUFFER_BIT);

		//Ime je uvek tu bez obzira na stanje programa
		shaderProgram.Activate();
		name.Draw(shaderProgram,nameMesh.vao,0.0f,0.0f,1.0f);

	


		if (orderStarted)
		{

			//da li je burger preko sporeta???
			if (PattyOverStove())
			{
				cookingProgress -= 0.005f;  // or heat transfer
			}
			else
			{
				cookingProgress += 0.005f; // cool down
			}

			// za kontrolisemo da cooking progress ide od 0 do 2 (2 je zato sto tehnicki ide po X osi od -1 do 1
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

			// texturing shader active
			shaderProgram.Activate();
			
			//iscrtavamo smederevac i burger koji bojimo toliko koliko se ispekao (cooking progress napredovao)
			smederevac.Draw(shaderProgram, stoveMesh.vao, 0.0f, 0.0f, 1.0f);

			pattyShader.Activate();
			glUniform1f(glGetUniformLocation(pattyShader.ID, "cook"), cookingProgress/2.0f);
			patty.Draw(pattyShader, rectMesh.vao, uX, uY, 1.0f);

			float leftX = -1.0f;
			float rightX = leftX + (cookingProgress * 1.0f); // moves from -1 → 0

			//povecavamo gornji i donji desni X koordinatu
			progressBarVertices[2 * 6 + 0] = rightX;   // top-right X
			progressBarVertices[3 * 6 + 0] = rightX;   // bottom-right X

			// Azuriranje progress bara zato sto se povecava cooking progress
			progressBar.UpdateVertices(progressBarVertices);

			colorShader.Activate();
			progressBar.Draw(colorShader,
				0.0f,
				0.0f,
				1.0f,   
				1.0f
			);

	

		}


		/// <summary>
		/// DRUGI DEO (DAL LI JE ISPECEN BURGER PROVERA)
		/// </summary>

		else if (pattyCooked)
		{
			//bezuslovno se crta sto i tanjir
			shaderProgram.Activate();
			table.Draw(shaderProgram, tableMesh.vao, 0, 0, 1);
			plate.Draw(shaderProgram, plateMesh.vao, 0, 0, 1);

			//tekstura i MESH odgovarajuceg trenutnog itema koji se crta
			Texture* texture;
			Mesh* mesh;

			// nacrtaj već postavljene delove
			for (int i = 0; i < placedCount; i++)
			{
				// if else zato sto moze da bude patty (tu koristimo drugi sejder) a moze biti i bilo koji drugi element
				if (placed[i].texture == &patty) {
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
				//ako je bilo koji drugi element onda se koristi basic sejder za teksture
				else {
					shaderProgram.Activate();
					placed[i].texture->Draw(shaderProgram,
						placed[i].mesh->vao,
						placed[i].x,
						placed[i].y,
						1.5f);
				}
			}
			//crtamo sve neuspesne pokusaje stavljanja kecapa i senfa
			// oni ostaju tu do kraja programa
			for (int s = 0; s < spillCount; s++)
			{
				shaderProgram.Activate();
				spills[s].texture->Draw(shaderProgram,
					rectMesh.vao,
					spills[s].x,
					spills[s].y,
					1.5f);
			}


			// ako još imamo sastojke u porudžbini
			//dok imamo sastojaka vrtece se petlja
			if (currentItem < ORDER_COUNT)
			{
				//ako je trenutni item patty onda ga crtamo pecenog 
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
				//u suprotnom normalno crtanje
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


				// inicijalno stavljamo teksturu i mesh da budu od trenutne stavke
				//ovo nije uvek isto i videcemo zasto (DOLE PRIMER ZA KECAP SENF)
				Texture* texture = order[currentItem].texture;
				Mesh* mesh = order[currentItem].mesh;


				//racunamo ako je kecap/senf u pitanju
				float halfW = GetHalfWidth(order[currentItem].mesh) * 1.5f;
				float halfH = GetHalfHeight(order[currentItem].mesh) * 1.5f;

				bool canPlace = false;
				float newX = uX;
				float newY = uY;
				bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

				//ovo je bilo potrebno jer inicijalno se desavalo da kecap i senf odjednom dodaju na burger
				//a klik se desio samo jednom
				//tako da bi se detektovalno JEDNOM po kliku
				if (spaceNow && !spaceWasDown)  // <-- triggeruje se samo za jedan KLIK
				{
					if (placedCount == 2 || placedCount == 3)
					{
						//fini layer kecapa i senfa koji ide na burger ako je boca iznad njega
						Texture* layer = (placedCount == 2) ? &ketchupLayer : &mustardLayer;
						
						//splash koji ide na sto usled neuspesnog pokusaja
						Texture* splash = (placedCount == 2) ? &ketchupSplash: &mustardSplash;

						//prosli deo
						BurgerPart prev = placed[placedCount - 1];

						float prevHalfW = GetHalfWidth(prev.mesh) * 1.5f;
						float currHalfW = GetHalfWidth(order[currentItem].mesh) * 1.5f;

						float currMinX = uX - currHalfW;
						float currMaxX = uX + currHalfW;

						float prevMinX = prev.x - prevHalfW;
						float prevMaxX = prev.x + prevHalfW;

						//racunamo da li je iznad proslog dela burgera(burgera in general)
						bool aboveBurger = !(currMaxX < prevMinX || currMinX > prevMaxX);

						if (aboveBurger)
						{
							canPlace = true;
							newX = prev.x;
							newY = prev.y + 0.03f;

							texture = layer;
							mesh = &rectMesh;
						}
						//ako nije iznad dodajemo u spills
						else
						{
							spills[spillCount] = { splash, uX, -1.8f };
							spillCount++;
						}
					}
				}

				spaceWasDown = spaceNow; // update for next frame


				//malo hard kodovano za prvi element
				//ali u ovom slucaju je tanjir staticki element
				//pa nije strasno uzeti njegovu koordinatu hard coded
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

				//ako su delovi kecap boca i senf boca preskace se ovaj deo
				else if(placedCount != 2 && placedCount != 3)
				{
					BurgerPart prev = placed[placedCount - 1];

					// DO NOT use half heights anymore
					float snapY = prev.y + 0.03f;

					// Emprijiski zakljuceno da snapY mora se umanjiti 1.3f kod burgera
					bool yCloseEnough;
					if (placedCount == 1) {
						yCloseEnough = (uY <= snapY - 1.3f);
					}
					//nakon toga svaki deo normalno radi sa snapY
					else {
						yCloseEnough = (uY <= snapY);

					}

					//OVO GORE SE DESAVA JER BOTTOM BUN NEMA RECT VERTICES, ALI MI SE NIJE DALO MENJATI

					// Da li je X dovoljno blizu
					bool xCloseEnough = fabs(uX - prev.x) <= 0.2f;

					//AKO SE ISPUNJAVAJU OBA USLOVA ONDA MOZEMO STAVITI
					if (xCloseEnough && yCloseEnough)
					{
						canPlace = true;
						newX = prev.x;    //ALIGNUJEMO TRENUTNO X I PROSLO X

						//isto kao malopre za burger moramo umanjiti za 1.3
						if (placedCount == 1) {
							newY = prev.y - 1.3f;
						}
						//za ostale iteme sve okej i ne mora se adjustovatiu
						else {
							newY = prev.y + 0.05f;
						}
					}
				}

				//na kraju ako je canPlace true onda stavljamo odredjeni item
				if (canPlace)
				{

					//texture,mesh,newX,newY je sve dobijeno kroz ovaj proces gore
					placed[placedCount] = {
						texture,
						mesh,
						newX,
						newY,
						(currentItem == 1) // peceni burger 
					};

					//povecavamo placedCount i currentItem
					placedCount++;
					currentItem++;

					//stavljamo offset na uY da bi se pojavilo na gornjem delu
					if (currentItem < ORDER_COUNT)
					{
						uX = 0.0f;
						uY = -0.5f;
					}
				}
			}
			//ako smo zavrsili iscrtavamo dodtano 
			else {
				greetings.Draw(shaderProgram, greetingsMesh.vao, 0.0f, 0.0f, 1.0f);
			}
			
		}

		//ako je tek pocetak onda se iscrtava samo dugme u sredini
		else {
			// texturing shader active
			shaderProgram.Activate();
			myButton.texture.Draw(shaderProgram, myButton.mesh.vao, 0.0f, 0.0f, 1.0f);
		}

		//za dugme->kada se klikne krece porudzbina
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

	//brisemo sve teksture
	smederevac.Delete();
	patty.Delete();
	bottomBun.Delete();
	topBun.Delete();
	table.Delete();
	plate.Delete();
	ketchup.Delete();
	ketchupLayer.Delete();
	ketchupSplash.Delete();
	mustard.Delete();
	mustardSplash.Delete();
	mustardLayer.Delete();
	onion.Delete();
	pickles.Delete();
	lettuce.Delete();
	cheese.Delete();
	greetings.Delete();

	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}