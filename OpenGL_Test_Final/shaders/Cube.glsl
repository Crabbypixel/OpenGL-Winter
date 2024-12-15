#ifdef SHADER_VERTEX

layout (location = 0) in vec3 aPos;

uniform mat4 matModel;
layout (std140)	uniform Matrices
{
	uniform mat4 matProjection;
	uniform mat4 matView;
};

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0f);
}

#endif

#ifdef SHADER_FRAGMENT

out vec4 FragColor;
uniform vec3 lightColor;

void main()
{
	FragColor = vec4(lightColor, 1.0f);
}

#endif