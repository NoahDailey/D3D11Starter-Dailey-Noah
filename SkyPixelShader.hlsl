TextureCube CubeMap : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECITON;
};

float4 main(VertexToPixel input) : SV_TARGET
{
    return CubeMap.Sample(BasicSampler, input.sampleDir);
}