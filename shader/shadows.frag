#version 430

//in variables relating to vertexes from vertex shader
in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;

//shadow map
uniform sampler2DShadow ShadowMap;

//texture variables
in vec2 TexCoord;
layout (binding = 1) uniform sampler2D Tex1;
layout (binding = 2) uniform sampler2D Tex2;

 //light information struct
uniform struct LightInfo 
{
  vec4 Position; // Light position in eye coords.
  vec3 Ld;       // Diffuse light intensity
  vec3 La;       // Ambient Intensity
  vec3 Ls;       // Specular Intensity
} Light;

////material information struct
uniform struct MaterialInfo 
{
  vec3 Kd;      // Diffuse reflectivity
  vec3 Ka;      // Ambient Reflectivity
  vec3 Ks;      // Specular Reflectivity
} Material;

//view position
uniform vec3 viewPos;

//ID of shader to use
uniform int shaderID;

//toon shading variables
const int levels = 4;
const float scaleFactor = 1.0 / levels;

//out variable to send colour to fragment
layout (location = 0) out vec4 FragColor;

//phong shading method with shadows
vec3 phongModelWithShadows(){
    
    //texture setting
  vec3 metalTexColor = texture(Tex1, TexCoord).rgb;
  vec4 mossTexColor = texture(Tex2, TexCoord);
  vec3 texColor = mix(metalTexColor.rgb, mossTexColor.rgb, mossTexColor.a);
    
    vec3 n = Normal;

    vec3 s = normalize(vec3(Light.Position) - Position);

    vec3 v = normalize(-Position.xyz);

    vec3 r = reflect(-s, n);

    float sDotN = max(dot(s,n), 0.0);

    vec3 diffuse = Light.Ld * Material.Kd * sDotN * texColor;

    vec3 spec = vec3(0.0);

    if(sDotN > 0.0){
    
    spec = Light.Ls * Material.Ks * pow(max(dot(r, v), 0.0), 32) * texColor;

    return diffuse + spec;

    }
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow(){

vec3 ambient = Light.La * Material.Ka;
vec3 diffAndSpec = phongModelWithShadows();

float shadow = 1.0;

if(ShadowCoord.z >= 0){

shadow = textureProj(ShadowMap, ShadowCoord);

}

FragColor = vec4(diffAndSpec * shadow + ambient, 1.0);

FragColor = pow(FragColor, vec4(1.0 / 2.2));



}

subroutine (RenderPassType)
void RecordDepth()
{

 //depth written automatically

}

void main()
{

    RenderPass();

}