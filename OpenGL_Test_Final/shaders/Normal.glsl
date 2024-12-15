#ifdef SHADER_VERTEX
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
	vec3 normal;
} vs_out;

uniform mat4 matView;
uniform mat4 matModel;

void main()
{
	gl_Position = matView * matModel * vec4(aPos, 1.0f);
	mat3 normalMatrix = mat3(transpose(inverse(matView * matModel)));
	vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0f));
	//vs_out.normal = normalize(vec3(matProjection * vec4(normalMatrix * aNormal, 1.0)));
}

#endif

#ifdef SHADER_GEOMETRY
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
	vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.1f;

uniform mat4 matProjection;

void GenerateLine(int index)
{
	gl_Position = matProjection * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = matProjection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0f) * MAGNITUDE);

    EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0);		// first vertex normal
	GenerateLine(1);		// second vertex normal
	GenerateLine(2);		// third vertex normal
}

#endif

#ifdef SHADER_FRAGMENT
out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
#endif