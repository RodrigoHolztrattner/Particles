////////////////////////////////////////////////////////////////////////////////
// Filename: mmapclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MMAPCLASS_H_
#define _MMAPCLASS_H_

//////////////
// INCLUDES //
//////////////

#include <windows.h>


using namespace std;

///////////
// CLASS //
///////////
class MMAP
{

public:
	MMAP();
	MMAP(const MMAP&);
	~MMAP();

private:
	bool MMapFile(LPCWSTR filename);

public:

	// The map file functions
	bool MapFile(LPCWSTR filename);
	bool MapFile(const char* filename);

	// The unmap function
	bool UnmapFile();

	// The get array function
	void* GetArray();

	// The Read Memory function
	bool MemoryRead(void* dest, size_t size);

	// The set position function
	bool SetPosition(unsigned long pos);

	// The get error function
	int GetError();

public:


private:
   HANDLE f; 
   HANDLE m; 
   void *p;

   //
   int* lenght;

   // The file position variable
   unsigned long m_FilePosition;

   // The mapped variable
   bool m_FileMapped;

};
#endif
	

