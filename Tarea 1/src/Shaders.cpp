#include "Shaders.h"
using namespace std;



Shaders::Shaders()
{

}

Shaders::~Shaders()
{

}

char * Shaders::textFileRead(char *fn)
{
  FILE *fp;
  char *content = NULL;

  int count=0;

  if (fn != NULL) {
    fopen_s(&fp, fn,"rt");
    if (fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

      if (count > 0) {
        content = (char *)malloc(sizeof(char) * (count+1));
        count = fread(content,sizeof(char),count,fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
  }
  return content;
}


void Shaders::printInfoLog(GLhandleARB obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
           &infologLength);

  if (infologLength > 0)
  {
  infoLog = (char *)malloc(infologLength);
  glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
  if(infoLog != NULL)
    printf("%s\n",infoLog);
  free(infoLog);
  }
}


bool Shaders::isSuported()
{
  if (!glewIsSupported("GL_VERSION_3_3")){
    cout<<"You don't support shaders with OpenGl 3.3"<<endl;
    return false;
  }
  return true;
}

void Shaders::setShaders()
{
  char *vs,*fs;

  //Asign the type of shader
  vprogram = glCreateShader(GL_VERTEX_SHADER);
  fprogram = glCreateShader(GL_FRAGMENT_SHADER);

  //We read the files
  vs = textFileRead(Vertex);
  fs = textFileRead(Fragment);

  //Transform the char tu const char
  const char * vv = vs, * ff = fs;

  //We asign them to this program
  glShaderSource(vprogram, 1, &vv,NULL);
  glShaderSource(fprogram, 1, &ff,NULL);

  //Free the used memory
  free(vs);free(fs);

  //Compile the files
  glCompileShader(vprogram);
  glCompileShader(fprogram);

  //Create the program
  program = glCreateProgram();

  //Atacch shaders to program
  glAttachShader(program,vprogram);
  glAttachShader(program,fprogram);

  //Link the program
  glLinkProgram(program);

  //Use the program
  glUseProgram(program);

  //Print if there is an error
  printInfoLog(program);

  //Use the default program
  glUseProgram(0);
}

void Shaders::Disable()
{
  glUseProgram(0);
}

void Shaders::Enable()
{
  glUseProgram(program);
}

void Shaders::Init(char *vertex,char *fragment)
{
  Vertex=vertex;
  Fragment=fragment;
}


GLint Shaders::GetUniformLocation(char * name)
{
  return glGetUniformLocation(program,name);
}


void Shaders::BindAttribLocation(int index, char * name)
{
  glBindAttribLocation(program,index,name);
}
