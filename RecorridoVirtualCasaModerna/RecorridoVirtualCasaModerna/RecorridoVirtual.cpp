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


Model casa;
Model sofa;
Model cama;
Model mesa;
Model tina;
Model escusado;
Model muebleBanio;
Model toalla;
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

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.2f, 0.5f);

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


	pisoTexture.LoadTextureA();
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

	// INICIALIZACIÓN DE MODELOS

	casa = Model();
	sofa = Model();
	cama = Model();
	mesa = Model();
	tina = Model();
	escusado = Model();
	muebleBanio = Model();
	toalla = Model();
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

	casa.LoadModel("Models/Casa.obj");
	sofa.LoadModel("Models/Sofa.obj");
	cama.LoadModel("Models/Cama.obj");
	mesa.LoadModel("Models/Escritorio.obj");
	tina.LoadModel("Models/Tina.obj");
	escusado.LoadModel("Models/Escusado.obj");
	muebleBanio.LoadModel("Models/MuebleBanio.obj");
	toalla.LoadModel("Models/Toalla.obj");
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

	


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);


	int hora = 0;
	double paso = 3;
	float t=0.0;
	bool f1 = 1,f2 = 1;
	AjusteModelo ajuste = AjusteModelo(5);
	
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
		printf("Hora: %i\n", hora);
		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		ajuste.ajustar(mainWindow.getsKeys());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);



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

		//Casa
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f,2.0f,0.0f));
		model = glm::scale(model, glm::vec3(4.0f,4.0f,4.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		casa.RenderModel();
		
		//Sofa
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(17.78f,2.74f,-16.0f));
		model = glm::scale(model, glm::vec3(0.21f,0.21f,0.21f));
		model = glm::rotate(model, 90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		sofa.RenderModel();
		
		//Cama 1
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(37.26f,2.82f,6.18f));
		model = glm::scale(model, glm::vec3(0.30f,0.30f,0.30f));
		model = glm::rotate(model, -90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();
		
		//Cama 2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.20f,2.82f,6.24f));
		model = glm::scale(model, glm::vec3(0.30f,0.30f,0.30f));
		model = glm::rotate(model, 90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();
		
		//Cama 3
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-18.14f,2.82f,8.74f));
		model = glm::scale(model, glm::vec3(0.30f,0.30f,0.30f));
		model = glm::rotate(model, -90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cama.RenderModel();

		
		//Cama 4
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-5.48f,2.82f,8.8f));
		model = glm::scale(model, glm::vec3(0.30f,0.30f,0.30f));
		model = glm::rotate(model, 90*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
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
		model = glm::translate(model, glm::vec3(0.28f, 2.66f, -3.18f));
		model = glm::scale(model, glm::vec3(1.95f, 1.95f, 1.95f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cocina.RenderModel();
	

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(ajuste.getX(), ajuste.getY(), ajuste.getZ()));
		model = glm::scale(model, glm::vec3(ajuste.getEscala(), ajuste.getEscala(), ajuste.getEscala()));
		model = glm::rotate(model, ajuste.getRotacion()*toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		switch (ajuste.getModelo()) {
		case 0:
			audi.RenderModel();
			break;
		case 1:
			avion.RenderModel();
			break;
		case 2:
			cocheRC.RenderModel();
			break;
		case 3:
			balon.RenderModel();
			break;
		case 4:
			toalla.RenderModel();
			break;
		default:
			break;
		}





		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}