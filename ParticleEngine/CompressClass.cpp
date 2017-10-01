////////////////////////////////////////////////////////////////////////////////
// Filename: CompressClassclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CompressClass.h"

CompressClass::CompressClass()
{
	// Zero the values
	m_SCompressed = false;
	m_OriginalSize = 0;
	m_CompressedArraySize = 0;
	m_CompressedTotalArraySize = 0;
	m_CompressedArray = 0;
}


CompressClass::CompressClass(const CompressClass& other)
{
}


CompressClass::~CompressClass()
{
}

bool CompressClass::CompressMemory(unsigned char* from, unsigned int fromSize, int level)
{
	HRESULT result;

	// Check if we need to release first
	if(m_SCompressed)
	{
		return false;
	}

	// Check if we got a valid from array
	if(!from)
	{
		return false;
	}

	// Set the original size
	m_OriginalSize = fromSize;

	// Get the total size that we need to alocate for this compression
	m_CompressedTotalArraySize = GetCompressSize(fromSize);

	// Set the compressed size to be the same as the total compressed size
	m_CompressedArraySize = m_CompressedTotalArraySize;

	// Alocate space for the compression
	m_CompressedArray = new unsigned char[m_CompressedTotalArraySize];
	if(!m_CompressedArray)
	{
		return false;
	}

	// Compress
	result = compress2(m_CompressedArray, &m_CompressedArraySize, from, m_OriginalSize, level);
	if(result != S_OK)
	{
		return false;
	}

	// Set bool variable
	m_SCompressed = true;

	return true;
}

bool CompressClass::CompressMemory(unsigned char* from, unsigned char* to, unsigned int& fromSize, int level)
{
	unsigned long originalSize;
	unsigned long neededSize;
	unsigned long compressedSize;
	HRESULT result;

	// Check if we got a valid from array
	if(!from)
	{
		return false;
	}

	// Check if we got a valid go array
	if(!to)
	{
		return false;
	}

	// Set the size values
	originalSize = fromSize;
	compressedSize = originalSize;
	neededSize = GetCompressSize(fromSize);

	// Compress
	result = compress2(to, &compressedSize, from, originalSize, level);
	if(result != S_OK)
	{
		return false;
	}

	// Set the compressed size
	fromSize = compressedSize;

	return true;
}

bool CompressClass::UncompressMemory(unsigned char* from, unsigned char* to, unsigned int compressedSize, unsigned int originalSize)
{
	HRESULT result;
	unsigned long CSize;

	// Check if we got a valid from array
	if(!from)
	{
		return false;
	}

	// Check if we got a valid go array
	if(!to)
	{
		return false;
	}

	// Set the original size
	CSize = originalSize;

	// Uncompress
	result = uncompress(to, &CSize, from, compressedSize);
	if(result != S_OK)
	{
		return false;
	}

	return true;
}

size_t CompressClass::GetCompressedSize()
{
	size_t size;

	// Check if we compressed something
	if(!m_SCompressed)
	{
		return false;
	}

	// Set the size
	size = (size_t)m_CompressedArraySize;

	return size;
}

bool CompressClass::GetCompressedArray(unsigned char* to)
{
	// Check if we compressed something
	if(!m_SCompressed)
	{
		return false;
	}

	// Check if we got a valid to
	if(!to)
	{
		return false;
	}

	// Memcpy the array
	memcpy(to, m_CompressedArray, m_CompressedArraySize);

	return true;
}

bool CompressClass::Release()
{
	// Check if we compressed something
	if(!m_SCompressed)
	{
		return false;
	}

	// Delete the old array
	if(m_CompressedArray)
	{
		delete [] m_CompressedArray;
		m_CompressedArray = 0;
	}

	// Zero the values
	m_SCompressed = false;
	m_OriginalSize = 0;
	m_CompressedArraySize = 0;
	m_CompressedTotalArraySize = 0;
	m_CompressedArray = 0;

	return true;
}

unsigned long CompressClass::GetCompressSize(unsigned long size)
{
	return (size * 1.1) + 12;
}