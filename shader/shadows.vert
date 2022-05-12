#version 430

//in variables, these are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoord;

//out variables for the fragment shader
out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;
out vec4 ShadowCoord;

//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 MVP;				//model view projection matrix
uniform mat4 ProjectionMatrix;	//Projection matrix
uniform mat4 ShadowMatrix;		//shadow matrix
 
void main() 
{ 
  //pass normal vector for vertex to fragment shader
  Normal = normalize(NormalMatrix * VertexNormal);

  //pass vertex position in view coordinates to fragment shader
  Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

  //pass texture coordinate on model to fragment shader
  TexCoord = VertexTexCoord;

  //pass shadow coordinate from shadow matrix
  ShadowCoord = ShadowMatrix * vec4(VertexPosition, 1.0);

  gl_Position = MVP * vec4(VertexPosition,1.0); 
} 