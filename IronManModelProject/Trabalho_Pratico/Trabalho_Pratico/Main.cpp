#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

#include <iostream>
#include <vector>
#include <algorithm>

#define GLEW_STATIC
#include <GL\glew.h>

#define GLFW_USE_DWM_SWAP_INTERVAL
#include <GLFW\glfw3.h>

#include "LoadShaders.h"
#include"File_Loader.h"

#include <glm/glm.hpp> // vec3, vec4, ivec4, mat4, ...
#include <glm/gtc/matrix_transform.hpp> // translate, rotate, scale, perspective, ...
#include <glm/gtc/type_ptr.hpp> // value_ptr
#include <glm\gtc\matrix_inverse.hpp> // glm::inverseTranspose()
using namespace std;


void CameraInput(GLFWwindow *window);
void LightInput(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow * window, double xpos, double ypos);
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
void print_error(int error, const char *description);
void init(void);
void display(void);
void load_texture(void);

//Para por a personagem a rodar
GLfloat ANGLE = 0.0f;
//Matrizes que formam o MVP
glm::mat4 Model, View, Projection;
glm::mat3 NormalMatrix;

//Chama a variavel publica da classe
Model_Atributes Model::Coord;
//Criasse uma vaiavel do tipo da estrutura onde estao guardados os vertices e isso
Model_Atributes _SavedCoord;
//Criasse uma vaiavel do tipo da estrutura onde esta guardado as propriedades da luz
Elumination _Light;

//Settings
#define WIDTH 800
#define HEIGHT 600

#define NumVAOs 1
#define NumBuffers 4 // Vértices, textura,cor, normais
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint programa;
const GLuint NumVertices = 9264;


//Camera
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

//Luzes
bool Ambient = true;
bool Direcional = true;
bool Pontual = true;
bool Conico=true;

//mouse state
bool firstMouse = true;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
//timing
float deltaTime = 0.0f; // tempo entre frames
float lastFrame = 0.0f;


//Cubo do stor
std::vector<glm::vec3> Load3DModel(void) {
	// 6 faces * 4 vértices por face
	glm::vec3 point[6 * 4] = {
		// Frente
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),

		// Trás

		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		// Direita
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		// Esquerda
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f),
		// Cima
		glm::vec3(-1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f,  1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f),
		// Baixo
		glm::vec3(-1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f,  1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f)
	};

	std::vector<glm::vec3> ret;
	for (auto i : point)
		ret.push_back(i);

	return ret;
}

//display do cubo stor
void display(std::vector<glm::vec3> obj, glm::mat4 mvp) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float *vertex_stream = static_cast<float*>(glm::value_ptr(obj.front()));

	std::vector<glm::vec3> colors{
		glm::vec3(1.0f, 0.0f, 0.0f), // Red
		glm::vec3(1.0f, 1.0f, 0.0f), // Yellow
		glm::vec3(0.0f, 1.0f, 0.0f), // Green
		glm::vec3(0.0f, 1.0f, 1.0f), // Cyan
		glm::vec3(0.0f, 0.0f, 1.0f), // Blue
		glm::vec3(1.0f, 0.0f, 1.0f)  // Magenta
	};

	// Desenha quad em modo imediato
	glBegin(GL_QUADS);
	/* obj.size()*(obj.front().length()) é o mesmo que 6*4*3 */
	/* 6 faces * 4 vértices por face * 3 coordenadas por vértice */
	for (int nv = 0; nv < 6 * 4 * 3; nv += 3) {
		glColor3f(colors[nv / (4 * 3)].r, colors[nv / (4 * 3)].g, colors[nv / (4 * 3)].b);
		glm::vec4 vertex = glm::vec4(vertex_stream[nv], vertex_stream[nv + 1], vertex_stream[nv + 2], 1.0f);
		// Cálculo das coordenadas de recorte
		glm::vec4 transformed_vertex = mvp * vertex;
		// Divisão de Perspetiva
		glm::vec4 normalized_vertex = transformed_vertex / transformed_vertex.w;
		// Desenho do vértice
		glVertex3f(normalized_vertex.x, normalized_vertex.y, normalized_vertex.z);
	}
	glEnd();
}

