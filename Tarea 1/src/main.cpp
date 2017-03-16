#pragma comment (lib,"lib/freeglut.lib")
#pragma comment (lib,"lib/AntTweakBar.lib")
#pragma comment (lib,"lib/glew32.lib")
#include "../tools/GL/glew.h"
#include "../tools/GL/freeglut.h"
#include "../tools/AntTweakBar/AntTweakBar.h"
#include "../tools/glm/glm.hpp" // glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "../tools/glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "../tools/glm/gtc/type_ptr.hpp" // 
#include "../tools/glm/gtx/transform2.hpp" // for glm::lookAtglm::value_ptr

#include <iostream>
#include <vector>
#include "shaders.h"
#include "Quaternion.h"
#include "volumen.h"
#include "carga_archivo.h"

//Defines
#define PI 3.141592f
#define SQRT3 1.7320508f
#define LIGHT_ACTIVADA 1
#define BUFFER_OFFSET(i) (reinterpret_cast<void*>(i))
using namespace std;

//Shaders
Shaders *sh,*flat,*smooth;

//Matrices Globales
glm::mat4 Identity = glm::mat4(1.0f);
glm::mat4 Scalar = glm::scale(1.0f,1.0f,1.0f);
glm::mat4 Traslate = glm::translate(0.0f,0.0f,0.0f);
glm::mat4 Project;
glm::mat4 Vista;

//Luces
GLfloat shiny;

GLfloat materialAmbiental[3];
GLfloat materialDifuso[3];
GLfloat materialEspecular[3];
GLfloat AmbienteGlobal[3];

GLfloat luzAmbiental[3];
GLfloat luzDifusa[3];
GLfloat luzEspecular[3];
GLfloat luzDireccion[3];

//Buffer Object
GLuint myVBO;
GLuint myVBOIndex;
GLuint myVBOLines;
GLuint myVBOIndexLines;
GLuint Indices;
GLint w;
GLint h;
bool dibujar;
bool phong=true;
bool smoot=false;

//Variables Globales
std::string nombre="";
float X=0.0f,Y=0.0f,Z=0.0f;
bool traslationOnZ;
float escalamiento = 1.0f;
glm::vec3 traslacion;
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
bool firstTime=true;

//Volume
volumen *Vol;
unsigned int TamX,TamY,TamZ;
float ScaleX,ScaleY,ScaleZ;
char name[100];
unsigned char treshold;
unsigned char old_treshold;

unsigned int poligons;


//FPS
int frame=0,timef,timebase=0; 
char fps[100];


struct complete_vertex{
	GLfloat vn[6];
};


/* ====================================////////////////////////////////================
											FUNCIONES
=======================================////////////////////////////////==============*/

