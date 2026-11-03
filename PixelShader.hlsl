#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

// Identify the constant buffer of external data
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    float2 UVScale;
    float2 UVOffset;
    
    float1 TotalTime;       // The total time that has passed
    float3 CameraPosition;  // Current position of the camera
    
    Light DirectionalLight1;
    
    float3 ColorTint;       // A tint color of a pixel
    float1 padding;
    float3 AmbientColor;    // Ambient color of the pixel
}

Texture2D SurfaceTexture  : register(t0); // "t" registers for textures
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
    // Determine the proper normal values
    input.normal = normalize(input.normal);
    
    // Set the uv scale and offset appropriately for textures
    input.uv = input.uv * UVScale + UVOffset;
    
    // Determine the surface and texture color of the pixel
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= ColorTint;
    
    // Lighting calculations
    // Value to be used as the final color applied to the material
    float3 finalColor = float3(0, 0, 0);
    
    // Ambiant term
    float3 ambientTerm = AmbientColor * surfaceColor;
    
    // Diffuse value
    float diffuseIntensity = saturate(dot(input.normal, normalize(-DirectionalLight1.Direction)));
    float3 diffuseTerm = diffuseIntensity * DirectionalLight1.Color * DirectionalLight1.Intensity * surfaceColor;
    
    // Build the final color
    finalColor = ambientTerm + diffuseTerm;
   
    return float4(finalColor, 1);
}