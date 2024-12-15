#ifdef SHADER_VERTEX

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
	mat4 matProjection;
	mat4 matView;
};

layout (std140) uniform FloatValue
{
	float myFloat;
};

uniform mat4 matModel;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0f);
	gl_PointSize = myFloat;
}

#endif


#ifdef SHADER_FRAGMENT

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

#endif