#version 430

//position and intensity of light source
struct LightInfo{
	vec4 Position;
	vec3 Intensity;
};

uniform LightInfo Light;

//reflectivity of different lights from object
struct MaterialInfo{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;

};
uniform MaterialInfo Material;

//color and width of wireframe lines
uniform struct LineInfo{
	float Width;
	vec4 Color;
} Line;

//geometry positions, normals and edge distance
in vec3 GPosition;
in vec3 GNormal;
noperspective in vec3 GEdgeDistance;

//output colour
layout (location = 0) out vec4 FragColor;

//phong shading method
vec3 phongModel(vec3 vertPosition, vec3 vertNormal){
    
    //  //calculate light direction
  vec3 s = normalize(vec3(Light.Position) - vertPosition);

  //calculate dot product for vector s and vertex normal
  float sDotN = max( dot(s,vertNormal), 0.0 );

  //calculate view direction
  vec3 viewDir = normalize(GEdgeDistance - vertPosition);

  //calculate reflection direction
  vec3 reflectDir = reflect(-s, vertNormal);

  //calculate specular
  vec3 spec = Material.Ks * pow(max(dot(viewDir, reflectDir), 0.0), 32) * Light.Intensity;

  //calculate ambient
  vec3 ambient = Material.Ka * Light.Intensity;

  //difuse formula for light calculations
  vec3 diffuse = Light.Intensity * Material.Kd * sDotN;

    return ambient + diffuse + spec;
}

void main(){

	//get colour of object with lighting
	vec4 color = vec4(phongModel(GPosition, GNormal), 1.0);

	//check if fragment is near edge of triangle calculated in geometry shader, colour as line colour if near enough to edge
	float d = min(GEdgeDistance.x, GEdgeDistance.y);
	d = min(d, GEdgeDistance.z);

	float mixVal;
	if(d < Line.Width - 1)
	{
		mixVal = 1.0;
	}
	else if(d > Line.Width + 1)
	{
		mixVal = 0.0;
	}
	else
	{
		float x = d - (Line.Width - 1);
		mixVal = exp2(-2.0 * (x*x));
	}

	//output colour
	FragColor = mix( color, Line.Color, mixVal);
}