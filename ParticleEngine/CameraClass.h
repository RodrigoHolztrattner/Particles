////////////////////////////////////////////////////////////////////////////////
// Filename: CameraClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CameraClass_H_
#define _CameraClass_H_


//////////////
// INCLUDES //
//////////////
#include <Windows.h>
#include <d3dx10math.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	// Initialize the camera
	void Initialize(float screenNear, float screenDepth);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

	void RenderBaseViewMatrix();
	void GetBaseViewMatrix(D3DXMATRIX&);

	// Return the screen near
	float GetScreenNear();

	// Return the screen depth
	float GetScreenDepth();

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix, m_baseViewMatrix;

	// The screen near and depth
	float m_ScreenNear, m_ScreenDepth;
};

//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// The main camera ptr
static CameraClass* s_Camera;

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// Return the global camera ptr
CameraClass* GetCamera();
#define Camera GetCamera()

#endif