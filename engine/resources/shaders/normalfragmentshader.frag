#version 430 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 Normal;
out vec4 FragColor;

uniform sampler2D otexture;

uniform vec3 objectColor;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float radius;
};

#define NR_POINT_LIGHTS 29
uniform Light lights[NR_POINT_LIGHTS];
uniform int lightsSize;
uniform vec3 viewPos;
uniform bool useTexture;
uniform bool useColor;
uniform bool useLighting;

void main()
{
	vec3 endColor;
	if (useTexture && useColor)
		endColor = texture(otexture, TexCoords).rgb *objectColor;
	else if (useTexture)
		endColor = texture(otexture, TexCoords).rgb;
	else if (useColor)
		endColor = objectColor;

	if (useLighting)
	{
		vec3 result;
		for(int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			float ambientStrength = 0.1;
			vec3 ambient = ambientStrength * lights[i].ambient;
			vec3 norm = normalize(Normal);
			vec3 lightDir = normalize(lights[i].position - FragPos);  
			float diff = max(dot(norm, lightDir), 0.0);
			vec3 diffuse = diff * lights[i].ambient;

			float specularStrength = 0.2;
			vec3 viewDir = normalize(viewPos - FragPos);
			vec3 reflectDir = reflect(-lightDir, norm);  
			float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
			vec3 specular = specularStrength * spec * lights[i].ambient;
			float distance    = length(lights[i].position - FragPos);
			float attenuation = clamp(1.0 - distance*distance/(lights[i].radius*lights[i].radius), 0.0, 1.0); attenuation *= attenuation;
			ambient *= attenuation;
			diffuse  *= attenuation;
			specular *= attenuation;
			vec3 calcLight = (ambient + diffuse + specular);
			if(calcLight.x > 0)
				result += calcLight;
		}
		FragColor = vec4(result * endColor, 1.0);
	}
	else
		FragColor = vec4(endColor, 1.0);
}
