#ifdef SHADER_VERTEX

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vTexCoords;

// Passed to geometry shader (if geometry shader was not present, the "out" variables would directly go to the fragment shader)
out vec3 vFragPos;
out vec3 vNormalOrg;

out VS_OUT {
	vec2 texCoords;
} gs_out;

uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;

void main()
{
	gl_Position = matProjection * matView * matModel * vec4(vPos, 1.0f);
	vNormalOrg = mat3(transpose(inverse(matModel))) * vNorm;
	vFragPos = vec3(matModel * vec4(vPos, 1.0f));
	gs_out.texCoords = vTexCoords;
}

#endif

#ifdef SHADER_GEOMETRY

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vFragPos[];
in vec3 vNormalOrg[];

in VS_OUT {
	vec2 texCoords;
} gs_in[];

// Passed to fragment shader
out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform float fTime;

vec4 explode(vec4 position, vec3 normal);
vec3 getNormal();

void main()
{
	FragPos = vFragPos[0];

	vec3 normal = getNormal();
	Normal = vNormalOrg[0];

	gl_Position = explode(gl_in[0].gl_Position, normal);
	TexCoords = gs_in[0].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[1].gl_Position, normal);
	TexCoords = gs_in[1].texCoords;
	EmitVertex();
	gl_Position = explode(gl_in[2].gl_Position, normal);
	TexCoords = gs_in[2].texCoords;
	EmitVertex();

	EndPrimitive();
}

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float fMagnitude = 1.0f;
	vec3 direction = vec3(0.0f, 0.0f, 0.0f);
    //direction = normal * ((sin(fTime) + 1.0f) / 2.0f) * fMagnitude;
	return position + vec4(direction, 0.0f);
}

#endif

#ifdef SHADER_FRAGMENT

struct PointLight
{
	vec3 vPosition;
	vec3 vLightColor;

	float fConstant;
	float fLinear;
	float fQuadratic;

	vec3 vAmbient;
	vec3 vDiffuse;
	vec3 vSpecular;
};

struct SpotLight
{
	vec3 vPosition;
	vec3 vDirection;
	vec3 vLightColor;

	vec3 vAmbient;
	vec3 vDiffuse;
	vec3 vSpecular;

	float fCutOff;
	float fOuterCutOff;

	float fConstant;
	float fLinear;
	float fQuadratic;
};

#define NR_POINT_LIGHTS 1

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 vViewPos;

uniform PointLight pointlights[NR_POINT_LIGHTS];
uniform SpotLight spotlight;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

vec3 CalcPointLight(PointLight light, vec3 vNormal, vec3 vFragPos, vec3 vViewDir);
vec3 CalcSpotLight(SpotLight light, vec3 vNormal, vec3 vFragPos, vec3 vViewDir);

void main()
{
	vec3 vNorm = normalize(Normal);
	vec3 vViewDir = normalize(vViewPos - FragPos);

	vec3 vResult = vec3(0.0f, 0.0f, 0.0f);

	// Point lights
	vResult += CalcPointLight(pointlights[0], vNorm, FragPos, vViewDir);

	// Spot light
	//vResult += CalcSpotLight(spotlight, vNorm, vFragPos, vViewDir);

	FragColor = vec4(vResult, 1.0f);
}


vec3 CalcPointLight(PointLight light, vec3 vNormal, vec3 vFragPos, vec3 vViewDir)
{
	vec3 vLightDir = normalize(light.vPosition - vFragPos);

	// Ambient shading
	vec3 vAmbient = light.vAmbient * light.vLightColor * texture(texture_diffuse1, TexCoords).rgb;

	// Diffuse shading
	float fDiff = max(dot(vNormal, vLightDir), 0.0f);
	vec3 vDiffuse = light.vDiffuse * light.vLightColor * fDiff * texture(texture_diffuse1, TexCoords).rgb;

	// Specular shading
	vec3 vReflectDir = reflect(-vLightDir, vNormal);
	float fSpec = pow(max(dot(vViewDir, vReflectDir), 0.0f), 128);
	vec3 vSpecular = light.vSpecular * light.vLightColor * fSpec * texture(texture_specular1, TexCoords).rgb;

	// Attenuation
	//float fDistance = length(light.vPosition - vFragPos);
	//float fAttenuation = 1.0f / (light.fConstant + light.fLinear * fDistance + light.fQuadratic * fDistance * fDistance);

	float fAttenuation = 1.0f;

	// We'll leave out attenuating the ambient shading
	vDiffuse = vDiffuse * fAttenuation;
	vSpecular = vSpecular * fAttenuation;

	// Return the combined shading
	return (vAmbient + vDiffuse + vSpecular);
}

vec3 CalcSpotLight(SpotLight light, vec3 vNormal, vec3 vFragPos, vec3 vViewDir)
{
	vec3 vLightDir = normalize(light.vPosition - vFragPos);

	// Ambient shading
	vec3 vAmbient = light.vAmbient * light.vLightColor * texture(texture_diffuse1, TexCoords).rgb;

	// Diffuse shading
	float fDiff = max(dot(vLightDir, vNormal), 0.0f);
	vec3 vDiffuse = light.vDiffuse * light.vLightColor * fDiff * texture(texture_diffuse1, TexCoords).rgb;

	// Specular shading
	vec3 vReflectDir = reflect(-vLightDir, vNormal);
	float fSpec = pow(max(dot(vViewDir, vReflectDir), 0.0f), 128);
	vec3 vSpecular = light.vSpecular * light.vLightColor * fSpec * texture(texture_specular1, TexCoords).rgb;

	// SpotLight
	float fTheta = dot(vLightDir, normalize(-light.vDirection));
	float fEpsilion = light.fCutOff - light.fOuterCutOff;
	float fIntensity = clamp((fTheta - light.fOuterCutOff) / fEpsilion, 0.0f, 1.0f);
	vDiffuse = vDiffuse * fIntensity;
	vSpecular = vSpecular * fIntensity;

	// Attenuation
	float fDistance = length(light.vPosition - vFragPos);
	float fAttenuation = 1.0f / (light.fConstant + light.fLinear * fDistance + light.fQuadratic * (fDistance * fDistance));
	
	vAmbient = vAmbient * fAttenuation;
	vDiffuse = vDiffuse * fAttenuation;
	vSpecular = vSpecular * fAttenuation;

	return (vAmbient + vDiffuse + vSpecular);
}

#endif