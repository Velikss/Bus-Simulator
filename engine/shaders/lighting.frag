#version 450

//#define DEBUG_NORMALS

const float gamma = 1.9;
const float PI = 3.14159265359;

layout (binding = 1) uniform sampler2D samplerPosition;
layout (binding = 2) uniform sampler2D samplerNormal;
layout (binding = 3) uniform sampler2D samplerAlbedo;
layout (binding = 4) uniform sampler2D samplerMaterial;
layout (binding = 5) uniform sampler2D samplerOverlay;

layout (location = 0) in vec2 inTexCoord;

layout (location = 0) out vec4 FragColor;

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

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);// prevent divide by zero for roughness=0.0 and NdotH=1.0
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------

vec3 HandlePBR(vec3 fragPos)
{
    float metalness = texture(samplerMaterial, inTexCoord).r;
    float roughness = texture(samplerMaterial, inTexCoord).g;

    vec3 normal = normalize(texture(samplerNormal, inTexCoord).xyz);

    vec3 viewPos = ubo.viewPos.xyz;
    vec3 albedo = texture(samplerAlbedo, inTexCoord).rgb;

    vec3 F0 = mix(vec3(0.04), albedo, metalness);

    vec3 Lo = vec3(0);
    for (uint index = 0; index < ubo.lightsCount; index++)
    {
        Light light = ubo.lights[index];
        vec3 lightPos = light.position.xyz;
        float distance = length(lightPos - fragPos);
        float radius = light.colorAndRadius.w * 1.5;

        if (distance <= radius * 1.5)
        {
            vec3 lightColor = light.colorAndRadius.rgb;
            vec3 lightDir = normalize(lightPos - fragPos);
            vec3 viewDir = normalize(viewPos - fragPos);
            vec3 halfwayDir = normalize(lightDir + viewDir);

            // radiance
            float attenuation = clamp(1.0 - distance*distance/(radius*radius), 0.0, 1.0); attenuation *= attenuation;
            vec3 radiance = lightColor * attenuation * 12;

            // Cook-Torrance BRDF
            float NDF = DistributionGGX(normal, halfwayDir, roughness);
            float G   = GeometrySmith(normal, viewDir, lightDir, roughness);
            vec3  F   = fresnelSchlick(clamp(dot(halfwayDir, viewDir), 0.0, 1.0), F0);

            vec3 nominator    = NDF * G * F;
            float denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
            vec3 specular     = nominator / max(denominator, 0.001);// prevent divide by zero for NdotV=0.0 or NdotL=0.0

            // kS is equal to Fresnel
            vec3 kS = F;
            // for energy conservation, the diffuse and specular light can't
            // be above 1.0 (unless the surface emits light); to preserve this
            // relationship the diffuse component (kD) should equal 1.0 - kS.
            vec3 kD = vec3(1.0) - kS;
            // multiply kD by the inverse metalness such that only non-metals
            // have diffuse lighting, or a linear blend if partly metal (pure metals
            // have no diffuse light).
            kD *= 1.0 - metalness;

            // scale light by NdotL
            float NdotL = max(dot(normal, lightDir), 0.0);

            Lo += (kD * albedo / PI + specular) * radiance * NdotL;

            #ifdef DEBUG_NORMALS
            return normal;
            #endif
        }
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = albedo * 0.1;

    vec3 result = ambient + Lo;

    // HDR tonemapping
    result = result / (result + vec3(1.0));
    // gamma correct
    result = pow(result, vec3(1.0 / gamma));

    return result;
}

void main()
{
    // If there is a fragment of the overlay with 100% opacity, render it and skip the lighting calculations
    vec4 overlay = texture(samplerOverlay, inTexCoord);
    if (overlay.a == 1)
    {
        FragColor = overlay;
    }
    else
    {
        vec3 fragPos = texture(samplerPosition, inTexCoord).xyz;

        vec3 color;
        if (fragPos.x > 9999)
        {
            color = texture(samplerAlbedo, inTexCoord).rgb;
        }
        else
        {
            color = HandlePBR(fragPos);
        }

        // If the overlay fragment has an opacity of 0, just render the final color
        if (overlay.a == 0)
        {
            FragColor = vec4(color, 1.0);
        }
        else
        {
            // If the overlay fragment has an opacity between 0 and 100%, mix the overlay color and final color
            FragColor = vec4(mix(color, overlay.rgb, overlay.a), 1.0);
        }
    }
}