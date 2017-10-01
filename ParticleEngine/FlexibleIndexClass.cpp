///////////////////////////////////////////////////////////////////////////////
// Filename: FlexibleIndexClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "FlexibleIndexClass.h"

FlexibleIndexClass::FlexibleIndexClass()
{
	// Set the data deleted to false
	m_DataDeleted = false;

	// Set the buffers created to false
	m_BuffersCreated = false;

	// Zero the size
	m_IndexSize = 0;
}

FlexibleIndexClass::FlexibleIndexClass(const FlexibleIndexClass& other)
{
}

FlexibleIndexClass::~FlexibleIndexClass()
{
}

bool FlexibleIndexClass::Add(unsigned int* indexFrom, unsigned int quantity)
{
	// Check if we dont deleted the data
	if(m_DataDeleted)
	{
		return false;
	}

	// Add the indexes
	m_Indexes.Add(indexFrom, quantity);

	// Set the sizes
	m_IndexSize = m_Indexes.Size();

	return true;
}

int FlexibleIndexClass::operator [] (unsigned int index)
{
	// Check if we dont deleted the data
	if(m_DataDeleted)
	{
		return false;
	}

	return m_Indexes[index];
}

bool FlexibleIndexClass::CreateBuffers(ID3D11Device* device, bool deleteData)
{
	bool result;
	unsigned int indexSize;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT Hresult;

	// Set the index size
	indexSize = sizeof(unsigned int) * m_Indexes.Size();
	
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = indexSize;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = m_Indexes.GetArray();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	Hresult = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if(FAILED(Hresult))
	{
		return false;
	}

	// Set the buffers created to true
	m_BuffersCreated = true;

	// Check if the data should be deleted
	if(deleteData)
	{
		// Clean the index array
		m_Indexes.Zero();

		// Set the deleted data to true
		m_DataDeleted = true;
	}

	return true;
}

void FlexibleIndexClass::Shutdown()
{
	// See if we need to delete the data
	if(!m_DataDeleted)
	{
		// Clean it
		m_Indexes.Zero();
	}

	// See if we need to delete the buffers
	if(m_BuffersCreated)
	{
		// Clean them
		m_IndexBuffer->Release();
	}
}

bool FlexibleIndexClass::SetBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	ID3D10Buffer* targetBuffer;

    // Set the index buffer to active in the input assembler so it can be rendered.
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return true;
}