void AgregarAlBuffer(){
	vector <complete_vertex> arreglo_vertices;
	complete_vertex auxiliar;
	GLuint *arreglo_indices;
	poligons = Vol->triangles.size();
	arreglo_indices = (GLuint*)malloc(sizeof(GLuint)*Vol->triangles.size()*3);
	Indices = Vol->triangles.size()*3;
	if(Vol->triangles.size()>0){
		for(unsigned int i=0;i< Vol->triangles.size();i++){
					auxiliar.vn[0] =  Vol->vertices[ Vol->triangles[i]->v[0]]->position.x;
					auxiliar.vn[1] =  Vol->vertices[ Vol->triangles[i]->v[0]]->position.y;
					auxiliar.vn[2] =  Vol->vertices[ Vol->triangles[i]->v[0]]->position.z;
					auxiliar.vn[3] =  Vol->vertices[ Vol->triangles[i]->v[0]]->normal.x;
					auxiliar.vn[4] =  Vol->vertices[ Vol->triangles[i]->v[0]]->normal.y;
					auxiliar.vn[5] =  Vol->vertices[ Vol->triangles[i]->v[0]]->normal.z;
					arreglo_indices[i*3] = i*3;
					arreglo_vertices.push_back(auxiliar);

					auxiliar.vn[0] =  Vol->vertices[ Vol->triangles[i]->v[1]]->position.x;
					auxiliar.vn[1] =  Vol->vertices[ Vol->triangles[i]->v[1]]->position.y;
					auxiliar.vn[2] =  Vol->vertices[ Vol->triangles[i]->v[1]]->position.z;
					auxiliar.vn[3] =  Vol->vertices[ Vol->triangles[i]->v[1]]->normal.x;
					auxiliar.vn[4] =  Vol->vertices[ Vol->triangles[i]->v[1]]->normal.y;
					auxiliar.vn[5] =  Vol->vertices[ Vol->triangles[i]->v[1]]->normal.z;
					arreglo_indices[i*3+1] = i*3+1;
					arreglo_vertices.push_back(auxiliar);

					auxiliar.vn[0] =  Vol->vertices[ Vol->triangles[i]->v[2]]->position.x;
					auxiliar.vn[1] =  Vol->vertices[ Vol->triangles[i]->v[2]]->position.y;
					auxiliar.vn[2] =  Vol->vertices[ Vol->triangles[i]->v[2]]->position.z;
					auxiliar.vn[3] =  Vol->vertices[ Vol->triangles[i]->v[2]]->normal.x;
					auxiliar.vn[4] =  Vol->vertices[ Vol->triangles[i]->v[2]]->normal.y;
					auxiliar.vn[5] =  Vol->vertices[ Vol->triangles[i]->v[2]]->normal.z;
					arreglo_indices[i*3+2] = i*3+2;
					arreglo_vertices.push_back(auxiliar);

		}
		if(firstTime){
			glGenBuffers(1, &myVBO);
			glGenBuffers(1, &myVBOIndex);
			firstTime = false;
		}

		// bind buffer for positions and copy data into buffer
		// GL_ARRAY_BUFFER is the buffer type we use to feed attributes
		glBindBuffer(GL_ARRAY_BUFFER, myVBO);
 
		// feed the buffer, and let OpenGL know that we don't plan to
		// change it (STATIC) and that it will be used for drawing (DRAW)
		glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, arreglo_vertices.size() * sizeof(complete_vertex), &arreglo_vertices[0], GL_DYNAMIC_DRAW);

		//bind buffer for positions and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myVBOIndex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,  Vol->triangles.size()*3 * sizeof(GL_UNSIGNED_INT), arreglo_indices, GL_DYNAMIC_DRAW);
	
		//Disable Buffers and vertex attributes
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		dibujar = true;
	}else{
		dibujar = false;
	}
}

/* ====================================////////////////////////////////================
											FUNCIONES AntTweakBar
=======================================////////////////////////////////==============*/
void TW_CALL CopyStdStringToClient(string& destinationClientString, const string& sourceLibraryString)
{
  // Copy the content of souceString handled by the AntTweakBar library to destinationClientString handled by your application
  destinationClientString = sourceLibraryString;
}

void TW_CALL pressExit(void *clientData)
{ 
	TwTerminate();
	exit(0);
}

void TW_CALL CargarModelo(void *clientData){
	string archivo=openfilenameRAW();
	if(archivo!=""){
		if(!Vol->cargarVolumen(archivo,treshold,TamX,TamY,TamZ,ScaleX,ScaleY,ScaleZ)){
			printf("Error cargando el raw\n");
		}else{

		size_t found;
		found=archivo.rfind("\\");
		if (found!=string::npos){
			archivo=archivo.substr(found+1);
		}

		strcpy_s(name,archivo.c_str());
		printf("%s Cargado Exitosamente\n",name);
		Vol->BuildSurface();
		printf("Superficie Agregada\n ************************\n");
		AgregarAlBuffer();
		}
	}
} 

void TW_CALL Reset(void *clientData)
{ 
	g_Rotation[0] = 0.0f;
	g_Rotation[1] =	0.0f;
	g_Rotation[2] = 0.0f;
	g_Rotation[3] = 1.0f;


	traslationOnZ = false;

	traslacion.x = traslacion.y = traslacion.z = 0.0f;
	escalamiento=1.0f;
	X=0.0f,Y=0.0f,Z=0.0f;
}

