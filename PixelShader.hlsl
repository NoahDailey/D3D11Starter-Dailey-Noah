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
    
    // Needed calculations for diffuse
    float3 lightDirection = normalize(DirectionalLight1.Direction);
    
    // Diffuse value
    float diffuseIntensity = saturate(dot(input.normal, -lightDirection));
    float3 diffuseTerm = diffuseIntensity * DirectionalLight1.Color * DirectionalLight1.Intensity * surfaceColor;
    
    // Needed calculations for specular
    float3 camDirection = normalize(CameraPosition - input.worldPosiiton);
    
    // Specular Calculations
    // A specular scaling factor might come from a texture (just the red channel)
    float specScale = SurfaceTexture.Sample(BasicSampler, input.uv).r;
    
    // Calculate the reflection of the INCOMING light direction
    // using the durface normal (ensure both are normalized first
    float3 reflection = reflect(lightDirection, input.normal);
    
    // Calculate cosine of the reflection and camera vector
    // - saturate() ensures it's between 0 and 1
    float RdotV = saturate(dot(reflection, camDirection));
    
    // Raise the that to a power equal to some "shininess"
    // factor - try large numbers here (64, 200, etc.)
    float3 specularTerm = pow(RdotV, SHININESS) * specScale *   // Initial specular term * scale
    DirectionalLight1.Color * DirectionalLight1.Intensity *     // Colored by light's color
    surfaceColor;                                               // Tinted by surface color
    
    // Build the final color
    finalColor = ambientTerm + diffuseTerm + specularTerm;
   
    return float4(finalColor, 1);
}