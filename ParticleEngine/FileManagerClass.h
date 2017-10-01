////////////////////////////////////////////////////////////////////////////////
// Filename: FileManagerClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FileManagerClass_H_
#define _FileManagerClass_H_

//////////////
// INCLUDES //
//////////////

#include <windows.h>
#include <stdio.h>

/////////////
// DEFINES //
/////////////
#define FILE_NORMAL			0
#define FILE_MAP			1
#define FILE_WRITE			2

using namespace std;

///////////
// CLASS //
///////////
class FileManagerClass
{

public:
	FileManagerClass(unsigned int type);
	FileManagerClass();
	FileManagerClass(const FileManagerClass&);
	~FileManagerClass();

public:

	// Set the type
	void SetType(unsigned int type);

	// Open the file
	bool Open(const char* filename);

	// Close the file
	bool Close();

	// The Read Memory function
	bool MemoryRead(void* dest, size_t size);

	// The memory write function
	bool MemoryWrite(void* from, size_t size);

	// The set position function
	bool SetPosition(unsigned long pos);

	// The get array function
	void* GetArray();

private:

	// The file map function
	bool FileMap(const char* filename);

	// The unmap function
	bool UnmapFile();

////////////////////////////////
private: // Filemap Variables //
////////////////////////////////

	///////////////////////////////
	// Windows Filemap Variables //
	///////////////////////////////
	#ifdef _WIN32 /////////////////
	///////////////////////////////

	// The file and the mapped data
	HANDLE m_File;
	HANDLE m_MappedFile;
	
	///////////////////////////////////
	// End Windows Filemap Variables //
	///////////////////////////////////
	#endif ////////////////////////////
	///////////////////////////////////

	// The mapped view (the array)
	void* m_MappedView;

////////////////////////////////////
private: // File Normal Variables //
////////////////////////////////////

	// The file variable
	FILE* m_FilePtr;

private:

	// The manager type
	unsigned int m_ManagerType;

	// The current position inside the file
	unsigned long m_FilePosition;

	// This will be true if the file is open
	bool m_FileOpen;
};
#endif
	