void TW_CALL GetTresholdCallBack(void *value, void *clientData)
{ 
    *(unsigned char *)value = treshold;  // for instance
}
void TW_CALL SetTresholdCallBack(const void *value, void *clientData)
{ 
    treshold = *(const unsigned char *)value;  // for instance
	if(old_treshold != treshold){
		old_treshold = treshold;
		if(dibujar){
			Vol->setTreshold(treshold);
			Vol->restart();
			Vol->BuildSurface();
			printf("Superficie Agregada\n ************************\n");
			AgregarAlBuffer();
		}
	}
}


void TW_CALL ActivatePhong(void *clientData)
{ 
	phong = true;
	smoot  = false;
	TwDefine("Opciones/Phong  readonly=true \n"
			"Opciones/Smooth  readonly=false \n");
}
void TW_CALL ActivateSmooth(void *clientData)
{ 
	smoot = true;
	phong  = false;
	TwDefine("Opciones/Phong  readonly=false \n"
			"Opciones/Smooth  readonly=true \n");
}
/* ====================================////////////////////////////////================
											INICIALIZADORES
=======================================////////////////////////////////==============*/

void initAntTweak(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(w,h);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	TwBar *myBar;
	myBar = TwNewBar("Opciones");
	TwCopyStdStringToClientFunc(CopyStdStringToClient);
	TwAddVarRW(myBar, "nombredelString", TW_TYPE_CSSTRING(sizeof(name)),name, "label='File' group='Volume'");
	TwAddVarCB(myBar,"treshold",TW_TYPE_UINT8,SetTresholdCallBack,GetTresholdCallBack,&treshold,"label='treshold' group='Volume' max=254");
	TwAddVarRO(myBar,"pol",TW_TYPE_UINT32,&poligons,"label='poligons' group='Volume'");

	TwAddButton(myBar,"Phong", ActivatePhong,NULL,"label='Phong' group='Shader' ");
	TwAddButton(myBar,"Smooth", ActivateSmooth,NULL,"label='Smooth' group=Shader");
	//TwAddButton(myBar,"Smooth", ActivateFlat,NULL,"label='Smooth' group=Shader");
	//TwAddVarRO(myBar,"FPS",TW_TYPE_CSSTRING(sizeof(fps)),fps,"label='FPS' group='Modelo'");

	TwAddButton(myBar,"Load", CargarModelo,NULL,"label='Cargar Modelo' group='Load Volume'");
	TwAddVarRW(myBar, "TamanoX", TW_TYPE_UINT16,&TamX, "label='Slice X' group='Load Volume'");
	TwAddVarRW(myBar, "TamanoY", TW_TYPE_UINT16,&TamY, "label='Slice Y' group='Load Volume'");
	TwAddVarRW(myBar, "TamanoZ", TW_TYPE_UINT16,&TamZ, "label='Slice Z' group='Load Volume'");
	TwAddVarRW(myBar, "SX", TW_TYPE_FLOAT,&ScaleX, "label='Scale X' group='Load Volume' min=0 step=0.01");
	TwAddVarRW(myBar, "SY", TW_TYPE_FLOAT,&ScaleY, "label='Scale Y' group='Load Volume' min=0 step=0.01");
	TwAddVarRW(myBar, "SZ", TW_TYPE_FLOAT,&ScaleZ, "label='Scale Z' group='Load Volume' min=0 step=0.01");


	TwAddVarRW(myBar,"Scal",TW_TYPE_FLOAT, &escalamiento,"label='Escalamineto' group='Transformacion' min=0 step=0.01");
	TwAddVarRW(myBar, "Tras", TW_TYPE_BOOLCPP, &traslationOnZ, "label='Traslacion Z' group='Transformacion'");
    TwAddVarRW(myBar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation,  " label='Object rotation' group='Transformacion' ");

	TwAddVarRW(myBar,"Shiny",TW_TYPE_FLOAT,&shiny, "label=Brillo group=Material min=0 max=128");
	TwAddVarRW(myBar,"MatDif",TW_TYPE_COLOR3F,materialDifuso, "label='Material Difuso' group=Material");
	TwAddVarRW(myBar,"MatSpe",TW_TYPE_COLOR3F,materialEspecular, "label='Material Especular' group=Material");
	TwAddVarRW(myBar,"MatAmb",TW_TYPE_COLOR3F,materialAmbiental, "label='Material Ambiental' group=Material");

	TwAddVarRW(myBar,"AmbGlobal",TW_TYPE_COLOR3F,AmbienteGlobal, "label='Ambiente Global' group=Material");


	TwAddVarRW(myBar, "LigthDiff", TW_TYPE_COLOR3F,luzDifusa, " label='Color Difuso'  group='Luz'");
	TwAddVarRW(myBar, "LightSpe", TW_TYPE_COLOR3F,luzEspecular, " label='Color Especular'  group='Luz'");
	TwAddVarRW(myBar, "LightAmb", TW_TYPE_COLOR3F,luzAmbiental, " label='Color Ambiental'  group='Luz'");
	TwAddVarRW(myBar, "LightDir", TW_TYPE_DIR3F, luzDireccion, " label='Direccion Luz'  group='Luz'");

	TwAddButton(myBar,"Clean", Reset,NULL,"label='Reset' group=Archivo");
	TwAddButton(myBar,"Salir", pressExit,NULL,"label='Salir' group=Archivo");

	TwDefine("Opciones/Phong  readonly=true \n");
}

