#pragma once
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include<string>
#include<fstream>
#include<sstream>

#define GLEW_STATIC
#include <GL\glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GLFW\glfw3.h>
#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4, ...
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#include <glm/gtc/type_ptr.hpp> // value_ptr

struct Model_Atributes
{
	std::vector<glm::vec3> pos;
	std::vector<glm::vec2> texture;
	std::vector<glm::vec3>normal;
	std::string string;
};

struct Elumination
{
	glm::vec3 Ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float Ns;
	float Ni;
	std::string string;
};


class Model
{
public:
	Model(const char* file);
	static Elumination load_M_T_L(const char* file2);
	static Model_Atributes Load_X_Y_Z(const char* file1);
	static Model_Atributes Coord;
private:

};

Model::Model(const char* file)
{
	Coord = Model::Load_X_Y_Z(file);
}


Model_Atributes Model::Load_X_Y_Z(const char* file1)
{

	Model_Atributes vertices;

	std::stringstream a;
	std::ifstream myFile(file1);
	std::string currentLine = "";
	std::string firstLetter = "";
	std::string MTLFile;
	glm::vec3  vec3Temporario;
	glm::vec2  vec2Temporario;

	if (!myFile.is_open())
	{
		std::cout << "ERRO! Nao foi possivel abrir o ficheiro." << std::endl;
	}

	//ter uma linha de cada vez
	while (std::getline(myFile, currentLine))
	{
		//obter a primeira letra de cada linha
		a.clear();
		a.str(currentLine);
		a >> firstLetter;

		// se a primeira letra for v faz x

		if (firstLetter == "v") //vertex pos
		{
			a >> vec3Temporario.x >> vec3Temporario.y >> vec3Temporario.z;
			vertices.pos.push_back(vec3Temporario);
		}
		if (firstLetter == "vt")
		{
			a >> vec2Temporario.x >> vec2Temporario.y;
			vertices.texture.push_back(vec2Temporario);
		}
		else if (firstLetter == "vn")
		{
			a >> vec3Temporario.x >> vec3Temporario.y >> vec3Temporario.z;
			vertices.normal.push_back(vec3Temporario);
		}
		else if (firstLetter == "mtllib")
		{
			a >> MTLFile;
			vertices.string = MTLFile;
			

		}
		
	}
	
	return vertices;
}
Elumination Model::load_M_T_L(const char* file2)
{
	std::stringstream a;
	std::ifstream myFile(file2);
	std::string currentLine = "";
	std::string firstLetter = "";
	std::string TAGMap;
	glm::vec3  vec3Temporario;
	float temporario;

	Elumination var;

	if (!myFile.is_open())
	{
		std::cout << "ERRO! Nao foi possivel abrir o ficheiro.2" << std::endl;
	}

	//ter uma linha de cada vez
	while (std::getline(myFile, currentLine))
	{
		//obter a primeira letra de cada linha
		a.clear();
		a.str(currentLine);
		a >> firstLetter;
		if (firstLetter == "Ka") //vertex pos
		{
			a >> vec3Temporario.x >> vec3Temporario.y >> vec3Temporario.z;
			var.Ka = vec3Temporario;
		}
		if (firstLetter == "Kd")
		{
			a >> vec3Temporario.x >> vec3Temporario.y >> vec3Temporario.z;
			var.kd = vec3Temporario;
		}
		else if (firstLetter == "Ks")
		{
			a >> vec3Temporario.x >> vec3Temporario.y >> vec3Temporario.z;
			var.ks = vec3Temporario;
		}
		else if (firstLetter == "Ns")
		{
			a >> temporario;
			var.Ns = temporario;

		}
		else if (firstLetter == "Ni")
		{
			a >> temporario;
			var.Ni = temporario;

		}
		else if (firstLetter == "map")
		{
			a >> TAGMap;
			var.string = TAGMap;
			
		}

	}

	return var;

}