int main(void) {
	GLFWwindow *window;

	//Chamada do construtor da classe que recebe o ficheiro
	Model::Model("Iron_Man.XYZ");
	//coordenada do tipo da estrutura que vai receber os valores guardados na estrutura(vertices)
	_SavedCoord = Model::Coord;
	//coordenada do tipo da estrutura que vai receber os valores guardados na estrutura(luz)
	_Light = Model::load_M_T_L(_SavedCoord.string.c_str());


	std::vector<glm::vec3> obj = Load3DModel();



	glfwSetErrorCallback(print_error);

	if (!glfwInit()) return -1;

	window = glfwCreateWindow(WIDTH, HEIGHT, "Trabalho Pratico", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	glewInit();

	//funcao que recebe a imagem de textura e manda a pro buffer
	load_texture();

	
	init();

	// Indicação da Unidade de Textura a ligar ao sampler 'TexSampler'.
	//glUniform1i(glGetUniformLocation(programa, "TexSampler"), 0 /* Unidade de Textura #0 */);
	GLint locationTexSampler = glGetProgramResourceLocation(programa, GL_UNIFORM, "TexSampler");
	glProgramUniform1i(programa, locationTexSampler, 0 /* Unidade de Textura #0 */);

	//tempo mandado como uniform
	float time = float(glfwGetTime());
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "time"), time);

	
	glfwSetKeyCallback(window, LightInput);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//aqui dizemos para o GLFW capturar o rato
	glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);



	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		//logica por frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		/*Recebe o Input pra janela*/
		CameraInput(window);


		/*display(obj, mvp);*/
		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	return 0;
}



//Responsavel pela ativaçao/desativacao das teclas
void LightInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{

	
	if (key== GLFW_KEY_1 && action== GLFW_PRESS)
		Ambient = !Ambient;

	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		Direcional = !Direcional;

	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		Pontual = !Pontual;

	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		Conico = !Conico;
}

//Responsavel pelo input da camera
void CameraInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraFront * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp))*cameraSpeed;
}

//sempre que se mexer o rato esta funcao é chamada
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	//serve pra o ecra nao dar um 'flip'
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

}

//sempre que usamos o scroll este callback é chamado
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void print_error(int error, const char *description) {
	std::cout << description << std::endl;
}