void Init_Light(){

	luzAmbiental[0] = luzAmbiental[1] = luzAmbiental[2] = 0.0f;
	luzDifusa[0] = luzDifusa[1] = luzDifusa[2] = 0.0f; 	luzDifusa[0] = 1.0f;
	luzEspecular[0] = luzEspecular[1] = luzEspecular[2] = 0.0f; luzEspecular[1] = 1.0f; 

	luzDireccion[0] = luzDireccion[1] = luzDireccion[2] = 500.0f;

	materialAmbiental[0] = materialAmbiental[1] = materialAmbiental[2] = 0.25f;
	materialDifuso[0] = materialDifuso[1] = materialDifuso[2] = 1.0f; 
	materialEspecular[0] = materialEspecular[1] = materialEspecular[2] = 1.0f; 
	AmbienteGlobal[0] = AmbienteGlobal[1] = AmbienteGlobal [2] = 0.5f;

	shiny = 50.0f;

}

void init(){
	glEnable(GL_DEPTH_TEST);//zbuffer
	glEnable(GL_CULL_FACE);//BACK FACE
	glEnable(GL_NORMALIZE);
	glDepthFunc(GL_LEQUAL);
	dibujar = false;
	glewInit();
	//Load Shaders
	TamX = TamY = TamZ = 32;
	ScaleX = ScaleY = ScaleZ = 1.0f;
	treshold = 128;

	initAntTweak();

	sh = new Shaders();
	flat = new Shaders();
	smooth = new Shaders();
	if(sh->isSuported() && flat->isSuported() && smooth->isSuported()){
		cout<<"Init Model View Projection Color Shader"<<endl;
		sh->Init("../files/Shader/mvpc.vert","../files/Shader/mvpc.frag");
		smooth->Init("../files/Shader/smooth.vert","../files/Shader/smooth.frag");
		smooth->setShaders();
		sh->setShaders(); 
		//flat->setShaders();
	}else{
		exit(0);
	}
	Vol = new volumen();

}

GLvoid idle() {
	glutPostRedisplay();
}


