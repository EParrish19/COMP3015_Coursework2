#version 430

//input geometry information as traingles and output as triangle strips
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//output geometry information from vertices
out vec3 GNormal;
out vec3 GPosition;

//distance from edges without considering viewpoint
noperspective out vec3 GEdgeDistance;

//arrays of vertex information
in vec3 VNormal[];
in vec3 VPosition[];

//viewport matrix
uniform mat4 ViewportMatrix;


void main()
{
	
	//calculate each point on a triangles input
	vec2 p0 = vec2(ViewportMatrix *(gl_in[0].gl_Position / gl_in[0].gl_Position.w));
	vec2 p1 = vec2(ViewportMatrix *(gl_in[1].gl_Position / gl_in[1].gl_Position.w));
	vec2 p2 = vec2(ViewportMatrix *(gl_in[2].gl_Position / gl_in[2].gl_Position.w));

	//claculate length of triangle sides
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);

	//calculate angles of the triangle
	float alpha = acos((b*b + c*c - a*a) / (2.0*b*c));
	float beta = acos((a*a + c*c - b*b) / (2.0*a*c));


	float ha = abs(c * sin(beta));
	float hb = abs(c * sin(alpha));
	float hc = abs(b * sin(alpha));

	GEdgeDistance = vec3(ha, 0, 0);

	//emit calculated vertices as part of triangle
	GNormal = VNormal[0];
	GPosition = VPosition[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	GEdgeDistance = vec3(0, hb, 0);

	GNormal = VNormal[1];
	GPosition = VPosition[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	GEdgeDistance = vec3(0, 0, hc);

	GNormal = VNormal[2];
	GPosition = VPosition[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	//complete triangle and send info to fragment shader
	EndPrimitive();

}