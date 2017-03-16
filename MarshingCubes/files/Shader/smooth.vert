#version 420

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 rotacion;
uniform vec3 color;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 text;

out vec3 vColor;

uniform vec3 lightDir;
uniform vec3 lightSpecular;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

uniform vec3 MaterialSpecular;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;
uniform vec3 ModelLight;

uniform float shiny;
 
void main( void )
{
 
	vec3 normales = normal;
	vec2 texturas = text;
	mat4 modelo = model*rotacion;
	mat4 mv = view*modelo;
	mat4 mvi = transpose(inverse(mv));//modelview inverse
	gl_Position = projection*mv*vec4(vertex,1.0f); //posicion al final, multiplicar por las transformadas

	vec4 vertexEyeSpace  = mv*vec4(vertex,1.0f);
	vec3 normalEyeSpace  = vec3(normalize(mvi* vec4(normal,0.0)));
	vec3 viewDirection = normalize(vec3(vec3(0.0, 0.0, -1.7320508) - vertexEyeSpace.xyz));
	vec3 L = normalize(lightDir.xyz - vertexEyeSpace.xyz);
	vec3 E = normalize(vertexEyeSpace.xyz);
	vec3 N = normalEyeSpace;
	vec3 R = normalize(reflect(L,N));
	vec3 iAmb = lightAmbient * MaterialAmbient;
	vec3 globalAmbient = ModelLight*MaterialAmbient;
	vec3 iDiff = lightDiffuse * max( 0.0, dot(N, -L) ) * MaterialDiffuse; 
	vec3 iSpec;
	if(dot(normalEyeSpace, -lightDir.xyz) < 0.0){
      iSpec = vec3(0.0, 0.0, 0.0);
	}else{
		iSpec = (MaterialSpecular*lightSpecular)*pow(max(dot(R,viewDirection),0.0),0.3*shiny);
	}
	vColor = (iAmb + globalAmbient + iDiff + iSpec);

}