void init(void)
{
	Projection = glm::perspective(glm::radians(fov), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	View = glm::lookAt(
		glm::vec3(cameraPos),	// Posição da câmara no mundo
		glm::vec3(cameraPos + cameraFront),	// Direção para a qual a câmara esta apontada
		glm::vec3(cameraUp)		// Vector vertical
	);


	Model = glm::rotate(glm::mat4(1.0f), ANGLE+=2.0f, glm::vec3(0.0f, 1.0f, 0.0f));


	glm::mat4 ModelViewProjection = Projection * View * Model;
	glm::mat4 ModelView = View * Model;

	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));

	// ****************************************************
	// VAOs - Vertex Array Objects
	// ****************************************************

	// Gerar 'NumVAOs' nomes para VAOs.
	// Neste caso, como NumVAOS = 1, gera apenas 1 nome.
	glGenVertexArrays(NumVAOs, VAOs);
	// Faz bind do VAO, cujo nome está definido em 'VAOs[0]', com o contexto do OpenGL.
	// Um VAO é criado no primero bind que lhe seja feito.
	// Este VAO passa a estar ativo até que seja feito o bind a outro VAO, ou seja feito o bind com valor 0.
	glBindVertexArray(VAOs[0]);
	// ****************************************************
	// VBOs - Vertex Buffer Objects
	// ****************************************************

	// Gera 'NumBuffers' nomes para VBOs.
	// Neste caso, como NumBuffers = 2, gera 2 nomes
	// Esta função pode ser chamada antes da criação de VAOs.
	glGenBuffers(NumBuffers, Buffers);
	// Para cada nome de VBO
	for (int i = 0; i < NumBuffers; i++) {
		// Faz bind do VBO ao buffer GL_ARRAY_BUFFER.
		// GL_ARRAY_BUFFER é utilizado para dados de atributos de vértices.
		// Um VBO é criado no primeiro bind que lhe seja feito.
		// Este VBO passa a estar ativo até que seja feito o bind a outro VBO ou seja feito o bind com valor 0.
		// Para alocar a memória para os BO, não é necessário que já esteja vinculado um VAO.
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[i]);
		if (i == 0) {
			// Inicializa o VBO (que está ativo) com memória de tamanho imutável.
			glBufferStorage(GL_ARRAY_BUFFER, NumVertices * sizeof(glm::vec3), (GLfloat*)glm::value_ptr(_SavedCoord.pos[0]), 0);
		
		}
		else if (i == 2)
		{
			// Inicializa o VBO (que está ativo) com memória de tamanho imutável.
			glBufferStorage(GL_ARRAY_BUFFER, NumVertices * sizeof(glm::vec2), (GLfloat*)glm::value_ptr(_SavedCoord.texture[0]), 0);
			
		}
		else if (i == 3)
		{
			// Inicializa o VBO (que está ativo) com memória de tamanho imutável.
			glBufferStorage(GL_ARRAY_BUFFER, NumVertices * sizeof(glm::vec3), (GLfloat*)glm::value_ptr(_SavedCoord.normal[0]), 0);
		}

	}


	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER,   "Shaders/VertexShader.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/ShaderFragment.frag" },
		{ GL_NONE, NULL }
	};

	programa = LoadShaders(shaders);
	if (!programa) exit(EXIT_FAILURE);
	glUseProgram(programa);

	// ****************************************************
	// Ligar Atributos aos Shaders
	// ****************************************************

	// As localizações de atributos fazem parte do programa shader, e são completamente independentes do VAO.
	// Podemos, assim, obter as localizações de atributos sem que antes se tenha criado qualquer VAO.
	// ------------------------------------------------------------------------------------------------------
	// Obtém a localização do atributo 'vPosition' no 'programa'.
	//GLint coordsId = glGetAttribLocation(programa, "vPosition"); // Para versões anteriores à 4.3
	GLint coordsId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vPosition"); // Para versões >= 4.3
	// Obtém a localização do atributo 'vColors' no 'programa'.
	//GLint coresId = glGetAttribLocation(programa, "vColors"); // Para versões anteriores à 4.3
	GLint coresId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vColors"); // Para versões >= 4.3
	// Obtém a localização do atributo 'vTexturesCoords' no 'programa'.
	//GLint coresId = glGetAttribLocation(programa, "vTexturesCoords"); // Para versões anteriores à 4.3
	GLint texturaId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vTextureCoords");//associa as texturas aos shaders
	// Obtém a localização do atributo 'vNormal' no 'programa'.
	//GLint coresId = glGetAttribLocation(programa, "vNormal"); // Para versões anteriores à 4.3
	GLint normalId = glGetProgramResourceLocation(programa, GL_PROGRAM_INPUT, "vNormal");//associa as normais aos shaders

	// Atribui valor ao uniform MVP
	GLint mvpId = glGetProgramResourceLocation(programa, GL_UNIFORM, "MVP");
	glProgramUniformMatrix4fv(programa, mvpId, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

	// Ativa o VBO 'Buffers[0]'.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	// Liga a localização do atributo 'vPosition' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui já é necessário ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// É neste ponto que o VBO fica associado ao VAO.
	// Especifica também como é que a informação do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 3 elementos do tipo float.
	glVertexAttribPointer(coordsId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);

	// Ativa o VBO 'Buffers[1]'.
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	// Liga a localização do atributo 'vColors' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui já é necessário ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// É neste ponto que o VBO fica associado ao VAO.
	// Especifica também como é que a informação do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 3 elementos do tipo float.
	glVertexAttribPointer(coresId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);


	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	// Liga a localização do atributo 'texturaId' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui já é necessário ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// É neste ponto que o VBO fica associado ao VAO.
	// Especifica também como é que a informação do atributo 'coordsId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 2 elementos do tipo float.
	glVertexAttribPointer(texturaId, 2 /*2 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);


	glBindBuffer(GL_ARRAY_BUFFER, Buffers[3]);
	// Liga a localização do atributo 'normalId' dos shaders do 'programa', ao VBO e VAO (ativos).
	// Aqui já é necessário ter o VAO e o VBO vinculados ao contexto do OpenGL.
	// É neste ponto que o VBO fica associado ao VAO.
	// Especifica também como é que a informação do atributo 'normalId' deve ser interpretada.
	// Neste caso, o atributo irá receber, por vértice, 3 elementos do tipo float.
	glVertexAttribPointer(normalId, 3 /*3 elementos por vértice*/, GL_FLOAT/*do tipo float*/, GL_FALSE, 0, nullptr);

	// Habitita o atributo com localização 'coresId' para o VAO ativo.
	// Os atributos de um VAO estão desativados por defeito.
	glEnableVertexAttribArray(coordsId);

	// Habitita o atributo com localização 'coresId' para o VAO ativo.
	// Os atributos de um VAO estão desativados por defeito.
	glEnableVertexAttribArray(coresId);

	// Habitita o atributo com localização 'texturaId' para o VAO ativo.
	// Os atributos de um VAO estão desativados por defeito.
	glEnableVertexAttribArray(texturaId);

	// Habitita o atributo com localização 'normalId' para o VAO ativo.
	// Os atributos de um VAO estão desativados por defeito.
	glEnableVertexAttribArray(normalId);


	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));
	// Atribui valor ao uniform View
	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));
	// Atribui valor ao uniform ModelView
	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));
	// Atribui valor ao uniform Projection
	GLint projectionId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(programa, projectionId, 1, GL_FALSE, glm::value_ptr(Projection));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

	

	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.emissive"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.ambient"), 1, glm::value_ptr(_Light.Ka));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.diffuse"), 1, glm::value_ptr(_Light.kd));
	glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.specular"), 1, glm::value_ptr(_Light.ks));
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "material.shininess"), _Light.Ns);


	glViewport(0, 0, WIDTH, HEIGHT);

}

