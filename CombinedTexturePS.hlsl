#include "ShaderInclude.hlsli"

// Identify the constant buffer of externaldata
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    float4 ColorTint; // A tint color of a pixel
    float2 UVScale;
    float2 UVOffset;
    float1 TotalTime;
}

Texture2D SurfaceTexture  : register(t0); // "t" registers for textures
Texture2D OverlayTexture  : register(t1); // "t" registers for textures
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
    float4 surfaceColor = float4(SurfaceTexture.Sample(BasicSampler, input.uv).rgb * OverlayTexture.Sample(BasicSampler, input.uv).rgb, 1.0);
    surfaceColor *= ColorTint;
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
    return surfaceColor;
}