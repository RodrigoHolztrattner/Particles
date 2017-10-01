/////////////
// DEFINES //
/////////////

/////////////
// BUFFERS //
/////////////
cbuffer MatrixBuffer
{
	float3 camPosition;
	float quadLength;
    matrix realWorldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

/////////////
// STRUCTS //
/////////////

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

struct GeometryInputType
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

///////////////
// CONSTANTS //
///////////////

///////////////
// FUNCTIONS //
///////////////

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(4)]
void geometryShader(point GeometryInputType p[1], inout TriangleStream<PixelInputType> triStream){
	PixelInputType p1 = (PixelInputType)0;

    float3 normal			= p[0].pos - camPosition;
	normal					= mul(normal, viewMatrix);

    float3 rightAxis		= cross(float3(0.0f, 1.0f, 0.0f), normal);
    float3 upAxis			= cross(normal, rightAxis);
    rightAxis				= normalize(rightAxis);
    upAxis					= normalize(upAxis);

    float4 rightVector		= float4(rightAxis.xyz, 1.0f);
    float4 upVector         = float4(upAxis.xyz, 1.0f);
	p[0].pos				= mul(p[0].pos, viewMatrix);

    p1.pos = p[0].pos+rightVector*(quadLength)+upVector*(quadLength);
    p1.tex = float2(1.0f, 0.0f);
    p1.pos = mul(p1.pos, projectionMatrix);
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(quadLength)+upVector*(-quadLength);
    p1.tex = float2(1.0f, 1.0f);
    p1.pos = mul(p1.pos, projectionMatrix);
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(-quadLength)+upVector*(quadLength);
    p1.tex = float2(0.0f, 0.0f);
    p1.pos = mul(p1.pos, projectionMatrix);
    triStream.Append(p1);

    p1.pos = p[0].pos+rightVector*(-quadLength)+upVector*(-quadLength);
    p1.tex = float2(0.0f, 1.0f);
    p1.pos = mul(p1.pos, projectionMatrix);
    triStream.Append(p1);
}