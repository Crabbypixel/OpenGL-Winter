#ifdef SHADER_VERTEX
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fColor;
uniform vec2 offsets[100];

void main()
{
	// gl_InstanceID will increment after each vertex shader call
	// Therefore, different indexes are reference for different vertex calls

	vec2 offset = offsets[gl_InstanceID];
	gl_Position = vec4(aPos + offset, 0.0f, 1.0f);
	fColor = aColor;
}

#endif

#ifdef SHADER_FRAGMENT
out vec4 FragColor;
in vec3 fColor;

void main()
{
	FragColor = vec4(fColor, 1.0f);
}

#endif