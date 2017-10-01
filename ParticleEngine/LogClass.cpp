///////////////////////////////////////////////////////////////////////////////
// Filename: LogClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "LogClass.h"

LogClass::LogClass()
{
	// Set the global debug log
	s_Log = this;
}

LogClass::LogClass(const LogClass& other)
{
}

LogClass::~LogClass()
{
}

void LogClass::operator<<(const char* data)
{
#ifdef _DEBUG_LOG

    // Call the write function
	Write(data, strlen(data));

#endif

    return;
}

void LogClass::Erase()
{
	int error;
	bool result;
	FILE* m_FilePtr;

	// Open the file in write binary mode
	error = fopen_s(&m_FilePtr, LOG_FILENAME, "wb");
	if(error == 0)
	{
//		return;
	}

	// Read the file
	result = fclose(m_FilePtr);
	if(!result)
	{
		return;
	}
}

void LogClass::Write(const char* data, unsigned int size)
{
	int error;
	bool result;
	FILE* m_FilePtr;
	char enter = '\n';

	// Open the file in write binary mode
	error = fopen_s(&m_FilePtr, LOG_FILENAME, "a+");
	if(error == 0)
	{
//		return;
	}

	// Write into the file
	result = fwrite(data, size, 1, m_FilePtr);
	if(!result)
	{
		return;
	}

	// Write into the file
	result = fwrite(&enter, 1, 1, m_FilePtr);
	if(!result)
	{
		return;
	}

	// Read the file
	result = fclose(m_FilePtr);
	if(!result)
	{
		return;
	}
}

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// Return the log object
LogClass* GetDebugLog()
{
	return s_Log;
}