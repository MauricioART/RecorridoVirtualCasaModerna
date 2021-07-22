#pragma once
#include<stdio.h>
#include <glew.h>
#include <glm.hpp>
#include <glfw3.h>

class AjusteModelo {
private:
	float x, y, z, escala,rotacion;
	int modelo,maxModelos;
public:
	~AjusteModelo();
	AjusteModelo(int maxModelos);
	AjusteModelo(glm::vec3 pos, int maxModelos);
	void ajustar(bool* keys);
	float getX() { return x; };
	float getY() { return y; };
	float getZ() { return z; };
	int getModelo() { return modelo; };
	float getEscala() { return escala; };
	float getRotacion() { return rotacion; };
	
};