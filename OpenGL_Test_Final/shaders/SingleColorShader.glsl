#ifdef SHADER_VERTEX

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0f);
}

#endif

#ifdef SHADER_FRAGMENT

out vec4 FragColor;

void main()
{
	FragColor = vec4(0.04f, 0.28f, 0.26f, 1.0f);
}

#endif