void DrawTriangles(){
//Bind Buffers
	glBindBuffer(GL_ARRAY_BUFFER, myVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myVBOIndex);

		//Enable Clients
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(GL_FLOAT)*6, BUFFER_OFFSET(0)); //Vertex		 TOTAL
	glNormalPointer(GL_FLOAT,sizeof(GL_FLOAT)*6,BUFFER_OFFSET(sizeof(GL_FLOAT)*3));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*6, BUFFER_OFFSET(0)); //Vertex
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*6, BUFFER_OFFSET(sizeof(GL_FLOAT)*3)); //Normal Coord
		glDrawElements(GL_TRIANGLES, Indices, GL_UNSIGNED_INT, 0);	   //indices

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

		//Disable Clients
	glDisableClientState(GL_VERTEX_ARRAY);

		//Unbid Buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void usingPhong(){
	sh->Enable();
	glUniformMatrix4fv(sh->GetUniformLocation("projection"),1,false,glm::value_ptr(Project));
	glUniformMatrix4fv(sh->GetUniformLocation("view"),1,false,glm::value_ptr(Vista));
	glUniformMatrix4fv(sh->GetUniformLocation("model"),1,false,glm::value_ptr(Traslate*Scalar));

	//Luces
	glUniform3fv(sh->GetUniformLocation("lightDir"),1,luzDireccion); //direccion
	glUniform3fv(sh->GetUniformLocation("lightSpecular"),1,luzEspecular); //Especular
	glUniform3fv(sh->GetUniformLocation("lightAmbient"),1,luzAmbiental); //Ambiental
	glUniform3fv(sh->GetUniformLocation("lightDiffuse"),1,luzDifusa); //Difusa

	//Materiales
	glUniform1f(sh->GetUniformLocation("shiny"),shiny); //direccion
	glUniform3fv(sh->GetUniformLocation("MaterialSpecular"),1,materialEspecular); //Especular
	glUniform3fv(sh->GetUniformLocation("MaterialAmbient"),1,materialAmbiental); //Ambiental
	glUniform3fv(sh->GetUniformLocation("MaterialDiffuse"),1,materialDifuso); //Difusa
	glUniform3fv(sh->GetUniformLocation("ModelLight"),1,AmbienteGlobal); //Difusa
}

void usingSmooth(){
	smooth->Enable();
	glUniformMatrix4fv(smooth->GetUniformLocation("projection"),1,false,glm::value_ptr(Project));
	glUniformMatrix4fv(smooth->GetUniformLocation("view"),1,false,glm::value_ptr(Vista));
	glUniformMatrix4fv(smooth->GetUniformLocation("model"),1,false,glm::value_ptr(Traslate*Scalar));

	//Luces
	glUniform3fv(smooth->GetUniformLocation("lightDir"),1,luzDireccion); //direccion
	glUniform3fv(smooth->GetUniformLocation("lightSpecular"),1,luzEspecular); //Especular
	glUniform3fv(smooth->GetUniformLocation("lightAmbient"),1,luzAmbiental); //Ambiental
	glUniform3fv(smooth->GetUniformLocation("lightDiffuse"),1,luzDifusa); //Difusa
	glUniform3fv(smooth->GetUniformLocation("ModelLight"),1,AmbienteGlobal); //Difusa

	//Materiales
	glUniform1f(smooth->GetUniformLocation("shiny"),shiny); //direccion
	glUniform3fv(smooth->GetUniformLocation("MaterialSpecular"),1,materialEspecular); //Especular
	glUniform3fv(smooth->GetUniformLocation("MaterialAmbient"),1,materialAmbiental); //Ambiental
	glUniform3fv(smooth->GetUniformLocation("MaterialDiffuse"),1,materialDifuso); //Difusa
}


void Draw(){
		//Transformaciones
	float Rotacion[16]; //matriz con la rotacion que se pasa a OpenGL
	Traslate = glm::translate(traslacion.x,traslacion.y,traslacion.z);
	Scalar = glm::scale(escalamiento,escalamiento,escalamiento);
	ConvertQuaternionToMatrix(g_Rotation, Rotacion);//convierto quaternion 2*2 en una matriz 4*4 para que OpenGL la entienda

	if(phong){
		usingPhong();
		glUniformMatrix4fv(sh->GetUniformLocation("rotacion"),1,false,Rotacion);
	}

	if(smoot){
		usingSmooth();
		glUniformMatrix4fv(smooth->GetUniformLocation("rotacion"),1,false,Rotacion);
	}
	if(dibujar){
		DrawTriangles();
	}

	if(phong)
		sh->Disable();
	if(smoot)
		smooth->Disable();

	GLenum error = glGetError(); 

	if (error != GL_NO_ERROR){
		std::cout << "[OpenGL ERROR]: " << error << " " << gluErrorString(error) << std::endl;
		assert(error != GL_NO_ERROR);
		system("pause");
		exit(0);
	}
				
		
}
 
