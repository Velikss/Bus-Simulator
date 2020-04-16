#version 430 core

layout(location = 0) in vec3 FragPos;
layout(location = 2) in vec3 Normal;
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float radius;
};
  
uniform vec3 viewPos;
uniform Material material;

#define NR_POINT_LIGHTS 29
uniform Light lights[NR_POINT_LIGHTS];
uniform int lightsSize;

void main()
{
	vec3 result = vec3(0, 0, 0);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
        // ambient
        vec3 ambient = lights[i].ambient * material.ambient;
  	    
        // diffuse 
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].diffuse * (diff * material.diffuse);
        
        // specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lights[i].specular * (spec * material.specular);
            
		float distance    = length(lights[i].position - FragPos);
		float attenuation = clamp(1.0 - distance*distance/(lights[i].radius*lights[i].radius), 0.0, 1.0); attenuation *= attenuation;
		ambient *= attenuation;
		diffuse  *= attenuation;
		specular *= attenuation;
		vec3 calcLight = (ambient + diffuse + specular);
		if(calcLight.x > 0)
			result += calcLight;
    }
    FragColor = vec4(result, 1.0);
} 