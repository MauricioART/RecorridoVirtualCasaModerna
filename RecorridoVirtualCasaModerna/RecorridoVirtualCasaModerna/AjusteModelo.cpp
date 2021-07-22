#include "AjusteModelo.h"

AjusteModelo::AjusteModelo(int maxModelos) {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	escala = 1.0;
	rotacion = 0.0;
	modelo = 0;
	this->maxModelos = maxModelos;
}
AjusteModelo::AjusteModelo(glm::vec3 pos, int maxModelos) {
	x = pos.x;
	y = pos.y;
	z = pos.z;
	escala = 1.0;
	rotacion = 0.0;
	modelo = 0;
	this->maxModelos = maxModelos;
}
void AjusteModelo::ajustar(bool* keys) {

	if (keys[GLFW_KEY_K])
	{
		x += 0.02;
	}
	if (keys[GLFW_KEY_H])
	{
		x -= 0.02;
	}

	if (keys[GLFW_KEY_I])
	{
		y +=  0.02;
	}

	if (keys[GLFW_KEY_Y])
	{
		y -= 0.02;
	}

	if (keys[GLFW_KEY_U])
	{
		z +=  0.02;
	}
	if (keys[GLFW_KEY_J]) {
		z -= 0.02 ;
	}
	if (keys[GLFW_KEY_O]) {
		escala += 0.005;
	}
	if (keys[GLFW_KEY_L]) {
		if(escala > 0)
			escala -= 0.005;
	}
	if (keys[GLFW_KEY_M]) {
		rotacion += 1;
	}
	if (keys[GLFW_KEY_N]) {
		rotacion -= 1;
	}
	if (keys[GLFW_KEY_P]) {
		if (modelo < maxModelos) {
			modelo++;
		}
		else {
			modelo = 0;
		}
	}
	printf("Modelo numero= %i\n", modelo);
	printf("Desplazamiento: (%f,%f,%f)\n ", x, y, z);
	printf("Escala: %f\n",escala);
	printf("Rotacion: %f\n", rotacion);
}

AjusteModelo::~AjusteModelo() {}