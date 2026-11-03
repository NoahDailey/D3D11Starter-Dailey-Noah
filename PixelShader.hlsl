#include "ShaderInclude.hlsli"

// Identify the constant buffer of external data
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    float4 ColorTint;       // A tint color of a pixel
    float2 UVScale;
    float2 UVOffset;
    float1 TotalTime;       // The total time that has passed
    float3 CameraPosition;  // Current position of the camera
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
    input.uv = input.uv * UVScale + UVOffset;
    
    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    float3 textureColor = surfaceColor * (float3) ColorTint;
    
    float3 ambientTerm = AmbientColor * textureColor * (float3) ColorTint;
   
    return float4(ambientTerm, 1);
}