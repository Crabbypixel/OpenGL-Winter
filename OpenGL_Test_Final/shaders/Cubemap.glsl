#ifdef SHADER_VERTEX

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 matProjection;
uniform mat4 matView;

void main()
{
    TexCoords = aPos;
    vec4 pos = matProjection * matView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  

#endif

#ifdef SHADER_FRAGMENT

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
	FragColor = texture(skybox, TexCoords);
}

#endif