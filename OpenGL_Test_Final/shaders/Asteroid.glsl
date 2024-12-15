#ifdef SHADER_VERTEX
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;
out vec2 TexCoords;

uniform mat4 matProjection;
uniform mat4 matView;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = matProjection * matView * instanceMatrix * vec4(aPos, 1.0f);
}

#endif

#ifdef SHADER_FRAGMENT
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, TexCoords);
}

#endif