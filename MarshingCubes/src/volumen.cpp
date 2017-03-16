#pragma once
#include "volumen.h"
#include "Casos.h"

volumen::volumen(){
	treshold = 128;
	SliceX = 0;
	SliceY = 0;
	SliceZ = 0;
	vertices.clear();
	triangles.clear();

	//Cableo una matriz para saber quien es vecino de quien en estructura
	Neighbor[0][0] = -1, Neighbor[0][1] = 1, Neighbor[0][2] = 0,Neighbor[0][3] = -1, 
	Neighbor[0][4] = 2, Neighbor[0][5] = -1,Neighbor[0][6] = -1,Neighbor[0][7] = -1;


	Neighbor[1][0] = 1, Neighbor[1][1] = -1, Neighbor[1][2] = -1,Neighbor[1][3] = 0,
	Neighbor[1][4] = -1, Neighbor[1][5] = 2, Neighbor[1][6] = -1,Neighbor[1][7] = -1;

	Neighbor[2][0] = 0, Neighbor[2][1] = -1, Neighbor[2][2]= -1, Neighbor[2][3] = 1,
	Neighbor[2][4] = -1, Neighbor[2][5] = -1, Neighbor[2][6] = 2,Neighbor[2][7] =-1;

	Neighbor[3][0] = -1, Neighbor[3][1] = 0, Neighbor[3][2] = 1,Neighbor[3][3] = -1,
	Neighbor[3][4] = -1, Neighbor[3][5] = -1, Neighbor[3][6] = -1, Neighbor[3][7] = 2;

	Neighbor[4][0] = 2, Neighbor[4][1] = -1, Neighbor[4][2] = -1, Neighbor[4][3] = -1,
	Neighbor[4][4] = -1, Neighbor[4][5] = 1, Neighbor[4][6] = 0, Neighbor[4][7] = -1;

	Neighbor[5][0] = -1, Neighbor[5][1] = 2, Neighbor[5][2] = -1, Neighbor[5][3] = -1,
	Neighbor[5][4] = 1, Neighbor[5][5] = -1, Neighbor[5][6] = -1, Neighbor[5][7] = 0;

	Neighbor[6][0] = -1, Neighbor[6][1] = -1, Neighbor[6][2] = 2, Neighbor[6][3] = -1,
	Neighbor[6][4] = 0, Neighbor[6][5] = -1, Neighbor[6][6] = -1, Neighbor[6][7] = 1;

	Neighbor[7][0] = -1, Neighbor[7][1] = -1, Neighbor[7][2] = -1, Neighbor[7][3] = 2,
	Neighbor[7][4] = -1, Neighbor[7][5] = 0, Neighbor[7][6] = 1, Neighbor[7][7] = -1;

}

volumen::~volumen(){
	free (values);

	for (unsigned int i=0; i<SliceX; ++i){
		for (unsigned int j=0; j<SliceY; ++j){
			free(estructura[i][j]);
		}
		free(estructura[i]);
	}
	free(estructura);

	if(!triangles.empty())
		triangles.clear();
	if(!vertices.empty()){
		for(GLuint i =0;i<vertices.size();++i)
			delete (vertices[i]);
		vertices.clear();
	}

}

void volumen::setTreshold(unsigned char umbral){
	  treshold = umbral;
}

bool volumen::cargarVolumen(string archivo,unsigned int umbral, unsigned int TamX,unsigned int TamY,unsigned int TamZ, float ScaleX, float ScaleY, float ScaleZ){
	if(archivo.compare("")==0) return false;
	if(vertices.size() >0) clean();

	treshold = umbral;
	nombre = archivo;
	SliceX = TamX;
	SliceY = TamY;
	SliceZ = TamZ;
	//Se inicializa la estructura
	values =  (unsigned char ***) malloc(SliceX*sizeof(unsigned char  **));
	for (unsigned int i=0; i<SliceX; ++i){
		values[i] = (unsigned char  **) malloc(SliceY*sizeof(unsigned char *));
		for (unsigned int j=0; j<SliceY; ++j)
			values[i][j] = (unsigned char  *) malloc(SliceZ*sizeof(unsigned char ));
	}

	//Se leen los datos
	ifstream entrada(nombre.c_str(),ios::in | ios::binary);
	char *auxValue = (char*) malloc(sizeof(char)*SliceX*SliceY*SliceZ);
	entrada.read(auxValue,SliceX*SliceY*SliceZ);
	for(int k=0;k<SliceZ;++k)
		for(int j=0;j<SliceY;++j)
			for(int i=0;i<SliceX;++i)
				values[i][j][k] = auxValue[i+j*SliceX+k*SliceX*SliceY];
	free (auxValue);
	entrada.close();
	//Fin de lectura

	//Se declaran los tamanos y las distancias		
	difX = ScaleX/(float)(TamX-1.0f);
	difY = ScaleY/(float)(TamY-1.0f);
	difZ = ScaleZ/(float)(TamZ-1.0f); 
	Scalax = ScaleX/2.0f;
	Scalay = ScaleY/2.0f;
	Scalaz = ScaleZ/2.0f;
		
	estructura = (node ***) malloc(SliceX*sizeof(node **));
	for (unsigned int i=0; i<SliceX; ++i){
		estructura[i] = (node **) malloc(SliceY*sizeof(node*));
		for (unsigned int j=0; j<SliceY; ++j){
			estructura[i][j] = (node *) malloc(4*sizeof(node));
		}
	}	 

	return true;	
};

