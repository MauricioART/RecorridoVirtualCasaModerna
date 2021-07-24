/*
Semestre 2021-2
Práctica : Iluminación
Cambios en el shader, en lugar de enviar la textura en el shader de fragmentos, enviaremos el finalcolor
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>
#include <time.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"
#include "AjusteModelo.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;
const float pi = 3.14159265f;


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


Texture pisoTexture;
Texture materialBlanco;
Texture materialDorado;
Texture materialAzul;
Texture materialGrisAzul;
Texture materialNegro;
Texture materialNegroTr;
Texture materialGris;
Texture materialGrisTr;
Texture ladrillos;
Texture pinturaCasa;
Texture techo;
Texture marmol;
Texture marmolDos;
Texture madera;
Texture maderaGris;
Texture puerta;
Texture vidrio;
Texture micro1;
Texture micro2;
Texture estufa;
Texture cafetera1;
Texture cafetera2;
Texture alacena1;
Texture alacena2;
Texture metal;
Texture cemento;
Texture pinturaAzul;
Texture pinturaAzul2;
Texture pinturaVerde;
Texture pinturaVerde2;
Texture pinturaAmarilla;
Texture pinturaAmarilla2;
Texture piel;
Texture mochila;
Texture cafe;
Texture rojoEllie;
Texture gray;

Model casa;
Model sofa;
Model cama;
Model mesa;
Model tina;
Model escusado;
Model muebleBanio;
Model ventana;
Model comedor;
Model banco;
Model columna;
Model lampara1;
Model lampara2;
Model cocina;
Model ventanaChica;
Model planta1;
Model planta2;
Model cajonera;
Model avion;
Model cocheRC;
Model audi;
Model balon;
Model chico;
Model ellie;
Model avatar;

///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes

#define MAX_FRAMES 100
int i_max_steps = 15;
int i_curr_steps = 0;
typedef struct _frame
{
	float x, z, yRot;
	float xInc, zInc, yRotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 38;			//introducir datos
bool play = true;
int playIndex = 0;

//void saveFrame(void) //tecla L
//{
//
//	printf("frameindex %d\n", FrameIndex);
//
//
//	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
//	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
//	KeyFrame[FrameIndex].giroAvion;
//	//no volatil, agregar una forma de escribir a un archivo para guardar los frames
//	FrameIndex++;
//}
float x, z, yRot;
float xIni, zIni, yRotIni;

void resetElements(void) //Tecla 0
{
	x = KeyFrame[0].x;
	z = KeyFrame[0].z;
	yRot = KeyFrame[0].yRot;
	printf("Hi im reseting x, z and yRot to %2f,%2f,%2f",x,z,yRot );
}

void interpolation(void)
{/*
	printf("Hola estoy en el playIndex: &i\n", playIndex);*/
	KeyFrame[playIndex].xInc = (KeyFrame[playIndex + 1].x - KeyFrame[playIndex].x) / i_max_steps;
	KeyFrame[playIndex].zInc = (KeyFrame[playIndex + 1].z - KeyFrame[playIndex].z) / i_max_steps;
	KeyFrame[playIndex].yRotInc = (KeyFrame[playIndex + 1].yRot - KeyFrame[playIndex].yRot) / i_max_steps;

	/*printf("(xInc,zInc,yRotInc)->(%3f,%3f,%3f)\n", KeyFrame[playIndex].xInc, KeyFrame[playIndex].zInc, KeyFrame[playIndex].yRotInc);*/
}

