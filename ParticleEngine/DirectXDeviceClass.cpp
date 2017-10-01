////////////////////////////////////////////////////////////////////////////////
// Filename: DirectXDeviceClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "DirectXDeviceClass.h"


DirectXDeviceClass::DirectXDeviceClass()
{
	m_device = 0;
	m_swapChain = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
	m_depthDisabledStencilState = 0;

	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
}


DirectXDeviceClass::DirectXDeviceClass(const DirectXDeviceClass& other)
{
}


DirectXDeviceClass::~DirectXDeviceClass()
{
}


bool DirectXDeviceClass::Initialize()
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	float fieldOfView, screenAspect;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	D3D11_BLEND_DESC blendStateDescription;
	D3D_FEATURE_LEVEL featureLevel;

	// Set an external pointer to this
	s_DirectX = this;

	// Store the vsync setting.
	m_vsync_enabled = true; // Data()->userData.vsync;

	// Log msg
	DebugLog << "	D3DX Factory";

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result))
	{
		return false;
	}

	// Log msg
	DebugLog << "	D3DX EnumAdapters";

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result))
	{
		return false;
	}

	// Log msg
	DebugLog << "	D3DX EnumOutput";

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result))
	{
		return false;
	}

	// Log msg
	DebugLog << "	D3DX DisplayModeList";

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
	{
		return false;
	}

	// Log msg
	DebugLog << "	D3DX DisplayModeList Attempt 2";

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result))
	{
		return false;
	}
	
	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)GetSystemDefinitions()->widthResolution)
		{
			if(displayModeList[i].Height == (unsigned int)GetSystemDefinitions()->heightResolution)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Log msg
	DebugLog << "	D3DX Display Width and Height";

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = GetSystemDefinitions()->widthResolution;
    swapChainDesc.BufferDesc.Height = GetSystemDefinitions()->heightResolution;

	// Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled)
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	
	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = (HWND)GetSystemWindow()->hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
	
	// Set to full screen or windowed mode.
	if(GetSystemDefinitions()->fullScreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_10_0;

	//////////////
	// PERF HUD //
	//////////////

#if SHIPPING_VERSION

	// Look for 'NVIDIA PerfHUD' adapter
	// If it is present, override default settings
	IDXGIFactory *pDXGIFactory;
	ID3D11Device *pDevice;
	HRESULT hRes;
	hRes = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
	// Search for a PerfHUD adapter. 
	UINT nAdapter = 0;
	IDXGIAdapter* Adapter = NULL;
	IDXGIAdapter* selectedAdapter = NULL;
	D3D11_DRIVER_TYPE driverType = D3D11_DRIVER_TYPE_HARDWARE;

	while (pDXGIFactory->EnumAdapters(nAdapter, &Adapter) != DXGI_ERROR_NOT_FOUND)
	{
		if (Adapter)
		{
			DXGI_ADAPTER_DESC adaptDesc;

			if (SUCCEEDED(Adapter->GetDesc(&adaptDesc)))
			{
				const bool isPerfHUD = wcscmp(adaptDesc.Description, L"NVIDIA PerfHUD") == 0;

				// Select the first adapter in normal circumstances or the PerfHUD one if it exists.
				if(nAdapter == 0 || isPerfHUD)
				selectedAdapter = Adapter;

				if(isPerfHUD)
				driverType = D3D11_DRIVER_TYPE_REFERENCE;
			}
		}

	++nAdapter;
	}

	// Create the swap chain and the Direct3D device.
	result = D3D11CreateDeviceAndSwapChain(selectedAdapter, driverType, NULL, D3D11_CREATE_DEVICE_DEBUG, D3D11_SDK_VERSION, 
										   &swapChainDesc, &m_swapChain, &m_device);
	if(FAILED(result))
	{
		return false;
	}
#else

	// When building a shipping version, disable PerfHUD (opt-out)

	// Log msg
	DebugLog << "	D3DX => Creating Device, Device Contex and Swap Chain";

	// Create the swap chain and the Direct3D device.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
					       D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if(FAILED(result))
	{
		return false;
	}
 

#endif

	// Log msg
	DebugLog << "	D3DX SwapChain Get Buffer";

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result))
	{
		return false;
	}

	// Log msg
	DebugLog << "	D3DX Creating the Render Target";

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = GetSystemDefinitions()->widthResolution;
	depthBufferDesc.Height = GetSystemDefinitions()->heightResolution;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Log msg
	DebugLog << "	D3DX Creating the DepthStencil Texture 2D";

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Log msg
	DebugLog << "	D3DX Creating the DepthStencil state";

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Set the depth stencil state on the D3D device.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Log msg
	DebugLog << "	D3DX Creating the DepthStencil view";

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result))
	{
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false; /////////////////////////////////////////////////////////////////////////////////////////////
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Log msg
	DebugLog << "	D3DX Creating the Rasterizer state";

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	// Setup the viewport for rendering.
    m_viewport.Width = GetSystemDefinitions()->widthResolution;
    m_viewport.Height = GetSystemDefinitions()->heightResolution;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;

	// Create the viewport.
    m_deviceContext->RSSetViewports(1, &m_viewport);

	// Setup the projection matrix.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)GetSystemDefinitions()->widthResolution / (float)GetSystemDefinitions()->heightResolution;

	// Create the projection matrix for 3D rendering.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, GetSystemDefinitions()->screenNear, GetSystemDefinitions()->screenDepth);
	
    // Initialize the world matrix to the identity matrix.
    D3DXMatrixIdentity(&m_worldMatrix);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)GetSystemDefinitions()->widthResolution, (float)GetSystemDefinitions()->heightResolution, GetSystemDefinitions()->screenNear, GetSystemDefinitions()->screenDepth);

	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Log msg
	DebugLog << "	D3DX Creating the disabled DepthStencil state";

	// Create the state using the device.
	result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if(FAILED(result))
	{
		return false;
	}

	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
