/////////////
// DEFINES //
/////////////
#define BLANK_COLOR float4(1, 1, 1, 1)
#define FULL_INTENSITY float4(1, 1, 1, 1)
#define ZERO_INTENSITY float4(0, 0, 0, 0)

/////////////
// BUFFERS //
/////////////
cbuffer TimeBuffer : register(b0)
{
	// The time
	float time;

	// The pad
	float3 pad;
};

/////////////
// STRUCTS //
/////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

///////////////
// CONSTANTS //
///////////////
Texture2D Texture :	register(t0);

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

///////////////
// FUNCTIONS //
///////////////

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 pixelShader(PixelInputType input) : SV_Target
{
	float3 textureColor;
	float4 color;

	textureColor = Texture.Sample(SampleType, input.tex);

	// Set the color
	float red, green, blue;
	
	red 	= 0.5 + cos(time*0.4+1.5)*0.5;
	green 	= 0.5 + cos(time*0.6)*sin(time*0.3)*0.35;
	blue 	= 0.5 + sin(time*0.2)*0.5;

	color = float4(red, green, blue, textureColor.g * 1.1);

	return color;

	return float4(1, 0.5, 1, 0.5);
}