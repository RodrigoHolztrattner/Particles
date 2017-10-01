///////////////////////////////////////////////////////////////////////////////
// Filename: ParticleClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "ParticleClass.h"

ParticleClass::ParticleClass()
{
	// Zero the position
	m_Position = D3DXVECTOR3(0, 0, 0);

	// Zero the acceleration
	m_Acceleration = D3DXVECTOR3(0, 0, 0);

	// Set the weight
	m_Weight = 0.99f;
}

ParticleClass::ParticleClass(const ParticleClass& other)
{
	m_Position = other.m_Position;
	m_LastPosition = other.m_LastPosition;
	m_Acceleration = other.m_Acceleration;
}

ParticleClass::~ParticleClass()
{
}

void ParticleClass::SetPosition(D3DXVECTOR3 position)
{
	m_Position = position;
}

D3DXVECTOR3 ParticleClass::GetPosition()
{
	return m_Position;
}

void ParticleClass::SetAcceleration(D3DXVECTOR3 acceleration)
{
	m_Acceleration = acceleration;
}

void ParticleClass::IncrementAcceleration(D3DXVECTOR3 acceleration)
{
	m_Acceleration += acceleration;
}

void ParticleClass::UpdateAcceleration(float factor, float range, bool gravity, bool limit)
{
	D3DXVECTOR3 acceleration;

	// Check if the gravity is active
	if(gravity)
	{
		m_Acceleration.y -= GRAVITY_FORCE;
	}

	if(limit)
	{
		AccelerationExtremes();
	}

	// Normalize the acceleration
	if(D3DXVec3Length(&m_Acceleration) > 1)
	{
		
	}

	D3DXVec3Normalize(&acceleration, &m_Acceleration);

	// Set the last position and the actual position
	m_LastPosition = m_Position;
	m_Position = 2.0*m_Position - m_LastPosition + acceleration * factor;

	// Check if we are only using the gravity
	if(!gravity)
	{
		//Keep all the particles inside
		if(D3DXVec3Length(&m_Acceleration) > range)
		{
			D3DXVECTOR3 norm;
			D3DXVec3Normalize(&norm, &m_Acceleration);
			m_Acceleration = norm*range;
		}
	}
}

void ParticleClass::AccelerationExtremes()
{
	// Check if we arent on the ground
	if(m_Position.y > VERTICAL_LIMIT)
	{
		m_Acceleration.y *= -1;
	}

	// Check if we arent on the top
	if(m_Position.y < -VERTICAL_LIMIT)
	{
		m_Acceleration.y *= -1;
	}

	// Check if we arent on the right
	if(m_Position.x > HORIZONTAL_LIMIT)
	{
		m_Acceleration.x *= -1;
	}

	// Check if we arent on the left
	if(m_Position.x < -HORIZONTAL_LIMIT)
	{
		m_Acceleration.x *= -1;
	}

	//
	if(m_Position.z > SIDE_LIMIT)
	{
		m_Acceleration.z *= -1;
	}

	//
	if(m_Position.z < -SIDE_LIMIT)
	{
		m_Acceleration.z *= -1;
	}
}

D3DXVECTOR3 ParticleClass::GetAcceleration()
{
	return m_Acceleration;
}

void ParticleClass::Update(float factor, float range, bool gravity)
{
	D3DXVECTOR3 acceleration;

	// Normalize the acceleration
	D3DXVec3Normalize(&acceleration, &m_Acceleration);

	// Set the last position and the actual position
	m_LastPosition = m_Position;
	m_Position = 2.0*m_Position - m_LastPosition + acceleration * factor;

	// Check if we are only using the gravity
	if(!gravity)
	{
		//Keep all the particles inside
		if(D3DXVec3Length(&m_Position) > range)
		{
			D3DXVECTOR3 norm;
			D3DXVec3Normalize(&norm, &m_Position);
			m_Position = norm*range;
		}
	}
}