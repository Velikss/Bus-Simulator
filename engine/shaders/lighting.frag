#version 450

layout (binding = 1) uniform sampler2D samplerposition;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerAlbedo;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 outFragColor;

struct Light {
    vec4 position;
    vec4 colorAndRadius;
};

layout (binding = 0) readonly buffer Buffer
{
    vec4 viewPos;
    float ambientLight;
    uint lightsCount;
    Light lights[];
} ubo;


void main()
{
    // Get G-Buffer values
    vec3 fragPos = texture(samplerposition, inTexCoord).rgb;
    vec4 albedo = texture(samplerAlbedo, inTexCoord);

    if (fragPos.x > 9999)
    {
        outFragColor = albedo;
    }
    else
    {
        vec3 normal = texture(samplerNormal, inTexCoord).rgb;

        // Ambient part
        vec3 fragcolor  = albedo.rgb * ubo.ambientLight;

        for (int i = 0; i < ubo.lightsCount; ++i)
        {
            float radius = ubo.lights[i].colorAndRadius.w;
            if (radius > 0)
            {
                vec3 lightColor = ubo.lights[i].colorAndRadius.rgb;

                // Vector to light
                vec3 L = ubo.lights[i].position.xyz - fragPos;
                // Distance from light to fragment position
                float dist = length(L);

                // Viewer to fragment
                vec3 V = ubo.viewPos.xyz - fragPos;
                V = normalize(V);

                if (dist < radius + 5)
                {
                    // Light to fragment
                    L = normalize(L);

                    // Attenuation
                    float atten = radius / (pow(dist, 2.0) + 1.0);

                    // Diffuse part
                    vec3 N = normalize(normal);
                    float NdotL = max(0.0, dot(N, L));
                    vec3 diff = lightColor * albedo.rgb * NdotL * atten;

                    // Specular part
                    // Specular map values are stored in alpha of albedo mrt
                    vec3 R = reflect(-L, N);
                    float NdotR = max(0.0, dot(R, V));
                    vec3 spec = lightColor * albedo.a * pow(NdotR, 16.0) * atten;

                    fragcolor += diff;
                }
            }
        }

        outFragColor = vec4(fragcolor, 1.0);
    }
}