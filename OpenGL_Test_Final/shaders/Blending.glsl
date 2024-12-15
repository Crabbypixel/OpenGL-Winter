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
in vec2 TexCoords;
uniform sampler2D texture1;

void main()
{
	vec4 texColor = texture(texture1, TexCoords);
	FragColor = texColor;
}

#endif