void display(void) {
	static const float black[] = {
		0.0f, 0.0f, 0.0f, 0.0f
	};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// A função glClearBufferfv(GL_COLOR, ...) é semelhante à função glClearColor().
	glClearBufferfv(GL_COLOR, 0, black);
	Projection = glm::perspective(glm::radians(fov), float(WIDTH) / float(HEIGHT), 0.1f, 100.f);

	View = glm::lookAt(
		glm::vec3(cameraPos),	// Posição da câmara no mundo
		glm::vec3(cameraPos + cameraFront),	// Direção para a qual a câmara esta apontada
		glm::vec3(cameraUp)		// Vector vertical
	);

	// Atualiza os dados do Uniform
	Model = glm::rotate(glm::mat4(1.0f), ANGLE +=0.001f, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

	glm::mat4 ModelViewProjection = Projection * View * Model;
	glm::mat4 ModelView = View * Model;

	float time = float(glfwGetTime());
	glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "time"), time);

	GLint mvpId = glGetProgramResourceLocation(programa, GL_UNIFORM, "MVP");
	glProgramUniformMatrix4fv(programa, mvpId, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));


	NormalMatrix = glm::inverseTranspose(glm::mat3(ModelView));
	// Atribui valor ao uniform Model
	GLint modelId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Model");
	glProgramUniformMatrix4fv(programa, modelId, 1, GL_FALSE, glm::value_ptr(Model));
	GLint viewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "View");
	glProgramUniformMatrix4fv(programa, viewId, 1, GL_FALSE, glm::value_ptr(View));
	GLint modelViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "ModelView");
	glProgramUniformMatrix4fv(programa, modelViewId, 1, GL_FALSE, glm::value_ptr(ModelView));	
	GLint projectionId = glGetProgramResourceLocation(programa, GL_UNIFORM, "Projection");
	glProgramUniformMatrix4fv(programa, projectionId, 1, GL_FALSE, glm::value_ptr(Projection));
	// Atribui valor ao uniform NormalMatrix
	GLint normalViewId = glGetProgramResourceLocation(programa, GL_UNIFORM, "NormalMatrix");
	glProgramUniformMatrix3fv(programa, normalViewId, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

	// Fonte de luz ambiente global
	if (Ambient) {
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(5.0, 5.0, 5.0)));		
	}
	else {
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "ambientLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	}

	if (Direcional)
	{
		// Fonte de luz direcional
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
	}
	else {
		// Fonte de luz direcional
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.direction"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "directionalLight.specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
	}

	if (Pontual)
	{
		// Fonte de luz pontual #1
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 5.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].constant"), 1.0f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].linear"), 0.06f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].quadratic"), 0.02f);

		// Fonte de luz pontual #2
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(-2.0, 2.0, 5.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].constant"), 1.0f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].linear"), 0.06f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].quadratic"), 0.02f);
	}
	else
	{
		// Fonte de luz pontual #1
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].constant"), 0.1f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].linear"), 0.1f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[0].quadratic"), 0.01f);

		// Fonte de luz pontual #2
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].position"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].constant"), 0.1f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].linear"), 0.1f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "pointLight[1].quadratic"), 0.01f);
	}

	if (Conico)
	{
		// Fonte de luz Conica 
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(cameraPos));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(30, 30, 30)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.linear"), 0.06f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.quadratic"), 0.02f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotCutoff"), glm::cos(glm::radians(12.f)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotExponent"), glm::cos(glm::radians(17.0f)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotDirection"), 1, glm::value_ptr(cameraFront));
	}
	else {
		// Fonte de luz Conica 
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.position"), 1, glm::value_ptr(cameraPos));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.ambient"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.specular"), 1, glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.constant"), 1.0f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.linear"), 0.06f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.quadratic"), 0.02f);
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotCutoff"), glm::cos(glm::radians(12.5f)));
		glProgramUniform1f(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotExponent"), glm::cos(glm::radians(60.0f)));
		glProgramUniform3fv(programa, glGetProgramResourceLocation(programa, GL_UNIFORM, "spotLight.spotDirection"), 1, glm::value_ptr(cameraFront));
	}
	// Vincula (torna ativo) o VAO
	glBindVertexArray(VAOs[0]);

	// Envia comando para desenho de primitivas GL_TRIANGLES, que utilizará os dados do VAO vinculado.
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);

}