bool firstTime = 1,interpolationDone = 0;
void animate(void)
{
	//Movimiento del objeto // barra espaciadora
	if (play)
	{
		if (firstTime) {
			firstTime = 0;
			interpolation();
		}
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			if (playIndex >= FrameIndex - 1)	//end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = -2;
				play = false;
				resetElements();
				interpolationDone = 1;
			}
			else //Next frame interpolations
			{	
				//printf("entro aquí\n");
				i_curr_steps = 0; //Reset counter
				//Interpolation
				if(!interpolationDone)
					interpolation();
			}
			playIndex++;
		}
		else
		{
			//printf("se quedó aqui\n");
			//printf("max steps: %f", i_max_steps);
			//Draw animation
			x += KeyFrame[playIndex-1].xInc;
			z += KeyFrame[playIndex-1].zInc;
			yRot += KeyFrame[playIndex-1].yRotInc;
			i_curr_steps++;
			printf("playIndex = %d\n", playIndex);
			printf("(x,z,yRot)->(%3f,%3f,%3f)\n", x, z, yRot);/*
			printf("(xInc,zInc,yRotInc)->(%3f,%3f,%3f)\n", KeyFrame[playIndex].xInc, KeyFrame[playIndex].zInc, KeyFrame[playIndex].yRotInc);*/

		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////


Skybox skybox[11];

//materiales
Material Material_brillante;
Material Material_opaco;

//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

struct variablesAnim {
	float x = 0.0f, y = 0.0f, z = 0.0f, xIni = 0.0f, yIni = 0.0f, zIni = 0.0f,Rot=0.0f,rotIni,escala = 1.0f;
	float xp, yp, zp;
};

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-3.0f, 0.0f, -3.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		3.0f, 0.0f, -3.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-3.0f, 0.0f, 3.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		3.0f, 0.0f, 3.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 1.215f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.5f);

	// INICIAÑIZACIÓN DE TEXTURAS
	pisoTexture = Texture("Textures/Piso.jpg");
	materialBlanco = Texture("Textures/BlancoPuro.png");
	materialDorado = Texture("Textures/Dorado.png");
	materialAzul = Texture("Textures/Azul.png");
	materialGrisAzul = Texture("Textures/Gris-azul.png");
	materialNegro = Texture("Textures/NegroCoche.png");
	materialNegroTr = Texture("Textures/NegroTrans.png");
	materialGris = Texture("Textures/GrisCoche.png");
	materialGrisTr = Texture("Textures/GrisTrans.png");
	ladrillos = Texture("Textures/Blue_glazed_pxr128_bmp.png");
	pinturaCasa = Texture("Textures/Arch137_033_mat_009.jpg");
	techo = Texture("Textures/Techo.png");
	marmol = Texture("Textures/Brown_marble_pxr128_bmp.png");
	marmolDos = Texture("Textures/Marble.jpg");
	madera = Texture("Textures/WoodFine22_COL_3K.jpg");
	maderaGris = Texture("Textures/Getama_Venus_Wood_r.jpg");
	puerta = Texture("Textures/10057_wooden_door_v1_diffuse.jpg");
	vidrio = Texture("Textures/Vidrio.png");
	micro1 = Texture("Textures/micro_kit68.jpg");
	micro2 = Texture("Textures/micro2_kit68.jpg");
	estufa = Texture("Textures/gashob_kit68.jpg");
	cafetera1 = Texture("Textures/coffee_kit68.jpg");
	cafetera2 = Texture("Textures/coffee2_kit68.jpg");
	alacena1 = Texture("Textures/Cabinets.png");
	alacena2 = Texture("Textures/Cabinent2.png");
	metal = Texture("Textures/Brush_metall.png");
	cemento = Texture("Textures/Cemento.png");
	pinturaAzul = Texture("Textures/Azul2.png");
	pinturaAzul2 = Texture("Textures/AzulOscuro.png");
	pinturaVerde = Texture("Textures/Verde.png");
	pinturaVerde2 = Texture("Textures/Green.png");
	pinturaAmarilla = Texture("Textures/Amarillo.png");
	pinturaAmarilla2 = Texture("Textures/background-1126926_640.jpg");
	piel = Texture("Textures/Piel.png");
	mochila = Texture("Textures/Mochila.png");
	cafe = Texture("Textures/Cafe.png");
	rojoEllie = Texture("Textures/RojoEllie.png");
	gray = Texture("Textures/Gray.png");

	pisoTexture.LoadTexture();
	materialBlanco.LoadTexture();
	materialDorado.LoadTexture();
	materialAzul.LoadTexture();
	materialGrisAzul.LoadTexture();
	materialNegro.LoadTexture();
	materialNegroTr.LoadTexture();
	materialGris.LoadTexture();
	materialGrisTr.LoadTexture();
	ladrillos.LoadTexture();
	pinturaCasa.LoadTexture();
	techo.LoadTexture();
	marmol.LoadTexture();
	marmolDos.LoadTexture();
	madera.LoadTexture();
	maderaGris.LoadTexture();
	puerta.LoadTexture();
	vidrio.LoadTexture();
	micro1.LoadTexture();
	micro2.LoadTexture();
	estufa.LoadTexture();
	cafetera1.LoadTexture();
	cafetera2.LoadTexture();
	alacena1.LoadTexture();
	alacena2.LoadTexture();
	metal.LoadTexture();
	cemento.LoadTexture();
	pinturaAzul.LoadTexture();
	pinturaAzul2.LoadTexture();
	pinturaVerde.LoadTexture();
	pinturaVerde2.LoadTexture();
	pinturaAmarilla.LoadTexture();
	pinturaAmarilla2.LoadTexture();
	piel.LoadTexture();
	gray.LoadTexture();
	rojoEllie.LoadTexture();
	mochila.LoadTexture();
	cafe.LoadTexture();

	// INICIALIZACIÓN DE MODELOS

	casa = Model();
	sofa = Model();
	cama = Model();
	mesa = Model();
	tina = Model();
	escusado = Model();
	muebleBanio = Model();
	ventana = Model();
	comedor = Model();
	banco = Model();
	columna = Model();
	lampara1 = Model();
	lampara2 = Model();
	cocina = Model();
	ventana = Model();
	ventanaChica = Model();
	planta1 = Model();
	planta2 = Model();
	cajonera = Model();
	avion = Model();
	cocheRC = Model();
	audi = Model();
	balon = Model();
	chico = Model();
	ellie = Model();
	avatar = Model();

	casa.LoadModel("Models/Casa.obj");
	sofa.LoadModel("Models/Sofa.obj");
	cama.LoadModel("Models/Cama.obj");
	mesa.LoadModel("Models/Escritorio.obj");
	tina.LoadModel("Models/Tina.obj");
	escusado.LoadModel("Models/Escusado.obj");
	muebleBanio.LoadModel("Models/MuebleBanio.obj");
	ventana.LoadModel("Models/Ventanas.obj");
	comedor.LoadModel("Models/MesaComedor.obj");
	banco.LoadModel("Models/Banco.obj");
	columna.LoadModel("Models/Columna.obj");
	lampara1.LoadModel("Models/Lampara1.obj");
	lampara2.LoadModel("Models/Lampara2.obj");
	cocina.LoadModel("Models/Cocina2.obj");
	ventanaChica.LoadModel("Models/VentanaChica.obj");
	planta1.LoadModel("Models/Planta1.obj");
	planta2.LoadModel("Models/Planta2.obj");
	cajonera.LoadModel("Models/Cajonera.obj");
	avion.LoadModel("Models/Airplane.obj");
	cocheRC.LoadModel("Models/CocheRC.obj");
	audi.LoadModel("Models/Audi.obj");
	balon.LoadModel("Models/Balon.obj");
	chico.LoadModel("Models/Guy1.obj");
	ellie.LoadModel("Models/Ellie.obj");
	avatar.LoadModel("Models/TorsoRobotS.obj");

	std::string skyboxPath = "Textures/Skybox/";
	std::string skyboxPathBuf = "Textures/Skybox/";

	std::vector<std::string> skyboxFaces;
	Shader* skyShader = new Shader();
	skyShader->CreateFromFiles("shaders/skybox.vert", "shaders/skybox.frag");

	for (int i = 0; i < 5; i++) {

		skyboxFaces.push_back(skyboxPath.append("Right").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Left").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Bottom").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Top").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Front").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skyboxFaces.push_back(skyboxPath.append("Back").append(std::to_string(i)).append(".jpg"));
		skyboxPath = skyboxPathBuf;
		skybox[i] = Skybox(skyboxFaces,skyShader);
		skyboxFaces.clear();
	}

	

	// INICIALIZACIÓN MATERIALES
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.5f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		10.0f);
	spotLightCount++;

	// KeyFrames
	KeyFrame[0].x = 42.7f;
	KeyFrame[0].z = 22.9f;
	KeyFrame[0].yRot = -90;

	x = KeyFrame[0].x;
    z = KeyFrame[0].z;
	yRot = KeyFrame[0].yRot;

	KeyFrame[1].x = 30.9f;
	KeyFrame[1].z = 22.9f;
	KeyFrame[1].yRot = -90;

	KeyFrame[2].x = 22.3f;
	KeyFrame[2].z = 22.9f;
	KeyFrame[2].yRot = -94;

	KeyFrame[3].x = 18.16f;
	KeyFrame[3].z = 22.10f;
	KeyFrame[3].yRot = -99;

	KeyFrame[4].x = 15.5f;
	KeyFrame[4].z = 21.2f;
	KeyFrame[4].yRot = -105;

	KeyFrame[5].x = 13.8f;
	KeyFrame[5].z = 20.5f;
	KeyFrame[5].yRot = -123;

	KeyFrame[6].x = 11.3f;
	KeyFrame[6].z = 18.7f;
	KeyFrame[6].yRot = -135;

	KeyFrame[7].x = 9.4f;
	KeyFrame[7].z = 15.2f;
	KeyFrame[7].yRot = -153;

	KeyFrame[8].x = 7.8f;
	KeyFrame[8].z = 13.0f;
	KeyFrame[8].yRot = -145;

	KeyFrame[9].x = 6.3f;
	KeyFrame[9].z = 11.5f;
	KeyFrame[9].yRot = -129;

	KeyFrame[10].x = 5.4f;
	KeyFrame[10].z = 10;
	KeyFrame[10].yRot = -124;

	KeyFrame[11].x = 3.9f;
	KeyFrame[11].z = 8.5f;
	KeyFrame[11].yRot = -113;

	KeyFrame[12].x = 2.6f;
	KeyFrame[12].z = 7.7f;
	KeyFrame[12].yRot = -103;

	KeyFrame[13].x = 1.5f;
	KeyFrame[13].z = 7.4f;
	KeyFrame[13].yRot = -97;

	KeyFrame[14].x = -2.1f;
	KeyFrame[14].z = 7.3f;
	KeyFrame[14].yRot = -92;

	KeyFrame[15].x = -6;
	KeyFrame[15].z = 7.25f;
	KeyFrame[15].yRot = -90;

	KeyFrame[16].x = -10;
	KeyFrame[16].z = 7.25f;
	KeyFrame[16].yRot = -90;

	KeyFrame[17].x = -12.6f;
	KeyFrame[17].z = 7.3f;
	KeyFrame[17].yRot = -82;

	KeyFrame[18].x = -14.9f;
	KeyFrame[18].z = 7.7f;
	KeyFrame[18].yRot = -73;

	KeyFrame[19].x = -17.7f;
	KeyFrame[19].z = 8.3f;
	KeyFrame[19].yRot = -59;

	KeyFrame[20].x = -16.4f;
	KeyFrame[20].z = 6.6f;
	KeyFrame[20].yRot = -46;

	KeyFrame[21].x = -12;
	KeyFrame[21].z = 2;
	KeyFrame[21].yRot = -46;

	KeyFrame[22].x = -8.5f;
	KeyFrame[22].z = -0.7f;
	KeyFrame[22].yRot = -29;

	KeyFrame[23].x = -7.8f;
	KeyFrame[23].z = 2;
	KeyFrame[23].yRot = -13;

	KeyFrame[24].x = -7.4f;
	KeyFrame[24].z = 4.9f;
	KeyFrame[24].yRot = 17;

	KeyFrame[25].x = -6;
	KeyFrame[25].z = 6.2f;
	KeyFrame[25].yRot = 30;

	KeyFrame[26].x = -7.7f;
	KeyFrame[26].z = 2.9f;
	KeyFrame[26].yRot = 30;

	KeyFrame[27].x = -5.3f;
	KeyFrame[27].z = 5.2f;
	KeyFrame[27].yRot = 52;

	KeyFrame[28].x = -1.3f;
	KeyFrame[28].z = 7.5f;
	KeyFrame[28].yRot = 56;

	KeyFrame[29].x = 3.7f;
	KeyFrame[29].z = 10.8f;
	KeyFrame[29].yRot = 56;

	KeyFrame[30].x = 6.3f;
	KeyFrame[30].z = 13.4f;
	KeyFrame[30].yRot = 46;

	KeyFrame[31].x = 11.2f;
	KeyFrame[31].z = 18.1f;
	KeyFrame[31].yRot = 53;

	KeyFrame[32].x = 18.3f;
	KeyFrame[32].z = 21.5f;
	KeyFrame[32].yRot = 64;

	KeyFrame[33].x = 31.1f;
	KeyFrame[33].z = 23.6f;
	KeyFrame[33].yRot = 74;

	KeyFrame[34].x = 35.6f; 
	KeyFrame[34].z = 24.3f;
	KeyFrame[34].yRot = 81;

	KeyFrame[35].x = 41.6f;
	KeyFrame[35].z = 24;
	KeyFrame[35].yRot = 90;

	KeyFrame[36].x = 51.6f;
	KeyFrame[36].z = 24;
	KeyFrame[36].yRot = 90;

	KeyFrame[37].x = 42.7f;
	KeyFrame[37].z = 22.9f;
	KeyFrame[37].yRot = -90;
	

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);


	int hora = 0;
	double paso = 3;
	float t2 = 0,t3 = 0;
	bool f1 = 1,f2 = 1;
	bool b1= 1, b2=0, b3=0, b4=0;
	AjusteModelo ajuste = AjusteModelo(3);

	//Variables para animaciones
	variablesAnim avionPar = variablesAnim();
	variablesAnim cochePar = variablesAnim();
	variablesAnim audiPar = variablesAnim();

	avionPar.xIni = 9.3f;
	avionPar.yIni = 9.6f;
	avionPar.zIni = 6.9f;
	avionPar.escala = 0.015f;
	avionPar.rotIni = 0;

	cochePar.xIni = 17.7f;
	cochePar.yIni = 2.74f;
	cochePar.zIni = -12.0f;
	cochePar.escala = 0.2f;
	cochePar.rotIni = 90 * toRadians;

	float t = 0.0f;
	float w = 15, w2 = 20;
	float avatarRot, avatarFrontX,avatarFrontZ;
	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		if (f1) {
			t = 0.0;
			f1 = 0;
		}
		t += 0.01;
		if (f2) {
			if (t > paso) {
				hora++;
				f1 = 1;
			}
			if (hora > 3)
				f2 = 0;
		}
		else {
			if (t > paso) {
				hora--;
				f1 = 1;
			}
			if (hora < 1)
				f2 = 1;
		}
		/*printf("Hora: %i\n", hora);*/
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		ajuste.ajustar(mainWindow.getsKeys());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(camera.getTipoCamara())
			skybox[hora].DrawSkybox(camera.calculateViewMatrix3p(), projection);
		else
			skybox[hora].DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		if (camera.getTipoCamara()) {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix3p()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition3p().x, camera.getCameraPosition3p().y, camera.getCameraPosition3p().z);
		}
		else {
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		}


		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		glm::mat4 model(1.0);
		glm::mat4 modelAux(1.0);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pisoTexture.UseTexture();
		//agregar material al plano de piso
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		//Avatar 
		model = glm::mat4(1.0);
		model = glm::translate(model,camera.getCameraPosition() + glm::vec3(0.0f,-1.215f,0.0f));
		model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
		avatarFrontX = camera.getCameraDirection().x;
		avatarFrontZ = camera.getCameraDirection().z;
		if (avatarFrontZ != 0)
			avatarRot = atan(avatarFrontX / avatarFrontZ);
		if (avatarFrontZ > 0 )
			avatarRot += 180 * toRadians;
		model = glm::rotate(model, avatarRot + 90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		avatar.RenderModel();


		//Casa
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		casa.RenderModel();

		//Sofa
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(17.78f, 2.74f, -16.0f));
		model = glm::scale(model, glm::vec3(0.21f, 0.21f, 0.21f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sofa.RenderModel();

		//Cama 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(37.26f, 2.82f, 6.18f));
		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();

		//Cama 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.20f, 2.82f, 6.24f));
		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();

		//Cama 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-18.14f, 2.82f, 8.74f));
		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();


		//Cama 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.48f, 2.82f, 8.8f));
		model = glm::scale(model, glm::vec3(0.30f, 0.30f, 0.30f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();

		//Escritorio
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(31.46f, 2.72f, 7.18f));
		model = glm::scale(model, glm::vec3(1.46f, 1.46f, 1.46f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		mesa.RenderModel();

		//Banco
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(32.56f, 2.72f, 7.0f));
		model = glm::scale(model, glm::vec3(2.24f, 2.24f, 2.24f));
		model = glm::rotate(model, 72 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		banco.RenderModel();

		//Comedor
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.36f, 2.74f, -13.46f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		comedor.RenderModel();

		//Muebles baño
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(9.22f, 2.72f, -8.5f));
		model = glm::scale(model, glm::vec3(1.9f, 1.9f, 1.9f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		muebleBanio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(23.46f, 2.72f, -0.68f));
		model = glm::scale(model, glm::vec3(1.9f, 1.9f, 1.9f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		muebleBanio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(34.4f, 2.72f, -0.68f));
		model = glm::scale(model, glm::vec3(1.9f, 1.9f, 1.9f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		muebleBanio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-11.06f, 2.72f, 6.5f));
		model = glm::scale(model, glm::vec3(1.9f, 1.9f, 1.9f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		muebleBanio.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-12.44f, 2.72f, 6.44f));
		model = glm::scale(model, glm::vec3(1.9f, 1.9f, 1.9f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		muebleBanio.RenderModel();

		//Escusados

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.7f, 2.72f, 8.34f));
		model = glm::scale(model, glm::vec3(0.265f, 0.265f, 0.265f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		escusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-12.92f, 2.72f, 8.38f));
		model = glm::scale(model, glm::vec3(0.265f, 0.265f, 0.265f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		escusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(11.56f, 2.72f, -7.22f));
		model = glm::scale(model, glm::vec3(0.265f, 0.265f, 0.265f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		escusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(36.36f, 2.72f, -0.24f));
		model = glm::scale(model, glm::vec3(0.265f, 0.265f, 0.265f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		escusado.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(21.52f, 2.74f, -0.24f));
		model = glm::scale(model, glm::vec3(0.265f, 0.265f, 0.265f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		escusado.RenderModel();

		//Columnas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.66f, -2.6f, 1.42f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.66f, -2.6f, 11.7f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-22.92f, -2.6f, 11.7f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-22.92f, -2.6f, 1.42f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-14.24f, -2.6f, -5.62f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.46f, -2.6f, -5.62f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-4.54f, -2.54f, -19.48f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.44f, -2.54f, -19.48f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(18.04f, -2.6f, 9.10f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(18.14f, -2.6f, 2.12f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(39.3f, -2.6f, 9.18f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(39.44f, -2.6f, -1.12f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(29.94f, -2.6f, -2.96f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(23.14f, -2.54f, -17.22f));
		model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		columna.RenderModel();


		//Lamparas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(15.02f, 2.82f, -16.4f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(38.94f, 2.74f, 3.7f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara2.RenderModel();

		//Cajonera
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(35.82f, 2.74f, 3.24f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cajonera.RenderModel();

		//Plantas
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(13.46f, 2.72f, -16.56f));
		model = glm::scale(model, glm::vec3(0.53f, 0.53f, 0.53f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		planta1.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(19.46f, 2.82f, -5.24f));
		model = glm::scale(model, glm::vec3(0.605f, 0.605f, 0.605f));
		model = glm::rotate(model, 197 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		planta2.RenderModel();

		//Ventanas

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.3f, 3.32f, -17.52f));
		model = glm::scale(model, glm::vec3(1.645f, 1.645f, 1.645f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(21.02f, 3.4f, -13.52f));
		model = glm::scale(model, glm::vec3(1.62f, 1.62f, 1.62f));
		model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(34.58f, 3.32f, 9.52f));
		model = glm::scale(model, glm::vec3(1.645f, 1.645f, 1.645f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(23.24f, 3.32f, 9.48f));
		model = glm::scale(model, glm::vec3(1.645f, 1.645f, 1.645f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.92f, 3.42f, 6.26f));
		model = glm::scale(model, glm::vec3(1.55f, 1.55f, 1.55f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-23.04f, 3.56f, 7.94f));
		model = glm::scale(model, glm::vec3(1.595f, 1.595f, 1.595f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(4.42f, 3.32f, -3.56f));
		model = glm::scale(model, glm::vec3(1.525f, 1.525f, 1.525f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(14.74f, 3.32f, -3.56f));
		model = glm::scale(model, glm::vec3(1.525f, 1.525f, 1.525f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventana.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.94f, 3.32f, -4.6f));
		model = glm::scale(model, glm::vec3(1.66f, 1.66f, 1.66f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventanaChica.RenderModel();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-18.32f, 3.32f, 1.099f));
		model = glm::scale(model, glm::vec3(1.62f, 1.62f, 1.62f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ventanaChica.RenderModel();

		//Cocina
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.28f, 2.66f, -13.18f));
		model = glm::scale(model, glm::vec3(1.95f, 1.95f, 1.95f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocina.RenderModel();

		//Chico RC
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(18.14f, 2.74f, -6.3f));
		model = glm::scale(model, glm::vec3(14.0f, 14.0f, 14.0f));
		model = glm::rotate(model, 190 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		chico.RenderModel();

		//Ellie
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.8f, 6.7f, 6.9f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ellie.RenderModel();

		if (mainWindow.getPlayAnimation()) {
			if (t2 <= 45 || t2 >= 315) {
				cochePar.x = cochePar.xIni + 3 * cos(t2 * toRadians)*sqrt(cos(2 * t2* toRadians));
				cochePar.z = cochePar.zIni + 3 * sin(t2* toRadians)*sqrt(cos(2 * t2* toRadians));
				if (t2 == 45)
					t2 = 135;
				if (t2 == 360)
					t2 = 0;
			}
			else if (t2 <= 225) {
				cochePar.x = cochePar.xIni + 3 * cos(t2* toRadians)*sqrt(cos(2 * t2* toRadians));
				cochePar.z = cochePar.zIni - 3 * sin(t2* toRadians)*sqrt(cos(2 * t2* toRadians));
				if (t2 == 225)
					t2 = 315;
			}
			if (t2 < 45 || t2 > 315) {
				cochePar.xp = -3 * sin(t2*toRadians)*sqrt(cos(2 * toRadians*t2)) - 3 * cos(t2*toRadians)*sin(2 * t2*toRadians) / (sqrt(cos(2 * t2*toRadians)));
				cochePar.zp = 3 * cos(t2*toRadians)*sqrt(cos(2 * toRadians*t2)) - 3 * sin(t2*toRadians)*sin(2 * t2*toRadians) / (sqrt(cos(2 * t2*toRadians)));
				cochePar.Rot = cochePar.rotIni + atan(cochePar.zp / cochePar.xp);
			}
			else if (t2 < 225) {
				cochePar.xp = -3 * sin(t2*toRadians)*sqrt(cos(2 * toRadians*t2)) - 3 * cos(t2*toRadians)*sin(2 * t2*toRadians) / (sqrt(cos(2 * t2*toRadians)));
				cochePar.zp = -3 * cos(t2*toRadians)*sqrt(cos(2 * toRadians*t2)) + 3 * sin(t2*toRadians)*sin(2 * t2*toRadians) / (sqrt(cos(2 * t2*toRadians)));
				cochePar.Rot = cochePar.rotIni + atan(cochePar.zp / cochePar.xp);
			}
			if (t2 >= 180 && t2 <= 360) {
				cochePar.Rot += 180 * toRadians;
			}

			if (b1 && t3 <= 30) {
				if (t3 == 30) {
					t3 = 210;
					b1 = 0;
					b2 = 1;
				}
			}
			if (b2 && t3 <= 270) {
				if (t3 == 270) {
					t3 = 90;
					b2 = 0;
					b3 = 1;
				}
			}
			if (b3 && t3 <= 150) {
				if (t3 == 150) {
					t3 = 330;
					b4 = 1;
					b3 = 0;
				}
			}
			if (b4 && t3 <= 360) {
				if (t3 == 360) {
					t3 = 0;
					b4 = 0;
					b1 = 1;
				}
			}
			avionPar.y = avionPar.yIni + 5 * cos(t3*toRadians)*sqrt(cos(3 * t3*toRadians));
			avionPar.z = avionPar.zIni + 5 * sin(t3*toRadians)*sqrt(cos(3 * t3*toRadians));

			if ((int)t3 % 30 != 0) {
				avionPar.yp = -5 * sin(t3*toRadians)*sqrt(cos(3 * t3*toRadians)) - (15 / 2)* (cos(t3*toRadians)*sin(3 * t3*toRadians)) / (sqrt(cos(3 * t3*toRadians)));
				avionPar.zp = 5 * cos(t3*toRadians)*sqrt(cos(3 * t3*toRadians)) - (15 / 2)* (sin(t3*toRadians)*sin(3 * t3*toRadians)) / (sqrt(cos(3 * t3*toRadians)));
				if (avionPar.zp != 0)
					avionPar.Rot = avionPar.rotIni + atan(avionPar.yp / avionPar.zp);
				if ((t3 >= 114.47 && t3 <= 150) || (t3 >= 330 && t3 <= 339.59) || (t3 >= 21 && t3 <= 30) || (t3 >= 210 && t3 <= 245.51)) {
					avionPar.Rot -= 180 * toRadians;
				}
			}
			t3 += 1;
			t2 += 1;

			play = mainWindow.getPlay();

			animate();
		}
		else {

			t2 = 0;
			t3 = 0;
		}

		//Audi
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(x, -2.2f, z));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		model = glm::rotate(model, (yRot) *toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		audi.RenderModel();

		//Coche de RC
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(cochePar.x, cochePar.yIni, cochePar.z));
		model = glm::scale(model, glm::vec3(cochePar.escala, cochePar.escala, cochePar.escala));
		model = glm::rotate(model, -cochePar.Rot, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocheRC.RenderModel();


		//Coche de Avioncito
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(avionPar.xIni, avionPar.y, avionPar.z));
		model = glm::scale(model, glm::vec3(avionPar.escala, avionPar.escala, avionPar.escala));
		model = glm::rotate(model, - avionPar.Rot, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 180*toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		avion.RenderModel();


		/*model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(ajuste.getX(), ajuste.getY(), ajuste.getZ()));
		model = glm::scale(model, glm::vec3(ajuste.getEscala(), ajuste.getEscala(), ajuste.getEscala()));
		model = glm::rotate(model, ajuste.getRotacion()*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		materialGris.LoadTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		switch (ajuste.getModelo()) {
		case 0:
			avatar.RenderModel();
			break;
		case 1:
			ellie.RenderModel();
			break;
		case 2:
			balon.RenderModel();
			break;
		default:
			break;
		}*/

		if (playIndex == 37) {
			for (int i = 0; i < 37; i++) {
				printf("KeyFrameInc[%i]->(%3f,%3f,%3f)\n", i,KeyFrame[i].xInc, KeyFrame[i].zInc, KeyFrame[i].yRotInc);
			}
			
		}



		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}