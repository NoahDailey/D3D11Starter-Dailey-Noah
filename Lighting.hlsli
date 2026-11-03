#ifndef __GGP_SHADER_LIGHTING__
#define __GGP_SHADER_LIGHTING__

#define LIGHT_TYPE_DIRECTIONAL		0
#define LIGHT_TYPE_POINT			1
#define LIGHT_TYPE_SPOT				2

#define SHININESS                   256
#define MAX_NUMBER_LIGHTS           6

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
float3 DirectionalLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float3 ambient, float specScale)
{
    float3 lightDirection = normalize(light.Direction);
    float3 camDirection = normalize(cameraPosition - worldPosition);
    
    float3 diffuseTerm = Diffuse(normal, lightDirection) * light.Color * light.Intensity * surfaceColor;
    
    float3 specularTerm = Specular(specScale, camDirection, lightDirection,
        normal, light.Color, light.Intensity) * surfaceColor;
    
    return ambient + diffuseTerm + specularTerm;
}

float3 PointLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float3 ambient, float specScale)
{
    float3 lightDirection = normalize(worldPosition - light.Position);
    float3 camDirection = normalize(cameraPosition - worldPosition);
    
    float3 diffuseTerm = Diffuse(normal, lightDirection) * light.Color * light.Intensity * surfaceColor;
    
    float3 specularTerm = Specular(specScale, camDirection, lightDirection,
        normal, light.Color, light.Intensity) * surfaceColor;
    
    return (ambient + diffuseTerm + specularTerm) * Attenuate(light, worldPosition);
}

float3 SpotLight(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 worldPosition, float3 ambient, float specScale)
{
    float3 pointLight = PointLight(light, normal, surfaceColor, cameraPosition, worldPosition, ambient, specScale);
    
    float3 lightToPixel = normalize(worldPosition - light.Position);
    float pixelAngle = saturate(dot(lightToPixel, light.Direction));
    
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return pointLight * spotTerm;
}

#endif