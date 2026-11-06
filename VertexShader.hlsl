#include "ShaderInclude.hlsli"

// Identify the constant buffer of external data
// on the GPU to the Vertex Shader
cbuffer ExternalData : register(b0)
{
    float4x4	World;				// A change in transform
    float4x4	ViewMatrix;			// A camera view matrix
    float4x4	ProjectionMatrix;	// A camera projection matrix
    float4x4	WorldInvTranspose;	// An inverse of the world transform
}

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float2 uv				: TEXCOORD;		// RG uv
    float3 normal			: NORMAL;		// RGB normal
    float3 tangent			: TANGENT;		// A line across teh vertex
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// Here we're essentially passing the input position directly through to the next
	// stage (rasterizer), though it needs to be a 4-component vector now.  
	// - To be considered within the bounds of the screen, the X and Y components 
	//   must be between -1 and 1.  
	// - The Z component must be between 0 and 1.  
	// - Each of these components is then automatically divided by the W component, 
	//   which we're leaving at 1.0 for now (this is more useful when dealing with 
	//   a perspective projection matrix, which we'll get to in the future).
    matrix wvp = mul(ProjectionMatrix, mul(ViewMatrix, World));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	
    output.uv = input.uv;
    output.normal = mul((float3x3) WorldInvTranspose, input.normal);
    output.worldPosiiton = mul(World, float4(input.localPosition, 1)).xyz;
    output.tangent = mul((float3x3) World, input.tangent);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}