////////////////////////////////////////////////////////////////////////////////
// Filename: CompressClassclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMPRESSCLASS_H_
#define _COMPRESSCLASS_H_

//#define ZLIB_WINAPI

//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <zlib.h>

///////////
// CLASS //
///////////
class CompressClass
{

public:
	CompressClass();
	CompressClass(const CompressClass&);
	~CompressClass();

public: // Compress

	// The main compress function
	bool CompressMemory(unsigned char* from, unsigned int fromSize, int level = -1);

	// The secondary compress function
	bool CompressMemory(unsigned char* from, unsigned char* to, unsigned int& fromSize, int level = -1);

	// This function will return a valid size for the compress array
	unsigned long GetCompressSize(unsigned long size);

	// This will return the compressed size
	size_t GetCompressedSize();

	// This will return the compressed array
	bool GetCompressedArray(unsigned char* to);

	// This function will release the old compressed array and prepare for a new compression
	bool Release();

public: // Uncompress

	// The main uncompress function
	bool UncompressMemory(unsigned char* from, unsigned char* to, unsigned int compressedSize, unsigned int originalSize);

private: // Uncompress

private: // Compress
  
	// This will be set to true if we compressed something and we need to release it
	bool m_SCompressed;

	// The compressed array
	unsigned char* m_CompressedArray;

	// The original size
	unsigned long m_OriginalSize;

	// The size of the compressed array
	unsigned long m_CompressedArraySize;

	// The TOTAL size of the compressed array in the memory
	unsigned long m_CompressedTotalArraySize;
};
#endif
	