void volumen::BuildSurface(){
	int clasificacion;
	for(unsigned int c=0;c<SliceZ-1;++c){
		for(unsigned int i=0;i<SliceX-1;++i){
			for(unsigned int j=0;j<SliceY-1;++j){
				ObtainVertex(i,j,c)	;
				MakeTriangles(i,j,c);
			}
		}
	}
}

void volumen::ObtainVertex(int x,int y,int z){
	//Determino los putnos solamente en las aristas necesarias y calculo sus normales
	Det(5,7,x,y,z);
	Det(6,7,x,y,z); 
	Det(3,7,x,y,z);

	if(z == 0){
		Det(6,4,x,y,z);
		Det(2,6,x,y,z);
		if(x == 0){
			Det(0,4,x,y,z); 
			Det(4,5,x,y,z); 
			Det(5,1,x,y,z);
			
			if(y == 0){
				Det(0,1,x,y,z); 
				Det(0,2,x,y,z); 
				Det(2,3,x,y,z);
				Det(1,3,x,y,z);
			}
		}else{
			if(y== 0){
				Det(0,2,x,y,z); 
				Det(2,3,x,y,z);
				Det(1,3,x,y,z);
			}
		}
	}else{
		if(x == 0){
			Det(4,5,x,y,z); 
			Det(5,1,x,y,z);
			if(y == 0){
				Det(0,1,x,y,z); 
				Det(1,3,x,y,z); 
				Det(2,3,x,y,z);
			}
		}else{
			if(y == 0){
				Det(1,3,x,y,z); 
				Det(2,3,x,y,z);
			}
		}
	}

}

void volumen::Det(int v1, int v2,int x, int y, int z){
	int x1,y1,z1,x2,y2,z2;
	//Determino la posicion en la matriz de v1
	switch (v1){
		case(0):
			x1=x;y1=y;z1=z;
		break;
		case(1):
			x1=x;y1=y;z1=z+1;
		break;
		case(2):
			x1=x+1;y1=y;z1=z;
		break;
		case(3):
			x1=x+1;y1=y;z1=z+1;
		break;
		case(4):
			x1=x;y1=y+1;z1=z;
		break;
		case(5):
			x1=x;y1=y+1;z1=z+1;
		break;
		case(6):
			x1=x+1;y1=y+1;z1=z;
		break;
		case(7):
			x1=x+1;y1=y+1;z1=z+1;
		break;
		default: break;
	}

	//Determino Posicion Matrix para v2
	switch (v2){
		case(0):
			x2=x;y2=y;z2=z;
		break;
		case(1):
			x2=x;y2=y;z2=z+1;
		break;
		case(2):
			x2=x+1;y2=y;z2=z;
		break;
		case(3):
			x2=x+1;y2=y;z2=z+1;
		break;
		case(4):
			x2=x;y2=y+1;z2=z;
		break;
		case(5):
			x2=x;y2=y+1;z2=z+1;
		break;
		case(6):
			x2=x+1;y2=y+1;z2=z;
		break;
		case(7):
			x2=x+1;y2=y+1;z2=z+1;
		break;
		default: break;
	}
	glm::vec3 point1(((float)x1)*difX-Scalax,-1*((float)y1*difY-Scalay),(float)z1*difZ-Scalaz);	
	glm::vec3 point2(((float)x2)*difX-Scalax,-1*((float)y2*difY-Scalay),(float)z2*difZ-Scalaz);

	float position_value_v1 = values[x1][y1][z1];
	float position_value_v2	= values[x2][y2][z2];
	vertex *vertice = NULL;
	if(abs(position_value_v1 - position_value_v2) > 0.000001){
		float t = ((float)treshold - position_value_v1)/(position_value_v2 - position_value_v1);
		if(t >= 0 && t <=1){
			float aux_vec1,aux_vec2;
			glm::vec3 normal1,normal2;
			//Se invierte la resta para la y ya que la y esta invertida con respecto al eje de coordenadas
			//Calculo la normal para v1
			aux_vec1 = (x1+1 < SliceX)? (float)values[x1+1][y1][z1] : 0.0f;
			aux_vec2 = (x1-1 >= 0)? (float)values[x1-1][y1][z1] : 0.0f;
			normal1.x = (aux_vec1 - aux_vec2)/Scalax; 

			aux_vec1 = (y1+1 < SliceY)? (float)values[x1][y1+1][z1] : 0.0f;
			aux_vec2 = (y1-1 >= 0)? (float)values[x1][y1-1][z1] : 0.0f;
			normal1.y = (aux_vec1 - aux_vec2)/Scalay; 

			aux_vec1 = (z1+1 < SliceZ)? (float)values[x1][y1][z1+1] : 0.0f;
			aux_vec2 = (z1-1 >= 0)? (float)values[x1][y1][z1-1] : 0.0f;
			normal1.z = (aux_vec1 - aux_vec2)/Scalaz; 

			//Calculo la normal para v2
			aux_vec1 = (x2+1 < SliceX)? (float)values[x2+1][y2][z2] : 0.0f;
			aux_vec2 = (x2-1 >= 0)? (float)values[x2-1][y2][z2] : 0.0f;
			normal2.x = (aux_vec1 - aux_vec2)/Scalax; 

			aux_vec1 = (y2+1 < SliceY)? (float)values[x2][y2+1][z2] : 0.0f;
			aux_vec2 = (y2-1 >= 0)? (float)values[x2][y2-1][z2] : 0.0f;
			normal2.y = (aux_vec1 - aux_vec2)/Scalay; 

			aux_vec1 = (z2+1 < SliceZ)? (float)values[x2][y2][z2+1] : 0.0f;
			aux_vec2 = (z2-1 >= 0)? (float)values[x2][y2][z2-1] : 0.0f;
			normal2.z = (aux_vec1 - aux_vec2)/Scalaz; 

			vertice = new vertex();
			vertice->normal=normal1*(1.0f-t) + normal2*t;
			vertice->normal = glm::normalize(vertice->normal);
			vertice->position = point1*(1.0f-t) + point2*t;
			vertice->id = vertices.size();
			vertices.push_back(vertice);
		}
	}
	estructura[x1][y1][z1%4].edge[Neighbor[v1][v2]] = estructura[x2][y2][z1%4].edge[Neighbor[v2][v1]] = vertice;
}


