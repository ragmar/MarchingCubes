#version 420

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 rotacion;
uniform vec3 color;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 text;

out vec3 normales;
out vec2 texturas;
out vec3 colores;
out vec4 vertexEyeSpace;
out vec3 normalEyeSpace;

void main()
{

colores = color;
normales = normal;
texturas = text;
mat4 modelo = model*rotacion;
mat4 mv = view*modelo;
mat4 mvi = transpose(inverse(mv));//modelview inverse
gl_Position = projection*mv*vec4(vertex,1.0f); //posicion al final, multiplicar por las transformadas

vertexEyeSpace  = mv*vec4(vertex,1.0f);
normalEyeSpace  = vec3(normalize(mvi* vec4(normal,0.0)));
}