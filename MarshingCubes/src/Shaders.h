#ifndef Shaders_H
#define Shaders_H

#pragma comment(lib,"lib/glew32.lib")
#include "../tools/GL/glew.h"
#include <stdlib.h>
#include <iostream>

class Shaders {

  public:

    Shaders();
    ~Shaders();
    bool isSuported();
    void Init(char *vertex,char *fragment);
    void setShaders();
    void Disable();
    void Enable();
    GLint GetUniformLocation(char * name);
    void BindAttribLocation(int index, char * name);


  private:

    char *textFileRead(char *fn);
    void printInfoLog(GLhandleARB obj);

  //Variables

  public:
    char *Vertex,*Fragment;
    GLhandleARB program,fprogram,vprogram;

};

#endif //Shaders_H