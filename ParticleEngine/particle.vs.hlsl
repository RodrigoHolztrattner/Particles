/////////////
// DEFINES //
/////////////
#define TotalParticles		4096

/////////////
// STRUCTS //
/////////////
struct VertexInputType
{
	uint VertId		: SV_VertexId;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct InstanceType
{
	// The particle position
	float4 position;

	// The particle last position
	float4 lastPosition;

	// The particle acceleration
	float4 acceleration;	
};

StructuredBuffer<InstanceType> instance : register(u0);

/////////////
// GLOBALS //
/////////////

///////////////
// FUNCTIONS //
///////////////

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType vertexShader(VertexInputType input)
{
    PixelInputType output;  
	
	//////////////
	// POSITION //
	//////////////

	// Set the position
	output.position = float4(instance[input.VertId].position);
	output.position.w = 1.0f;

	/////////
	// TEX //
	/////////
	
	// Set the texture coordinates
	output.tex = float2(0, 0); // 1- ?????

	// Return the output
    return output;
}