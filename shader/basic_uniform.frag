#version 460

//in variables relating to vertexes from vertex shader
in vec3 Position;
in vec3 Normal;

//texture variables
in vec2 TexCoord;
layout (binding = 0) uniform sampler2D Tex1;
layout (binding = 1) uniform sampler2D Tex2;

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

//phong shading method
vec3 phongModel(vec3 vertPosition, vec3 vertNormal){
    
    //texture setting
  vec3 metalTexColor = texture(Tex1, TexCoord).rgb;
  vec4 mossTexColor = texture(Tex2, TexCoord);
  vec3 texColor = mix(metalTexColor.rgb, mossTexColor.rgb, mossTexColor.a);
    
    //  //calculate light direction
  vec3 s = normalize(vec3(Light.Position) - vertPosition);

  //calculate dot product for vector s and vertex normal
  float sDotN = max( dot(s,vertNormal), 0.0 );

  //calculate view direction
  vec3 viewDir = normalize(viewPos - vertPosition);

  //calculate reflection direction
  vec3 reflectDir = reflect(-s, vertNormal);

  //calculate specular
  vec3 spec = Material.Ks * pow(max(dot(viewDir, reflectDir), 0.0), 32) * Light.Ls;

  //calculate ambient
  vec3 ambient = Material.Ka * Light.La * texColor;

  //difuse formula for light calculations
  vec3 diffuse = Light.Ld * Material.Kd * sDotN * texColor;

    return ambient + diffuse + spec;
}

//blinn-phong shading method
vec3 blinnPhongModel(vec3 vertPosition, vec3 vertNormal){

    //texture setting
  vec3 metalTexColor = texture(Tex1, TexCoord).rgb;
  vec4 mossTexColor = texture(Tex2, TexCoord);
  vec3 texColor = mix(metalTexColor.rgb, mossTexColor.rgb, mossTexColor.a);
    
    //  //calculate light direction
  vec3 s = normalize(vec3(Light.Position) - vertPosition);

  //calculate dot product for vector s and vertex normal
  float sDotN = max( dot(s,vertNormal), 0.0 );

  //calculate view direction
  vec3 viewDir = normalize(viewPos - vertPosition);

  //calculate half vector
  vec3 v = normalize(-vertPosition.xyz);
  vec3 h = normalize(v + s);

  //calculate specular
  vec3 spec = Material.Ks * pow(max(dot(h, vertNormal), 0.0), 32) * Light.Ls;

  //calculate ambient
  vec3 ambient = Material.Ka * Light.La * texColor;

  //difuse formula for light calculations
  vec3 diffuse = Light.Ld * Material.Kd * sDotN * texColor;

    return ambient + diffuse + spec;
}

//toon shading method
vec3 toonShading(vec3 vertPosition, vec3 vertNormal){
  
  //texture setting
  vec3 metalTexColor = texture(Tex1, TexCoord).rgb;
  vec4 mossTexColor = texture(Tex2, TexCoord);
  vec3 texColor = mix(metalTexColor.rgb, mossTexColor.rgb, mossTexColor.a);

    //  //calculate light direction
  vec3 s = normalize(vec3(Light.Position) - vertPosition);

  //calculate dot product for vector s and vertex normal
  float sDotN = max( dot(s,vertNormal), 0.0 );

  //calculate view direction
  vec3 viewDir = normalize(viewPos - vertPosition);

  //calculate half vector
  vec3 v = normalize(-vertPosition.xyz);
  vec3 h = normalize(v + s);

  //calculate ambient
  vec3 ambient = Material.Ka * Light.La * texColor;

  //difuse formula for light calculations
  vec3 diffuse = Light.Ld * Material.Kd * floor(sDotN * levels) * scaleFactor * texColor;

    return ambient + diffuse;
}


void main()
{
    //select shader to use based on given shader ID

    if(shaderID == 1){
    FragColor = vec4(phongModel(Position, Normal), 1.0);
    }else if(shaderID == 2){
    FragColor = vec4(blinnPhongModel(Position, Normal), 1.0);
    }else{
    FragColor = vec4(toonShading(Position, Normal), 1.0);
    }
}