int volumen::Det_Position(int v1, int v2,int x, int y, int z){
	int x1,y1,z1;
	//Determino la posicion en la matriz de v1
	switch (v1){
		case(0):
			x1=x;y1=y;z1=z;
		break;
		case(1):
			x1=x;y1=y;z1=z+1;
		break;
		case(2):
			x1=x+1;y1=y;z1=z;
		break;
		case(3):
			x1=x+1;y1=y;z1=z+1;
		break;
		case(4):
			x1=x;y1=y+1;z1=z;
		break;
		case(5):
			x1=x;y1=y+1;z1=z+1;
		break;
		case(6):
			x1=x+1;y1=y+1;z1=z;
		break;
		case(7):
			x1=x+1;y1=y+1;z1=z+1;
		break;
		default: break;
	}
	return estructura[x1][y1][z1%4].edge[Neighbor[v1][v2]]->id;
}

void volumen::MakeTriangles(int x,int y, int z){
	int caso = 0;
	int caso_original;
	for(int j=y;j<y+2;++j){
		for(int i=x;i<x+2;++i){
			for(int c=z;c<z+2;++c){
				if(values[i][j][c] > treshold)
					caso = (caso << 1)+1;
				else
					caso = (caso << 1);
			}
		}
	}
	caso_original = caso;

	if(caso >= 128)
		caso = 255-caso;

	int off = offset[caso];
	int size = cases[off++];
	int edge1, edge2, edge3;

	for(int w=0; w < size ;w++){
		if(caso_original >= 128){
			edge1 = cases[off++];
			edge2 = cases[off++];	
			edge3 = cases[off++];	
		}else{
			edge1 = cases[off++];
			edge3 = cases[off++];
			edge2 = cases[off++];	
		}
		CreateTriangles(edge1/10,edge1%10,edge2/10,edge2%10,edge3/10,edge3%10,x,y,z);
	}

}

void volumen::CreateTriangles(int a1,int a2,int b1,int b2,int c1,int c2, int x, int y, int z){
	triangle *tri = new triangle();
	tri->v[0] = Det_Position(a1,a2,x,y,z);
	tri->v[1] = Det_Position(b1,b2,x,y,z);
	tri->v[2] = Det_Position(c1,c2,x,y,z);
	triangles.push_back(tri);
}

void volumen::clean(){

	for (unsigned int i=0; i<SliceX; ++i){
		for (unsigned int j=0; j<SliceY; ++j){
			free(estructura[i][j]);
			free(values[i][j]);
		}
		free(estructura[i]);
		free(values[i]);
	}
	free(estructura);
	free(values);

	if(!triangles.empty())
		triangles.clear();
	if(!vertices.empty())
		vertices.clear();
}

void volumen::restart(){
	if(!triangles.empty())
		triangles.clear();
	if(!vertices.empty())
		vertices.clear();
}