//	blendStateDescription.AlphaToCoverageEnable = TRUE;
	blendStateDescription.RenderTarget->BlendEnable = TRUE;
/*	blendStateDescription.BlendEnable[1] = TRUE;
	blendStateDescription.BlendEnable[2] = TRUE;
	blendStateDescription.BlendEnable[3] = TRUE;
	blendStateDescription.BlendEnable[4] = TRUE;
	blendStateDescription.BlendEnable[5] = TRUE;
	blendStateDescription.BlendEnable[6] = TRUE;
	blendStateDescription.BlendEnable[7] = TRUE;*/
    //blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDescription.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendStateDescription.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget->RenderTargetWriteMask = 0x0f;

	// Log msg
	DebugLog << "	D3DX Creating the enabled Blending state";

	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(result))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget->BlendEnable = FALSE;

	// Log msg
	DebugLog << "	D3DX Creating the disabled Blending state";

	// Create the blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(result))
	{
		return false;
	}

	// Create an alpha blend lightning state description.
//	blendStateDescription.AlphaToCoverageEnable = TRUE;
	blendStateDescription.RenderTarget->BlendEnable = TRUE;
/*	blendStateDescription.BlendEnable[1] = TRUE;
	blendStateDescription.BlendEnable[2] = TRUE;
	blendStateDescription.BlendEnable[3] = TRUE;
	blendStateDescription.BlendEnable[4] = TRUE;
	blendStateDescription.BlendEnable[5] = TRUE;
	blendStateDescription.BlendEnable[6] = TRUE;
	blendStateDescription.BlendEnable[7] = TRUE;*/
    //blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->SrcBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->DestBlend = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->DestBlendAlpha = D3D11_BLEND_ONE;
    blendStateDescription.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget->RenderTargetWriteMask = 0x0f;

	// Log msg
	DebugLog << "	D3DX Creatin the Light Blend state";

	// Create the lightning blend state using the description.
	result = m_device->CreateBlendState(&blendStateDescription, &m_alphaBlendingLightningState);
	if(FAILED(result))
	{
		return false;
	}
	
	// Log msg
	DebugLog << "	D3DX Creating the PrePass Render Targets";

	// Create the pre pass render targets
	result = CreatePrePassRenderTarget(GetSystemDefinitions()->widthResolution, GetSystemDefinitions()->heightResolution);
	if(FAILED(result))
	{
		return false;
	}

	D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS fdata;
	HRESULT te = m_device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &fdata, sizeof(D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS));

	// Log msg
	DebugLog << "	D3DX DirectX can use Compute Shader";

	// Log msg
	DebugLog << "	D3DX Load Done!";

    return true;
}


void DirectXDeviceClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if(m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}

	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	
	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	return;
}


void DirectXDeviceClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);
    
	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void DirectXDeviceClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}


ID3D11Device* DirectXDeviceClass::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* DirectXDeviceClass::GetDeviceContext()
{
	return m_deviceContext;
}

void DirectXDeviceClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void DirectXDeviceClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void DirectXDeviceClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void DirectXDeviceClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}


void DirectXDeviceClass::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void DirectXDeviceClass::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}


void DirectXDeviceClass::SetBackBufferRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void DirectXDeviceClass::ResetViewport()
{
	// Set the viewport.
    m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}

void DirectXDeviceClass::TurnOnAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void DirectXDeviceClass::TurnOffAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

