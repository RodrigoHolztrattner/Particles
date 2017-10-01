////////////////////////////////////////////////////////////////////////////////
// Filename: FileManagerClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "FileManagerClass.h"

FileManagerClass::FileManagerClass(unsigned int type)
{
	// Zero the file map data
	m_File = 0;
	m_MappedFile = 0;
	m_MappedView = 0;

	// Zero the file position
	m_FilePosition = 0;

	// Set the file mapped to false
	m_FileOpen = false;

	// Set the manager type
	m_ManagerType = type;
}

FileManagerClass::FileManagerClass()
{
	// Zero the file map data
	m_File = 0;
	m_MappedFile = 0;
	m_MappedView = 0;

	// Zero the file position
	m_FilePosition = 0;

	// Set the file mapped to false
	m_FileOpen = false;

	// Set the manager type
	m_ManagerType = -1;
}

FileManagerClass::FileManagerClass(const FileManagerClass& other)
{
}


FileManagerClass::~FileManagerClass()
{
}

void FileManagerClass::SetType(unsigned int type)
{
	// Set the manager type
	m_ManagerType = type;
}

bool FileManagerClass::Open(const char* filename)
{
	int error;
	bool result;

	// Check the manager type
	if(m_ManagerType == FILE_MAP)
	{
		// Map the file
		result = FileMap(filename);
		if(!result)
		{
			return false;
		}
	}
	else if(m_ManagerType == FILE_NORMAL)
	{
		// Open the file in read binary mode
		error = fopen_s(&m_FilePtr, filename, "rb");
		if(error == 0)
		{
			return false;
		}
	}
	else if(m_ManagerType == FILE_WRITE)
	{
		// Open the file in write binary mode
		error = fopen_s(&m_FilePtr, filename, "wb");
		if(error == 0)
		{
			return false;
		}
	}

	// Set the file open to true
	m_FileOpen = true;

	return true;
}

bool FileManagerClass::Close()
{
	bool result;

	// Check the manager type
	if(m_ManagerType == FILE_NORMAL || m_ManagerType == FILE_WRITE)
	{
		// Read the file
		result = fclose(m_FilePtr);
		if(!result)
		{
			return false;
		}

		// Do a fflush
		fflush(m_FilePtr);
	}
	else if(m_ManagerType == FILE_MAP)
	{
		// Unmap the file
		result = UnmapFile();
		if(!result)
		{
			return false;
		}
	}

	// Zero the file position
	m_FilePosition = 0;

	// Set the file mapped variable
	m_FileOpen = false;
}

bool FileManagerClass::FileMap(const char* filename)
{
	/////////////////////
	// Windows Filemap //
	/////////////////////
	#ifdef _WIN32 ///////
	/////////////////////

	wchar_t Lfilename[100];

	// Convert to wide-char
	mbstowcs(Lfilename, filename, 100);

	// Open the file
	m_File = CreateFile(Lfilename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,  NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!m_File)
	{
		return false;
	}

	// Map the data inside the file
	m_MappedFile = CreateFileMapping(m_File, NULL, PAGE_READWRITE, 0,0, NULL);
	if (!m_MappedFile)
	{
		CloseHandle(m_File); return false;
	}

	// Map the view
	m_MappedView = MapViewOfFile(m_MappedFile, FILE_MAP_READ | FILE_MAP_WRITE, 0,0,0);
	if (!m_MappedView)
	{
		CloseHandle(m_MappedFile); CloseHandle(m_File); return false;
	}

	GetFileSize(m_File, NULL);

	/////////////////////////
	// End Windows Filemap //
	/////////////////////////
	#endif //////////////////
	/////////////////////////

	return true;
}

bool FileManagerClass::MemoryRead(void* dest, size_t size)
{
	unsigned char* mappedArray;
	bool result;

	// Check the manager type
	if(m_ManagerType == FILE_WRITE)
	{
		return false;
	}

	// Check if we mapped the file
	if(!m_FileOpen)
	{
		return false;
	}

	// Check the size
	if(size <= 0)
	{
		return true;
	}

	// Check the manager type
	if(m_ManagerType == FILE_NORMAL)
	{
		// Read the file
		result = fread(dest, size, 1, m_FilePtr);
		if(!result)
		{
			return false;
		}
	}
	else if(m_ManagerType == FILE_MAP)
	{
		// Set the mapped array
		mappedArray = (unsigned char*)GetArray();

		// Copy
		memcpy(dest, &mappedArray[m_FilePosition], size);
	}

	// Set the file pos
	m_FilePosition += size;

	return true;
}


bool FileManagerClass::MemoryWrite(void* from, size_t size)
{
	bool result;

	// Check the manager type
	if(m_ManagerType != FILE_WRITE)
	{
		return false;
	}

	// Check if we mapped the file
	if(!m_FileOpen)
	{
		return false;
	}

	// Check the size
	if(size <= 0)
	{
		return true;
	}

	// Write into the file
	result = fwrite(from, size, 1, m_FilePtr);
	if(!result)
	{
		return false;
	}
}

void* FileManagerClass::GetArray()
{
	// Check the manager type
	if(m_ManagerType != FILE_MAP)
	{
		return 0;
	}

	return m_MappedView;
}

bool FileManagerClass::UnmapFile()
{
	/////////////////////
	// Windows Filemap //
	/////////////////////
	#ifdef _WIN32 ///////
	/////////////////////

	// Unmap the view
	if(m_MappedView)
	{
		UnmapViewOfFile(m_MappedView);
		m_MappedView=0;
	}

	// Unmap the mapped data
	if(m_MappedFile)
	{
		 CloseHandle(m_MappedFile);
		 m_MappedFile=0;
	}

	// Close the file
	if(m_File)
	{
		CloseHandle(m_File);
		m_File=0;
	}

	/////////////////////////
	// End Windows Filemap //
	/////////////////////////
	#endif //////////////////
	/////////////////////////

	return true;
}

bool FileManagerClass::SetPosition(unsigned long pos)
{
	// Check if we mapped the file
	if(!m_FileOpen)
	{
		return false;
	}

	// Check the manager type
	if(m_ManagerType == FILE_WRITE)
	{
		return false;
	}

	// Check the manager type
	if(m_ManagerType == FILE_NORMAL)
	{
		// Set the position inside the file
		fseek(m_FilePtr, pos, SEEK_SET);
	}
	else if(m_ManagerType == FILE_MAP)
	{
		// ...
		// ...
	}

	// Set the file pos
	m_FilePosition = pos;

	return true;
}
