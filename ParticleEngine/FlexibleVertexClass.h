////////////////////////////////////////////////////////////////////////////////
// Filename: FlexibleVertexClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FlexibleVertexClass_H_
#define _FlexibleVertexClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "GroupClass.h"

/////////////
// DEFINES //
/////////////

////////////////////////////////////////////////////////////////////////////////
// Class name: FlexibleVertexClass
////////////////////////////////////////////////////////////////////////////////
class FlexibleVertexClass
{
private:

public:
	FlexibleVertexClass(unsigned int size);
	FlexibleVertexClass();
	FlexibleVertexClass(const FlexibleVertexClass&);
	~FlexibleVertexClass();

	// Add
	bool Add(unsigned char* vertexFrom, unsigned int* indexFrom, unsigned int quantity = 1);

	// Set the unit size
	void SetUnitSize(unsigned int size);

	// Set the vertex type
	void SetVertexType(unsigned int type);

	// Return the vertex type
	unsigned int GetVertexType();

	// Create buffers (and delete the data array if we want)
	bool CreateBuffers(ID3D10Device* device, bool deleteData = true);

	// Set the buffers into the pipeline
	bool SetBuffers(ID3D10Device* device);

	// Operator []
	unsigned char* operator [] (unsigned int index);

	// The shutdown function
	void Shutdown();

private:

	// The base data
	unsigned int m_UnitSize;
	unsigned int m_VertexType;
	bool m_DataDeleted;
	bool m_BuffersCreated;
	unsigned int m_VertexSize;
	unsigned int m_IndexSize;

	// The data
	group<unsigned char> m_Vertexes;
	group<unsigned int> m_Indexes;

	// The buffers
	ID3D10Buffer* m_VertexBuffer;
	ID3D10Buffer* m_IndexBuffer;

public:

	// Return the buffers
	ID3D10Buffer* GetVertexBuffer(){return m_VertexBuffer;}
	ID3D10Buffer* GetIndexBuffer(){return m_IndexBuffer;}

	// Return the sizes
	unsigned int GetVertexSize(){return m_VertexSize;}
	unsigned int GetIndexSize(){return m_IndexSize;}
};

#endif
