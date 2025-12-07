#include"shaderClass.h"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

//Buildujemo shader program od dva shadera- vertex i fragment
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Citamo contente fajlova
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	// Prebacujemo u array charova
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Kreiramo vertex shader objekat
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Kacimo vertex shader source na objekat upravo napravljen
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	//Kompajlujemo
	glCompileShader(vertexShader);

	//Isto i za fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Kreiramo program i njegovu refrerencu
	ID = glCreateProgram();
	//"kacimo" shadere
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	//Objedinjavamo i linkujemo program
	glLinkProgram(ID);

	// Brisemo nase napravljene sejdere
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

// Aktivira program
void Shader::Activate()
{
	glUseProgram(ID);
}

// Deaktivira program
void Shader::Delete()
{
	glDeleteProgram(ID);
}