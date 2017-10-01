////////////////////////////////////////////////////////////////////////////////
// Filename: FlexibleIndexClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FlexibleIndexClass_H_
#define _FlexibleIndexClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <d3dx11.h>

#include "GroupClass.h"

/////////////
// DEFINES //
/////////////

////////////////////////////////////////////////////////////////////////////////
// Class name: FlexibleIndexClass
////////////////////////////////////////////////////////////////////////////////
class FlexibleIndexClass
{
private:

public:
	FlexibleIndexClass();
	FlexibleIndexClass(const FlexibleIndexClass&);
	~FlexibleIndexClass();

	// Add
	bool Add(unsigned int* indexFrom, unsigned int quantity = 1);

	// Create buffers (and delete the data array if we want)
	bool CreateBuffers(ID3D11Device* device, bool deleteData = true);

	// Set the buffers into the pipeline
	bool SetBuffers(ID3D11DeviceContext* device);

	// Operator []
	int operator [] (unsigned int index);

	// The shutdown function
	void Shutdown();

private:

	// The base data
	bool m_DataDeleted;
	bool m_BuffersCreated;
	unsigned int m_IndexSize;

	// The data
	group<unsigned int> m_Indexes;

	// The buffers
	ID3D11Buffer* m_IndexBuffer;

public:

	// Return the buffers
	ID3D11Buffer* GetIndexBuffer(){return m_IndexBuffer;}

	// Return the sizes
	unsigned int GetIndexSize(){return m_IndexSize;}
};

#endif
