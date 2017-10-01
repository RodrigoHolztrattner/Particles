////////////////////////////////////////////////////////////////////////////////
// Filename: mmapclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "MemoryMapClass.h"

MMAP::MMAP()
{
	f=0;
	m=0;
	p=0;

	lenght = 0;

	// Zero the file position
	m_FilePosition = 0;

	// Set the file mapped to false
	m_FileMapped = false;
}


MMAP::MMAP(const MMAP& other)
{
}


MMAP::~MMAP()
{
}

bool MMAP::MapFile(LPCWSTR filename)
{
	return MMapFile(filename);
}

bool MMAP::MapFile(const char* filename)
{
	wchar_t Lfilename[100];

	// Convert to wide-char
	mbstowcs(Lfilename, filename, 100);

	return MMapFile(Lfilename);
}

// map 'filename' and return a pointer to it. fill out *length and *un if not-NULL 
bool MMAP::MMapFile(LPCWSTR filename) 
{
	f = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,  NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); // OPEN_EXISTING

	if (!f) 
	{
		return false; 
	}

	m = CreateFileMapping(f, NULL, PAGE_READWRITE, 0,0, NULL);

	if (!m) 
	{ 
		CloseHandle(f); return false; 
	} 

	p = MapViewOfFile(m, FILE_MAP_READ | FILE_MAP_WRITE, 0,0,0); 

	if (!p) 
	{ 
		CloseHandle(m); CloseHandle(f); return false; 
	}

	GetFileSize(f, NULL); // if (n) *n = 

	// Set the file mapped variable
	m_FileMapped = true;

	return true; 
} 

void* MMAP::GetArray()
{
	return p;
}

bool MMAP::UnmapFile() 
{
	if(p)
	{
		UnmapViewOfFile(p);
		p=0;
	}
	if(m)
	{
		 CloseHandle(m);
		 m=0;
	}

	if(f)
	{
		CloseHandle(f);
		f=0;
	}

	// Set the file mapped variable
	m_FileMapped = false;

	return true;
}

bool MMAP::MemoryRead(void* dest, size_t size)
{
	unsigned char* mappedArray;

	// Check if we mapped the file
	if(!m_FileMapped)
	{
		return false;
	}

	// Check the size
	if(size <= 0)
	{
		return true;
	}

	// Set the mapped array
	mappedArray = (unsigned char*)GetArray();

	// Copy
	memcpy(dest, &mappedArray[m_FilePosition], size);

	// Set the file pos
	m_FilePosition += size;

	return true;
}

bool MMAP::SetPosition(unsigned long pos)
{
	// Check if we mapped the file
	if(!m_FileMapped)
	{
		return false;
	}

	// Set the file pos
	m_FilePosition = pos;

	return true;
}

int MMAP::GetError()
{
	int error;

	// Set the error code
	error = 0;

	return error;
}