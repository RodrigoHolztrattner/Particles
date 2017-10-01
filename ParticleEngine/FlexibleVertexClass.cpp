///////////////////////////////////////////////////////////////////////////////
// Filename: FlexibleVertexClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "FlexibleVertexClass.h"

FlexibleVertexClass::FlexibleVertexClass(unsigned int size)
{
	// Set the unity size
	m_UnitSize = size;

	// Zero the vertex type
	m_VertexType = 0;

	// Set the data deleted to false
	m_DataDeleted = false;

	// Set the buffers created to false
	m_BuffersCreated = false;

	// Zero the sizes
	m_VertexSize = 0;
	m_IndexSize = 0;
}

FlexibleVertexClass::FlexibleVertexClass()
{
	// Zero the vertex type
	m_VertexType = 0;

	// Set the data deleted to false
	m_DataDeleted = false;

	// Set the buffers created to false
	m_BuffersCreated = false;

	// Zero the sizes
	m_VertexSize = 0;
	m_IndexSize = 0;
}

FlexibleVertexClass::FlexibleVertexClass(const FlexibleVertexClass& other)
{
}

FlexibleVertexClass::~FlexibleVertexClass()
{
}

void FlexibleVertexClass::SetUnitSize(unsigned int size)
{
	m_UnitSize = size;
}

bool FlexibleVertexClass::Add(unsigned char* vertexFrom, unsigned int* indexFrom, unsigned int quantity)
{
	// Check if we dont deleted the data
	if(m_DataDeleted)
	{
		return false;
	}

	// Check if the unity size is zero
	if(!m_UnitSize)
	{
		return false;
	}

	// Add the vertexes
	m_Vertexes.Add(vertexFrom, quantity * m_UnitSize);

	// Add the indexes
	m_Indexes.Add(indexFrom, quantity);

	// Set the sizes
	m_VertexSize = m_Vertexes.Size();
	m_IndexSize = m_Indexes.Size();

	return true;
}

unsigned char* FlexibleVertexClass::operator [] (unsigned int index)
{
	// Check if we dont deleted the data
	if(m_DataDeleted)
	{
		return false;
	}

	return &(m_Vertexes.GetArray())[index * m_UnitSize];
}

void FlexibleVertexClass::SetVertexType(unsigned int type)
{
	m_VertexType = type;
}

unsigned int FlexibleVertexClass::GetVertexType()
{
	return m_VertexType;
}

bool FlexibleVertexClass::CreateBuffers(ID3D10Device* device, bool deleteData)
{
	bool result;
	unsigned int vertexSize, indexSize;
	D3D10_BUFFER_DESC vertexBufferDesc;
	D3D10_BUFFER_DESC indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData;
	D3D10_SUBRESOURCE_DATA indexData;
	HRESULT Hresult;

	// Set the vertex size
	vertexSize = m_Vertexes.Size();

	// Set the index size
	indexSize = sizeof(unsigned int) * m_Indexes.Size();

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = vertexSize;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_Vertexes.GetArray();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
		
	// Now finally create the vertex buffer.
	Hresult = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if(FAILED(Hresult))
	{
		return false;
	}
	
	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
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
		// Clean them
		m_Vertexes.Zero();
		m_Indexes.Zero();

		// Set the deleted data to true
		m_DataDeleted = true;
	}

	return true;
}

void FlexibleVertexClass::Shutdown()
{
	// See if we need to delete the data
	if(!m_DataDeleted)
	{
		// Clean it
		m_Vertexes.Zero();
		m_Indexes.Zero();
	}

	// See if we need to delete the buffers
	if(m_BuffersCreated)
	{
		// Clean them
		m_VertexBuffer->Release();
		m_IndexBuffer->Release();
	}
}

bool FlexibleVertexClass::SetBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;
	ID3D10Buffer* targetBuffer;

	// Set the buffer strides.
	stride = m_UnitSize;

	// Set the target buffer to be the real vertex buffer
	targetBuffer = m_VertexBuffer;
	
	// Set the buffer offsets.
	offset = 0;
	
	// Set the vertex and instance buffers to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &targetBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    device->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}