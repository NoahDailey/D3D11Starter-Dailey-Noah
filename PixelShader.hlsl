#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

// Identify the constant buffer of external data
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    Light lights[MAX_NUMBER_LIGHTS];
    
    float1 LightsCount;
    float3 AmbientColor; // Ambient color of the pixel
    
    float3 CameraPosition; // Current position of the camera
    float1 padding;
    
    float3 ColorTint; // A tint color of a pixel
    float1 morepadding;
    
    float2 UVScale;
    float2 UVOffset;
}

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
Texture2D NormalTexture : register(t1);
SamplerState BasicSampler : register(s0); // "s" registers for samplers

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
    // Determine the proper normal and tangent values
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
    
    // Set the uv scale and offset appropriately for textures
    input.uv = input.uv * UVScale + UVOffset;
    
    // Determine the surface and texture color of the pixel
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= ColorTint;
    
    // Determine how to get the normal map
    float3 unpackedNormals = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;
    float3 bitangent = cross(input.tangent, input.normal);
    
    float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);
    float3 finalNormals = mul(unpackedNormals, TBN);
    
    // Lighting calculations
    // Value to be used as the final color applied to the material
    float3 finalColor = float3(0, 0, 0);
    
    // Ambiant term
    float3 ambientTerm = AmbientColor * surfaceColor;
    
    // A specular scaling factor might come from a texture (just the red channel)
    float specScale = SurfaceTexture.Sample(BasicSampler, input.uv).r;
    
    for (int i = 0; i < LightsCount; i++)
    {
        if (lights[i].Type == 0)
        {
            finalColor += DirectionalLight(lights[i], finalNormals, surfaceColor, CameraPosition,
                input.worldPosiiton, ambientTerm, specScale);
        }
        
        if (lights[i].Type == 1)
        {
            finalColor += PointLight(lights[i], finalNormals, surfaceColor, CameraPosition,
                input.worldPosiiton, ambientTerm, specScale);
        }
        
        if (lights[i].Type == 2)
        {
            finalColor += SpotLight(lights[i], finalNormals, surfaceColor, CameraPosition,
                input.worldPosiiton, ambientTerm, specScale);

        }
    }
    return float4(finalColor, 1);
}