#ifdef SHADER_VERTEX
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
	vs_out.FragPos = aPos;
	vs_out.Normal = aNormal;
	vs_out.TexCoords = aTexCoords;
	gl_Position = matProjection * matView * matModel * vec4(aPos, 1.0f);
}

#endif

#ifdef SHADER_FRAGMENT
out vec4 FragColor;
in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform sampler2D floorTexture;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool blinn;

void main()
{
	vec3 color = texture(floorTexture, fs_in.TexCoords).rgb;

	// Ambient
	vec3 ambient = 0.05f * color;

	// Diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * color;

	// Specular (Phong & Blinn Phong)
	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = 0.0f;

	if(blinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0f), 32.0f);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
	}

	vec3 specular = vec3(0.3f) * spec;			// Assuming bright white color light
	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}

#endif