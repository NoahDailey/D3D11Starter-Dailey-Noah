// Identify the constant buffer of external data
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    float4x4 ViewMatrix; // A camera view matrix
    float4x4 ProjectionMatrix; // A camera projection matrix
}

struct VertexShaderInput
{
    float3 localPosition    : POSITION; // XYZ position
    float2 uv               : TEXCOORD; // RG uv
    float3 normal           : NORMAL; // RGB normal
    float3 tangent          : TANGENT; // A line across teh vertex
};

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECITON;
};

VertexToPixel main(VertexShaderInput input)
{
    // Set up output struct
    VertexToPixel output;
    
    float4x4 viewNoTranslation = ViewMatrix;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    /* CONTINUE SKYBOX IMPLEMENTATION FROM PAGE 6 */
    matrix viewProj = mul(ProjectionMatrix, viewNoTranslation);
    output.position = mul(viewProj, float4(input.localPosition, 1.0f));
    
    output.position.z = output.position.w;
    
    output.sampleDir = input.localPosition;
    
    return output;
}