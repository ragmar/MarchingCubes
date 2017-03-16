#ifndef volumen_H
#define volumen_H

#pragma once
#pragma comment (lib,"lib/freeglut.lib")
#pragma comment (lib,"lib/glew32.lib")
#include "../tools/GL/glew.h"
#include "../tools/GL/freeglut.h"
#include "../tools/glm/glm.hpp" // glm::vec3, glm::vec4, glm::ivec4, glm::mat4


#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>

using namespace std;

class volumen
{
	struct vertex{
			glm::vec3 position;
			glm::vec3 normal;
			int id;
	};

	struct triangle{
		int v[3];
	};

	struct node{
		vertex *edge[3];
	};

	string nombre;
	unsigned char treshold;
	unsigned int SliceX,SliceY,SliceZ;
	unsigned char *** values;
	int Neighbor[8][8];
	node ***estructura;

	//FUNCIONES
	public:
		float difX,difY,difZ;
		float Scalax,Scalay,Scalaz;
		volumen();
		~volumen();
		vector <vertex *>	vertices;
		vector <triangle *> triangles;
		void restart();
		void clean();
		void setTreshold(unsigned char umbral);
		void normalizarCentralizar();
		bool cargarVolumen(string archivo,unsigned int umbral, unsigned int TamX,unsigned int TamY,unsigned int TamZ, float ScaleX, float ScaleY, float ScaleZ);
		void MakeTriangles(int x,int y,int z);
		void CreateTriangles(int a1,int a2,int b1,int b2,int c1,int c2, int x, int y, int z);
		void BuildSurface();
		void ObtainVertex(int x,int y,int z);
		void Det(int v1, int v2,int x, int y, int z);
		int Det_Position(int v1, int v2,int x, int y, int z);

};

#endif