HRESULT DirectXDeviceClass::CreatePrePassRenderTarget(int screenWidth, int screenHeight)
{
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC TextureDesc;

	// Depth

	// Setup the render target texture description.
	TextureDesc.Width = GetSystemDefinitions()->widthResolution;
	TextureDesc.Height = GetSystemDefinitions()->heightResolution;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format =  DXGI_FORMAT_R32_FLOAT;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = m_device->CreateTexture2D(&TextureDesc, NULL, &m_renderTargetTextureDepth);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = TextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = m_device->CreateRenderTargetView(m_renderTargetTextureDepth, &renderTargetViewDesc, &m_renderTargetDepth);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = TextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = m_device->CreateShaderResourceView(m_renderTargetTextureDepth, &shaderResourceViewDesc, &m_shaderResourceDepth);
	if(FAILED(result))
	{
		return false;
	}

	// Normals

	// Setup the render target texture description.
	TextureDesc.Width = GetSystemDefinitions()->widthResolution;
	TextureDesc.Height = GetSystemDefinitions()->heightResolution;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format =  DXGI_FORMAT_R10G10B10A2_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = m_device->CreateTexture2D(&TextureDesc, NULL, &m_renderTargetTextureNormals);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = TextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = m_device->CreateRenderTargetView(m_renderTargetTextureNormals, &renderTargetViewDesc, &m_renderTargetNormals);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = TextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = m_device->CreateShaderResourceView(m_renderTargetTextureNormals, &shaderResourceViewDesc, &m_shaderResourceNormals);
	if(FAILED(result))
	{
		return false;
	}

	// Light

	// Setup the render target texture description.
	TextureDesc.Width = GetSystemDefinitions()->widthResolution;
	TextureDesc.Height = GetSystemDefinitions()->heightResolution;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format =  DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = m_device->CreateTexture2D(&TextureDesc, NULL, &m_renderTargetTextureLight);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = TextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = m_device->CreateRenderTargetView(m_renderTargetTextureLight, &renderTargetViewDesc, &m_renderTargetLight);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = TextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = m_device->CreateShaderResourceView(m_renderTargetTextureLight, &shaderResourceViewDesc, &m_shaderResourceLight);
	if(FAILED(result))
	{
		return false;
	}

	// Final

	// Setup the render target texture description.
	TextureDesc.Width = GetSystemDefinitions()->widthResolution;
	TextureDesc.Height = GetSystemDefinitions()->heightResolution;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format =  DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = m_device->CreateTexture2D(&TextureDesc, NULL, &m_renderTargetTextureFinal);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = TextureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = m_device->CreateRenderTargetView(m_renderTargetTextureFinal, &renderTargetViewDesc, &m_renderTargetFinal);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = TextureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = m_device->CreateShaderResourceView(m_renderTargetTextureFinal, &shaderResourceViewDesc, &m_shaderResourceFinal);
	if(FAILED(result))
	{
		return false;
	}	

	return S_OK;
}

bool DirectXDeviceClass::SetDepthNormalRenderTarget()
{
	ID3D11RenderTargetView* renderTargets[2];

	// Set the render targets
	renderTargets[0] = m_renderTargetDepth;
	renderTargets[1] = m_renderTargetNormals;

	// Set the render target
	m_deviceContext->OMSetRenderTargets(2, renderTargets, m_depthStencilView);

	return true;
}

bool DirectXDeviceClass::SetLightRenderTarget()
{
	// Set the render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetLight, m_depthStencilView);

	return true;
}

void DirectXDeviceClass::ClearDepthNormalRenderTarget(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the depth render target.
	m_deviceContext->ClearRenderTargetView(m_renderTargetDepth, color);

	// Clear the notmal render target.
	m_deviceContext->ClearRenderTargetView(m_renderTargetNormals, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    
	return;
}

void DirectXDeviceClass::ClearLightRenderTarget(float red, float green, float blue, float alpha)
{
	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the depth render target.
	m_deviceContext->ClearRenderTargetView(m_renderTargetLight, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    
	return;
}

ID3D11ShaderResourceView* DirectXDeviceClass::GetDepthShaderResourceView()
{
	return m_shaderResourceDepth;
}

ID3D11ShaderResourceView* DirectXDeviceClass::GetNormalShaderResourceView()
{
	return m_shaderResourceNormals;
}

ID3D11ShaderResourceView* DirectXDeviceClass::GetLightShaderResourceView()
{
	return m_shaderResourceLight;
}

void DirectXDeviceClass::SetAlphaBlendingLightningConfiguration()
{
	float blendFactor[4];
	
	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	m_deviceContext->OMSetBlendState(m_alphaBlendingLightningState, blendFactor, 0xffffffff);

	return;
}

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

DirectXDeviceClass* GetDirectX()
{
	return s_DirectX;
}