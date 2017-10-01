///////////////////////////////////////////////////////////////////////////////
// Filename: ParticleShaderClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "ParticleShaderClass.h"

ParticleShaderClass::ParticleShaderClass()
{
}

ParticleShaderClass::ParticleShaderClass(const ParticleShaderClass& other)
{
}

ParticleShaderClass::~ParticleShaderClass()
{
}

bool ParticleShaderClass::Initialize()
{
	bool result;

	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(L"../ParticleEngine/particle.vs", L"../ParticleEngine/particle.gs", L"../ParticleEngine/particle.ps");
	if(!result)
	{
		return false;
	}

	return true;
}

void ParticleShaderClass::Shutdown()
{
	// Shutdown the shader effect.
	ShutdownShader();

	return;
}

void ParticleShaderClass::Render(StatusData statusData, SessionData sessionData, ParticleInstance* particleInstance, unsigned int numberInstances, unsigned int numberIndexes)
{
	bool result;

	// Set the shader parameters
	result = SetShaderParameters(statusData, sessionData, particleInstance, numberInstances);
	if(!result)
	{
		return;
	}

	// Render
	RenderShader(numberInstances, numberIndexes);

	return;
}

bool ParticleShaderClass::InitializeShader(LPCWSTR vsFilename, LPCWSTR gsFilename, LPCWSTR psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
    D3D10_PASS_DESC passDesc;
	D3D11_BUFFER_DESC ParticleDataBufferDesc;
	D3D11_BUFFER_DESC AttractorDataBufferDesc;
	D3D11_BUFFER_DESC MatrixBufferDesc;
	D3D11_BUFFER_DESC TimerBufferDesc;
	ParticleDataType ParticleData;
	D3D11_SUBRESOURCE_DATA data;
	bool bResult;
	int i;

	// Create the real vertex layout
	bResult = CreateLayout(L"../ParticleEngine/particle.cs.hlsl", L"../ParticleEngine/particle.vs.hlsl", L"../ParticleEngine/particle.gs.hlsl", L"../ParticleEngine/particle.ps.hlsl");
	if(!bResult)
	{
		return false;
	}


	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	MatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	MatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBufferDesc.MiscFlags = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = DirectX->GetDevice()->CreateBuffer(&MatrixBufferDesc, NULL, &m_MatrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic instance constant buffer that is in the vertex shader.
	TimerBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	TimerBufferDesc.ByteWidth = sizeof(TimeBufferType);
	TimerBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TimerBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TimerBufferDesc.MiscFlags = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = DirectX->GetDevice()->CreateBuffer(&TimerBufferDesc, NULL, &m_TimerBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic instance constant buffer that is in the vertex shader.
	ParticleDataBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ParticleDataBufferDesc.ByteWidth = sizeof(ParticleDataType);
	ParticleDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ParticleDataBufferDesc.CPUAccessFlags = 0;
	ParticleDataBufferDesc.MiscFlags = 0;

	// Set the start data
	ParticleData.groupDimX		= m_ThreadGroupsX;
	ParticleData.groupDimY		= m_ThreadGroupsY;
	ParticleData.maxParticles	= TotalParticles;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem	= &ParticleData;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = DirectX->GetDevice()->CreateBuffer(&ParticleDataBufferDesc, &data, &m_ParticleDataBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic instance constant buffer that is in the vertex shader.
	AttractorDataBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	AttractorDataBufferDesc.ByteWidth = sizeof(AttractorType);
	AttractorDataBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	AttractorDataBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	AttractorDataBufferDesc.MiscFlags = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = DirectX->GetDevice()->CreateBuffer(&AttractorDataBufferDesc, NULL, &m_AttractorBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Create the particle texture
	result = D3DX11CreateShaderResourceViewFromFile(DirectX->GetDevice(), L"Particle.png", NULL, NULL, &m_ParticleTexture, NULL);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool ParticleShaderClass::CreateInstanceBuffer(ID3D11Device* device, ParticleInstance* particles, int maxParticles)
{
	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.BindFlags			= D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE ; 
	buffDesc.ByteWidth			= sizeof(ParticleInstance)*maxParticles;
	buffDesc.MiscFlags			= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	buffDesc.StructureByteStride= sizeof(ParticleInstance);
	buffDesc.Usage				= D3D11_USAGE_DEFAULT; 

	D3D11_SUBRESOURCE_DATA	data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = particles;

	if(FAILED(device->CreateBuffer(&buffDesc, &data, &m_InstanceBuffer)))
	{
		MessageBox(NULL, L"Could not create source particle buffer for compute shaders!", L"Error", 0);
		return false;
	}

	ZeroMemory(&buffDesc, sizeof(buffDesc));
	m_InstanceBuffer->GetDesc(&buffDesc);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension			= D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement		= 0;
	srvDesc.Format					= DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
	srvDesc.Buffer.NumElements		= maxParticles;

	if(FAILED(device->CreateShaderResourceView(m_InstanceBuffer, &srvDesc, &m_InstanceBufferView)))
	{
		MessageBox(NULL, L"Could not create source particle buffer view for compute shaders!", L"Error", 0);
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension			= D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags			= 0;
	uavDesc.Buffer.FirstElement		= 0;
	uavDesc.Format					= DXGI_FORMAT_UNKNOWN; //Must be UNKOWN when creating structured Buffer
	uavDesc.Buffer.NumElements		= maxParticles;

	if(FAILED(device->CreateUnorderedAccessView(m_InstanceBuffer, &uavDesc, &m_UnorderedInstanceBufferView)))
	{
		MessageBox(NULL, L"Could not create  source particle buffer UAV for compute shaders!", L"Error", 0);
		return false;
	}

	return true;
}

void ParticleShaderClass::CalculateThreadGroups(int maxParticles)
{
	int numGroups = 0;

	if(maxParticles % Max_Compute_Shader_Threads != 0){
		numGroups = (maxParticles/Max_Compute_Shader_Threads)+1;
	} else {
		numGroups = maxParticles/Max_Compute_Shader_Threads;
	}

	double thirdRoot = pow((double)numGroups, (double)(1.0/2.0)); // 3
	thirdRoot = ceil(thirdRoot);

	m_ThreadGroupsX = m_ThreadGroupsY = m_ThreadGroupsZ = static_cast <UINT> (thirdRoot);
	m_ThreadGroupsZ = 1;
}

bool ParticleShaderClass::CreateLayout(LPCWSTR csFilename, LPCWSTR vsFilename, LPCWSTR gsFilename, LPCWSTR psFilename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* computeShaderBuffer;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* geometryShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[8];
	unsigned int numElements;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // Compile the compute shader code.
	result = D3DX11CompileFromFile(csFilename, NULL, NULL, "computeShader", "cs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &computeShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, csFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(SystemWindow->hwnd, csFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "vertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(SystemWindow->hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the geometry shader code.
	result = D3DX11CompileFromFile(gsFilename, NULL, NULL, "geometryShader", "gs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &geometryShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, gsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(SystemWindow->hwnd, gsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
								   &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(SystemWindow->hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // Create the compute shader from the buffer.
    result = DirectX->GetDevice()->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), NULL, &m_ComputeShader);
	if(FAILED(result))
	{
		return false;
	}

	// Calculate the number of thread groups
	CalculateThreadGroups(TotalParticles);
	
    // Create the vertex shader from the buffer.
    result = DirectX->GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the geometry shader from the buffer.
    result = DirectX->GetDevice()->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &m_GeometryShader);
	if(FAILED(result))
	{
		return false;
	}

    // Create the pixel shader from the buffer.
    result = DirectX->GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
	if(FAILED(result))
	{
		return false;
	}

	// The layout of the vertex buffer (per vertex data) //
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "NORMAL";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
	
	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "BONES";
	polygonLayout[5].SemanticIndex = 0;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	polygonLayout[6].SemanticName = "WEIGHTS";
	polygonLayout[6].SemanticIndex = 0;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[6].InputSlot = 0;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[6].InstanceDataStepRate = 0;

	polygonLayout[7].SemanticName = "TEXCOORD";
	polygonLayout[7].SemanticIndex = 1;
	polygonLayout[7].Format = DXGI_FORMAT_R32_UINT;
	polygonLayout[7].InputSlot = 0;
	polygonLayout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[7].InstanceDataStepRate = 0;

	//

	// Get a count of the elements in the layout.
    numElements = 1; // sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the input layout.
	result = DirectX->GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	geometryShaderBuffer->Release();
	geometryShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	return true;
}

void ParticleShaderClass::ShutdownShader()
{
	// Release the pointer to the shader layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	return;
}

void ParticleShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, LPCWSTR shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(SystemWindow->hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool ParticleShaderClass::SetShaderParameters(StatusData statusData, SessionData sessionData, ParticleInstance* particleInstance, unsigned int numberInstances)
{
	unsigned int bufferNumber;
	MatrixBufferType* MatrixDataPtr;
	ParticleInstance* InstanceDataPtr;
	TimeBufferType* TimerDataPtr;
	AttractorType* AttractorDataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	bool result;
	HRESULT Hresult;
	D3DXMATRIX* tempArray;
	ID3D11ShaderResourceView** currentTexture;
	ID3D11ShaderResourceView* AnimationTexture;
	int i, index;

	D3DXMATRIX World;
	D3DXMATRIX View;
	D3DXMATRIX Projection;

	// MATRIX BUFFER //

	// Get the matrices
	DirectX->GetWorldMatrix(World);
	Camera->GetViewMatrix(View);
	DirectX->GetProjectionMatrix(Projection);
	
	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&World, &World);
	D3DXMatrixTranspose(&View, &View);
	D3DXMatrixTranspose(&Projection, &Projection);

	// Lock the instant buffer so it can be written to.
	Hresult = DirectX->GetDeviceContext()->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(Hresult))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	MatrixDataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the data into the constant buffer.
	MatrixDataPtr->world = World;
	MatrixDataPtr->view = View;
	MatrixDataPtr->projection = Projection;
	MatrixDataPtr->camPosition = Camera->GetPosition();
	MatrixDataPtr->quadLength = statusData.quadLength;

	// Unlock the constant buffer.
    DirectX->GetDeviceContext()->Unmap(m_MatrixBuffer, 0);

	// Set the position of the instance constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the instance buffer in the vertex shader with the updated values.
	DirectX->GetDeviceContext()->GSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

	// TIMER BUFFER //

	// Lock the timer buffer so it can be written to.
	Hresult = DirectX->GetDeviceContext()->Map(m_TimerBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(Hresult))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	TimerDataPtr = (TimeBufferType*)mappedResource.pData;
	
	// Copy the data into the constant buffer.
	TimerDataPtr->time = statusData.time;
	TimerDataPtr->pad = D3DXVECTOR3(1, 1, 1);

	// Unlock the constant buffer.
	DirectX->GetDeviceContext()->Unmap(m_TimerBuffer, 0);

	// Set the position of the timer constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the instance buffer in the vertex shader with the updated values.
	DirectX->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &m_TimerBuffer);

	// ATTRACTOR BUFFER //

	// Lock the attractor buffer so it can be written to.
	Hresult = DirectX->GetDeviceContext()->Map(m_AttractorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(Hresult))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	AttractorDataPtr = (AttractorType*)mappedResource.pData;
	
	// Copy the data into the constant buffer.
	AttractorDataPtr->frameTime = statusData.frameTime;
	AttractorDataPtr->attractor = statusData.attractor;
	AttractorDataPtr->areaLimit = sessionData.areaLimit;
	AttractorDataPtr->forceFactor = sessionData.forceFactor;
	AttractorDataPtr->data.x = sessionData.pulling;
	AttractorDataPtr->data.y = sessionData.limitingPosition;
	AttractorDataPtr->data.z = sessionData.gravity;
	AttractorDataPtr->data.w = sessionData.resistence;
	AttractorDataPtr->resistenceForce = sessionData.resistenceForce;

	// Unlock the constant buffer.
	DirectX->GetDeviceContext()->Unmap(m_AttractorBuffer, 0);

	// COMPUTE SHADER

	// Set the position of the timer constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the instance buffer in the vertex shader with the updated values.
	DirectX->GetDeviceContext()->CSSetConstantBuffers(bufferNumber, 1, &m_ParticleDataBuffer);

	// Set the position of the timer constant buffer in the pixel shader.
	bufferNumber = 1;

	// Finally set the instance buffer in the vertex shader with the updated values.
	DirectX->GetDeviceContext()->CSSetConstantBuffers(bufferNumber, 1, &m_AttractorBuffer);

	// Set the unordered instance buffer
	DirectX->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_UnorderedInstanceBufferView, NULL); 

	// Set the compute shader
	DirectX->GetDeviceContext()->CSSetShader(m_ComputeShader, NULL, 0);

	// Dispatch
	DirectX->GetDeviceContext()->Dispatch(m_ThreadGroupsX, m_ThreadGroupsY, m_ThreadGroupsZ);

	// Set a null unordered view
	DirectX->GetDeviceContext()->CSSetShader(NULL, NULL, 0);
	ID3D11UnorderedAccessView* pNullUAV = NULL;
	DirectX->GetDeviceContext()->CSSetUnorderedAccessViews( 0, 1, &pNullUAV, NULL );
	ID3D11Buffer* ppSRVNULL[2] = { NULL, NULL };
	DirectX->GetDeviceContext()->CSSetConstantBuffers(0, 2, ppSRVNULL);

	// INSTANCE BUFFER //

	// Lock the instance buffer so it can be written to.
//	DirectX->GetDeviceContext()->UpdateSubresource(m_InstanceBuffer, 0 , 0, particleInstance, 0, 0);

	// Set the position of the instance constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the instance buffer in the vertex shader with the updated values.
	DirectX->GetDeviceContext()->VSSetShaderResources(bufferNumber, 1, &m_InstanceBufferView);

	//

	// Put the particle texture at the pixel shader
	DirectX->GetDeviceContext()->PSSetShaderResources(0, 1, &m_ParticleTexture);

	return true;
}

void ParticleShaderClass::RenderShader(unsigned int numberInstances, unsigned int numberIndexes)
{
	// Set the vertex input layout.
//	DirectX->GetDevice()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	DirectX->GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	DirectX->GetDeviceContext()->GSSetShader(m_GeometryShader, NULL, 0);
	DirectX->GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	// Render the triangle.
	DirectX->GetDeviceContext()->Draw(numberInstances, 0);
	// DirectX->GetDeviceContext()->DrawIndexedInstanced(numberIndexes, numberInstances, 0, 0, 0);
	
	return;
}

void SetTotalParticles(unsigned int total)
{
	TotalParticles = total;
}