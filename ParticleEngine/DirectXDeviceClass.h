////////////////////////////////////////////////////////////////////////////////
// Filename: DirectXDeviceClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DirectXDeviceClass_H_
#define _DirectXDeviceClass_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3d10.h>
#include <d3dx10.h>
// #include "Global.h"

#include "DefinitionsClass.h"
#include "WindowClass.h"
#include "LogClass.h"



/////////////
// DEFINES //
/////////////

////////////////////////////////////////////////////////////////////////////////
// Class name: DirectXDeviceClass
////////////////////////////////////////////////////////////////////////////////
class DirectXDeviceClass
{
private:

public:
	DirectXDeviceClass();
	DirectXDeviceClass(const DirectXDeviceClass&);
	~DirectXDeviceClass();

	bool Initialize();
	void Shutdown();
	
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	void SetBackBufferRenderTarget();
	void ResetViewport();

public: // Light pre-pass functions

	// This function will create the pre-pass render targets view
	HRESULT CreatePrePassRenderTarget(int screenWidth, int screenHeight);
	
	// This function will set the render target to the depth and normal render targets
	bool SetDepthNormalRenderTarget();

	// This function will set the render target to light render target
	bool SetLightRenderTarget();

	// This function will clear the depth and the normal render targets
	void ClearDepthNormalRenderTarget(float red, float green, float blue, float alpha);

	// This function will clear the light render target
	void ClearLightRenderTarget(float red, float green, float blue, float alpha);

	// This function will set the alpha blending configuration for the lightning shader
	void SetAlphaBlendingLightningConfiguration();

	ID3D11ShaderResourceView* GetDepthShaderResourceView();
	ID3D11ShaderResourceView* GetNormalShaderResourceView();
	ID3D11ShaderResourceView* GetLightShaderResourceView();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_depthDisabledStencilState;
	D3D11_VIEWPORT m_viewport;

	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

private: // Light pre-pass render targets	

	ID3D11Texture2D* m_renderTargetTextureDepth;
	ID3D11RenderTargetView* m_renderTargetDepth;
	ID3D11ShaderResourceView* m_shaderResourceDepth;

	ID3D11Texture2D* m_renderTargetTextureNormals;
	ID3D11RenderTargetView* m_renderTargetNormals;
	ID3D11ShaderResourceView* m_shaderResourceNormals;

	ID3D11Texture2D* m_renderTargetTextureLight;
	ID3D11RenderTargetView* m_renderTargetLight;
	ID3D11ShaderResourceView* m_shaderResourceLight;

	ID3D11Texture2D* m_renderTargetTextureFinal;
	ID3D11RenderTargetView* m_renderTargetFinal;
	ID3D11ShaderResourceView* m_shaderResourceFinal;

	ID3D11BlendState* m_alphaBlendingLightningState;
};

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// Return the directX class object
DirectXDeviceClass* GetDirectX();
#define DirectX GetDirectX()

//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// The directX class
static DirectXDeviceClass* s_DirectX;

#endif
