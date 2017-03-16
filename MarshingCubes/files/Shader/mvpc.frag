#version 420
in vec3 normales;
in vec2 texturas;
in vec3 colores;
in vec4 vertexEyeSpace;
in vec3 normalEyeSpace;
out vec3 outputColor;

uniform vec3 lightDir;
uniform vec3 lightSpecular;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;

uniform vec3 MaterialSpecular;
uniform vec3 MaterialAmbient;
uniform vec3 MaterialDiffuse;

uniform vec3 ModelLight;
uniform float shiny;

void main()
{
vec3 viewDirection = normalize(vec3(vec3(0.0, 0.0, -1.7320508) - vertexEyeSpace.xyz));
vec3 L = normalize(lightDir.xyz - vertexEyeSpace.xyz);
vec3 E = normalize(vertexEyeSpace.xyz);
vec3 R = normalize(reflect(L,normalEyeSpace));  
vec3 Iamb = (MaterialAmbient*lightAmbient);
vec3 globalAmbient = ModelLight*MaterialAmbient;
vec3 Ispec;
vec3 Idiff = (MaterialDiffuse*lightDiffuse)*max(dot(normalEyeSpace,-L),0.0);
if(dot(normalEyeSpace, -lightDir.xyz) < 0.0){
      Ispec = vec3(0.0, 0.0, 0.0);
}else{
	Ispec = (MaterialSpecular*lightSpecular)*pow(max(dot(R,viewDirection),0.0),0.3*shiny);
}
outputColor = Ispec+Idiff+ globalAmbient + Iamb;
}
