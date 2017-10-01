////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleShaderClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ParticleShaderClass_H_
#define _ParticleShaderClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11.h>
#include <fstream>
#include <string>

#include "DirectXDeviceClass.h"
#include "WindowClass.h"
#include "CameraClass.h"


/////////////
// DEFINES //
/////////////

static unsigned int TotalParticles;

void SetTotalParticles(unsigned int total);
// #define TotalParticles				98304 // 32768

#define Max_Compute_Shader_Threads	512

struct ParticleInstance
{
	// The particle position
	D3DXVECTOR4 position;

	// The particle last position
	D3DXVECTOR4 lastPosition;

	// The particle acceleration
	D3DXVECTOR4 acceleration;
};

struct StatusData
{
	StatusData::StatusData(float tim, float frameTim, float quad, D3DXVECTOR3 attr)
	{
		// Set the values
		time = tim;
		frameTime = frameTim;
		quadLength = quad;
		attractor = attr;
	}

	// The time
	float time;

	// The frame time
	float frameTime;

	// The quad length
	float quadLength;

	// The attractor
	D3DXVECTOR3 attractor;
};

struct SessionData
{
	SessionData::SessionData(bool pull, bool limit, bool grav, bool resist, float forceFact, float resistenceForc, int area)
	{
		// Set the values
		pulling = pull;
		limitingPosition = limit;
		gravity = grav;
		resistence = resist;
		forceFactor = forceFact;
		resistenceForce = resistenceForc;
		areaLimit = area;
	}

	// If we are pulling the particles
	bool pulling;

	// If we are limiting the position or the acceleration
	bool limitingPosition;

	// If the gravity is on
	bool gravity;

	// If we are using air resistence this will be true
	bool resistence;

	// The factor that will multiply the force
	float forceFactor;

	// The air resistence force
	float resistenceForce;

	// The area that we are limiting the particles
	int areaLimit;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleShaderClass
////////////////////////////////////////////////////////////////////////////////
class ParticleShaderClass
{
	struct MatrixBufferType
	{
		D3DXVECTOR3 camPosition;
		float quadLength;
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct TimeBufferType
	{
		float time;

		D3DXVECTOR3 pad;
	};

	struct ParticleDataType
	{
		unsigned int groupDimX;
		unsigned int groupDimY;
		unsigned int maxParticles;
		unsigned int pad;
	};

	struct AttractorType
	{
		D3DXVECTOR3 attractor;
		float frameTime;

		D3DXVECTOR4 data;

		float forceFactor;
		float resistenceForce;
		int areaLimit;
		int pad;	
	};

public:

	ParticleShaderClass();
	ParticleShaderClass(const ParticleShaderClass&);
	~ParticleShaderClass();

	// The initialize function (call this after the object is created)
	bool Initialize();

	// The shutdown function (call this to before delete the object)
	void Shutdown();

	// The render function
	void Render(StatusData statusData, SessionData sessionData, ParticleInstance* particleInstance, unsigned int numberInstances, unsigned int numberIndexes);

	// Create the instance buffer
	bool CreateInstanceBuffer(ID3D11Device* device, ParticleInstance* particles, int maxParticles);

	// Calculate the number of thread groups
	void CalculateThreadGroups(int maxParticles);

private:

	// The set shader parameters function (call this function before the render start to set the constant buffers info)
	bool SetShaderParameters(StatusData statusData, SessionData sessionData, ParticleInstance* particleInstance, unsigned int numberInstances);

	// The render shader function (this will render the vertex and pixel shader)
	void RenderShader(unsigned int numberInstances, unsigned int numberIndexes);

private:

	// The initialize shader function (load the vertex and pixel shader and set the basic buffers infos
	bool InitializeShader(LPCWSTR vsFilename, LPCWSTR gsFilename, LPCWSTR psFilename);

	// The shutdown shader function (unload the vertex and pixel shader)
	void ShutdownShader();

	// The output shader error message (this will output an error message if the creation of the shaders dont succede)
	void OutputShaderErrorMessage(ID3D10Blob*, LPCWSTR);

	// This function will create the second (real) vertex layout
	bool CreateLayout(LPCWSTR csFilename, LPCWSTR vsFilename, LPCWSTR gsFilename, LPCWSTR psFilename);

private:

	// The number of X threads
	unsigned int m_ThreadGroupsX;

	// The number of Y threads
	unsigned int m_ThreadGroupsY;

	// The number of Z threads
	unsigned int m_ThreadGroupsZ;
		
private:

	// The shader layout
	ID3D11InputLayout* m_layout;

	// The compute shader
	ID3D11ComputeShader* m_ComputeShader;

	// The vertex shader
	ID3D11VertexShader* m_VertexShader;

	// The geometry shader
	ID3D11GeometryShader* m_GeometryShader;

	// The pixel shader
	ID3D11PixelShader* m_PixelShader;

private: // Buffers

	// The matrix buffer
	ID3D11Buffer* m_MatrixBuffer;

	// The instance buffer
	ID3D11Buffer* m_InstanceBuffer;

	// The particle data buffer
	ID3D11Buffer* m_ParticleDataBuffer;

	// The attractor data buffer
	ID3D11Buffer* m_AttractorBuffer;

	// The instance buffer view
	ID3D11ShaderResourceView* m_InstanceBufferView;

	// The unordered instance buffer view
	ID3D11UnorderedAccessView*	m_UnorderedInstanceBufferView;

	// The timer buffer
	ID3D11Buffer* m_TimerBuffer;

	// The particle texture
	ID3D11ShaderResourceView* m_ParticleTexture;
};

#endif
