////////////////////////////////////////////////////////////////////////////////
// Filename: ParticleClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ParticleClass_H_
#define _ParticleClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx11.h>
#include <d3d10.h>
#include <d3dx10.h>

/////////////
// DEFINES //
/////////////

#define GRAVITY_FORCE			0.00012f

#define VERTICAL_LIMIT			210.0f
#define HORIZONTAL_LIMIT		420.0f
#define SIDE_LIMIT				400.0f

////////////////////////////////////////////////////////////////////////////////
// Class name: ParticleClass
////////////////////////////////////////////////////////////////////////////////
class ParticleClass
{
private:

public:
	ParticleClass();
	ParticleClass(const ParticleClass&);
	~ParticleClass();

	// Set the atual position
	void SetPosition(D3DXVECTOR3 position);

	// Return the atual position
	D3DXVECTOR3 GetPosition();

	// Set the acceleration
	void SetAcceleration(D3DXVECTOR3 acceleration);

	// Update the acceleration
	void UpdateAcceleration(float factor, float range, bool gravity, bool limit);

	// Increment the acceleration
	void IncrementAcceleration(D3DXVECTOR3 acceleration);

	// Return the acceleration
	D3DXVECTOR3 GetAcceleration();

	// Update the particle
	void Update(float factor, float range, bool gravity);

private:

	// Check the extremes for the acceleration
	void AccelerationExtremes();

private:

	// The atual position
	D3DXVECTOR3 m_Position;

	// The last position
	D3DXVECTOR3 m_LastPosition;

	// The acceleration
	D3DXVECTOR3 m_Acceleration;

	// The weight
	float m_Weight;
};

#endif