void load_texture(void) {
	GLuint textureName = 0;

	// Gera um nome de textura
	glGenTextures(1, &textureName);

	// Ativa a Unidade de Textura #0
	// A Unidade de Textura 0 já está ativa por defeito.
	// Só uma Unidade de Textura está ativa a cada momento.
	glActiveTexture(GL_TEXTURE0);

	// Vincula esse nome de textura ao target GL_TEXTURE_2D da Unidade de Textura ativa.
	glBindTexture(GL_TEXTURE_2D, textureName);

	// Define os parâmetros de filtragem (wrapping e ajuste de tamanho)
	// para a textura que está vinculada ao target GL_TEXTURE_2D da Unidade de Textura ativa.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Leitura/descompressão do ficheiro com imagem de textura
	int width, height, nChannels;
	// Ativa a inversão vertical da imagem, aquando da sua leitura para memória.
	stbi_set_flip_vertically_on_load(true);
	// Leitura da imagem para memória do CPU
	unsigned char *imageData = stbi_load(_Light.string.c_str(), &width, &height, &nChannels, 0);
	if (imageData) {
		// Carrega os dados da imagem para o Objeto de Textura vinculado ao target GL_TEXTURE_2D da Unidade de Textura ativa.
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, imageData);

		// Gera o Mipmap para essa textura
		glGenerateMipmap(GL_TEXTURE_2D);

		// Liberta a imagem da memória do CPU
		stbi_image_free(imageData);
	}
	else {
		cout << "Error loading texture!" << endl;
	}
}


