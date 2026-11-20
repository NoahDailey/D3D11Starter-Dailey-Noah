#ifndef __GGP_SHADER_LIGHTING__
#define __GGP_SHADER_LIGHTING__

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

#define SHININESS                   256
#define MAX_NUMBER_LIGHTS           6

#define MIN_ROUGHNESS               0.0000001f
#define PI                          3.14159265f

struct Light
{
    int Type;
    float3 Direction;
    float Range;
    float3 Position;
    float Intensity;
    float3 Color;
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding;
};

// Create functions that we can use for our light calculations
float Diffuse(float3 normal, float3 lightDirection)
{
    return saturate(dot(normal, -lightDirection));
}

float3 Specular(float scale, float3 camDirection, float3 lightDirection, float3 normal, float3 color, float intensity)
{
    float3 reflection = reflect(lightDirection, normal);
    float RdotV = saturate(dot(reflection, camDirection));
    
    float3 specular = pow(RdotV, SHININESS) * scale * color * intensity;
    
    return specular;
}

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

/* LIGTH SPECIFIC CALCULATION METHODS */
float3 DirectionalLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float specScale)
{
    float3 lightDirection = normalize(light.Direction);
    float3 camDirection = normalize(cameraPosition - worldPosition);
    
    float3 diffuseTerm = Diffuse(normal, lightDirection) * light.Color * light.Intensity * surfaceColor;
    
    float3 specularTerm = Specular(specScale, camDirection, lightDirection,
        normal, light.Color, light.Intensity) * surfaceColor;
    
    // Cut the specular if the diffuse contribution is zero
    // - any() returns 1 if any component of the param is non-zero
    // - In other words:
    // - If the diffuse amount is 0, any(diffuse) returns 0
    // - If the diffuse amount is != 0, any(diffuse) returns 1
    // - So when diffuse is 0, specular becomes 0
    specularTerm *= any(diffuseTerm);
    
    return diffuseTerm + specularTerm;
}

float3 PointLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float specScale)
{
    float3 lightDirection = normalize(worldPosition - light.Position);
    float3 camDirection = normalize(cameraPosition - worldPosition);
    
    float3 diffuseTerm = Diffuse(normal, lightDirection) * light.Color * light.Intensity * surfaceColor;
    
    float3 specularTerm = Specular(specScale, camDirection, lightDirection,
        normal, light.Color, light.Intensity) * surfaceColor;
    
    // Cut the specular if the diffuse contribution is zero
    // - any() returns 1 if any component of the param is non-zero
    // - In other words:
    // - If the diffuse amount is 0, any(diffuse) returns 0
    // - If the diffuse amount is != 0, any(diffuse) returns 1
    // - So when diffuse is 0, specular becomes 0
    specularTerm *= any(diffuseTerm);
    
    return (diffuseTerm + specularTerm) * Attenuate(light, worldPosition);
}

float3 SpotLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float specScale)
{
    float3 pointLight = PointLight(light, normal, surfaceColor, cameraPosition, worldPosition, specScale);
    
    float3 lightToPixel = normalize(worldPosition - light.Position);
    float pixelAngle = saturate(dot(lightToPixel, light.Direction));
    
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return pointLight * spotTerm;
}


/* PBR LIGHTING FUNCTIONS */
float D_GGX(float3 n, float3 h, float roughness)
{
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness; // Remap roughness
    float a2 = max(a * a, MIN_ROUGHNESS); // MIN_ROUGHNESS is 0.0000001
    
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    
    // Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}

float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    
    // Final Value
    return 1 / (NdotV * (1 - k) + k);
}

float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    float VdotH = saturate(dot(v, h));
    
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}

float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0)
{
    float3 h = normalize(v + l);
    
    float3 D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float3 G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);

    float3 specularResult = (D * F * G) / 4;
    
    return specularResult * saturate(dot(n, l));
}

float3 DirLightPBR(Light light, float3 normal, float3 cameraPos, float3 worldPos, float3 surfaceColor, float3 specColor, float roughness, float metalness)
{
    float3 lightDirection = normalize(light.Direction);
    float3 camDirection = normalize(cameraPos - worldPos);
    
    float3 h = normalize(camDirection + lightDirection);
    
    float diffuse = Diffuse(normal, lightDirection);
    float3 spec = MicrofacetBRDF(normal, lightDirection, camDirection, roughness, specColor);
    
    float3 F = F_Schlick(camDirection, h, specColor);
    float3 balanceDiffuse = DiffuseEnergyConserve(diffuse, F, metalness);
    
    float3 total = (balanceDiffuse * surfaceColor + spec) * light.Intensity * light.Color;
    return total;
}

float3 PointLightPBR(Light light, float3 normal, float3 cameraPos, float3 worldPos, float3 surfaceColor, float3 specColor, float roughness, float metalness)
{
    float3 lightDirection = normalize(worldPos - light.Position);
    float3 camDirection = normalize(cameraPos - worldPos);
    
    float3 h = normalize(camDirection + lightDirection);
    
    float diffuse = Diffuse(normal, lightDirection);
    float3 spec = MicrofacetBRDF(normal, lightDirection, camDirection, roughness, specColor);
    
    float3 F = F_Schlick(camDirection, h, specColor);
    float3 balanceDiffuse = DiffuseEnergyConserve(diffuse, F, metalness);
    float attenuate = Attenuate(light, worldPos);
    
    return (balanceDiffuse * surfaceColor + spec) * attenuate * light.Intensity * light.Color;
}

float3 SpotLightPBR(Light light, float3 normal, float3 cameraPos, float3 worldPos, float3 surfaceColor, float3 specColor, float roughness, float metalness)
{
    float3 pointLight = PointLightPBR(light, normal, cameraPos, worldPos, surfaceColor, specColor, roughness, metalness);
    
    float3 lightToPixel = normalize(light.Position - worldPos);
    float pixelAngle = saturate(dot(lightToPixel, light.Direction));
    
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return pointLight * spotTerm;
}

#endif