/* ====================================////////////////////////////////================
											INTERACCIONES
=======================================////////////////////////////////==============*/

void teclado(unsigned char key, int x, int y){
	if(	!TwEventKeyboardGLUT(key,x,y)){
	}
	glutPostRedisplay();
}

void teclado_especiales(int key, int x, int y){
	if(	!TwEventSpecialGLUT(key,x,y)){
	
	}
	glutPostRedisplay();
}

void left_click(int boton, int estado, int x, int y) 
{
	float x1 =2.0f*(float)x/(float)w-1.0f;//redimenciono los puntos al tamaño de mi ventana
	float y1= - (2.0f*(float)y/h - 1.0f);
	if( !TwEventMouseButtonGLUT(boton,estado,x,y) )  // send event to AntTweakBar
	{
		if(estado==GLUT_DOWN){
			X=(float)x;
			Y=(float)y;
			Z=(float)y;
		}
	}
	glutPostRedisplay();
}

void movimiento(int x,int y){
	if(!TwEventMouseMotionGLUT(x,y) ){
		float xn=0.0f,yn=0.0f,zn=0.0f;
		if(traslationOnZ){
			zn=(-Z+(float)y)/(float)h;
			traslacion.z=traslacion.z+(zn);

		}else{
			xn=(-X+(float)x)/(float)w;
			yn=(Y+(float)-y)/(float)h;
			traslacion.x = traslacion.x+(xn);
			traslacion.y = traslacion.y+(yn);

		}

		X=(float)x;
		Y=(float)y;
		Z=(float)y;
	}

	glutPostRedisplay();

}

void renderScene(void) {//renderizar
	frame++;
    timef=glutGet(GLUT_ELAPSED_TIME);
    if (timef - timebase > 1000) {
           sprintf(fps,"%4.2f",frame*1000.0/(timef-timebase));
            timebase = timef;
            frame = 0;
    }
	glClearColor(0.0, 0.0, 0.0, 0.0);	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_NORMALIZE);
	//camara();
	//LucesDir();
	//Vol->Draw();
	Draw();

	TwDraw();
	GLenum error = glGetError(); 

	if (error != GL_NO_ERROR){
		std::cout << "[OpenGL ERROR]: " << error << " " << gluErrorString(error) << std::endl;
		assert(error != GL_NO_ERROR);
		system("pause");
		exit(0);
	}
	glutSwapBuffers();

}


void reshape(int x, int y){//redimensiono la pantalla
	
	w = x;
	h = y;
	glViewport(0, 0, w, h);	
	Vista = glm::lookAt(glm::vec3(0.0f,0.0f,SQRT3),glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,1.0f,0.0f));
	Project = glm::perspective(	45.0f, ((GLfloat) w) / ((GLfloat) h), 0.5f, 500.0f);
	//Project = glm::ortho(-1.0,1.0,-1.0,1.0,-1.0,1.0);
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(45.0f, ((GLfloat) w) / ((GLfloat) h), 1.0f, 500000.0f);
	//glTranslatef(0.0f,0.0f,-SQRT3);
	//glMatrixMode(GL_MODELVIEW);
	TwWindowSize(w, h);
	//glLoadIdentity();
}



//Main Principal
int main(int argc, char *argv[])
{	
	w = 800;
	h = 600;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(w,h);
	glutCreateWindow("T.C.G. - Tarea 3 - Alejandro Sans");


	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(teclado);//para usar el teclado con AntTweakBar fluidamente
	glutSpecialFunc(teclado_especiales);
	glutMouseFunc(left_click);
	glutMotionFunc(movimiento);//para mover el mouse con AntTweakBar fluidamente
	init();
	Init_Light();
	glutMainLoop();
	return 0;
}
