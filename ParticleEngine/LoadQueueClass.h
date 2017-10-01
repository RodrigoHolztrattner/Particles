////////////////////////////////////////////////////////////////////////////////
// Filename: LoadQueueObject.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LoadQueueObject_H_
#define _LoadQueueObject_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#include <assert.h>
#include <pthread.h>
#include "GroupClass.h"

/////////////
// DEFINES //
/////////////

/////////////
// STRUCTS //
/////////////

// The load queue data struct
struct LoadQueueData
{
	LoadQueueData(){};
	LoadQueueData(char* _filename, unsigned int _position, unsigned int _compressedSize, unsigned int _totalSize)
	{
		strcpy(filename, _filename);
		position = _position;
		compressedSize = _compressedSize;
		totalSize = _totalSize;
	}
	LoadQueueData(const LoadQueueData& other)
	{
		// Copy the data
		strcpy(filename, other.filename);
		position = other.position;
		compressedSize = other.compressedSize;
		totalSize = other.totalSize;
	}

	// The filename
	char filename[25];

	// The position inside the file
	unsigned int position;

	// The compressed size
	unsigned int compressedSize;

	// The total size (real size)
	unsigned int totalSize;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: LoadQueueObject
////////////////////////////////////////////////////////////////////////////////
class LoadQueueObject
{
private:

public:
	LoadQueueObject();
	LoadQueueObject(const LoadQueueObject&);
	~LoadQueueObject();

	// Call the load function
	virtual void Load(LoadQueueData data) = 0;

public:

	// If the load is done this will be true
	bool m_LoadDone;

private:

};

////////////////////////////////////////////////////////////////////////////////
// Class name: LoadQueueClass
////////////////////////////////////////////////////////////////////////////////
class LoadQueueClass
{
private:

	// The load queue info struct
	struct LoadQueueInfo
	{
		LoadQueueInfo(){};
		LoadQueueInfo(LoadQueueObject* _object, LoadQueueData _data)
		{
			object = _object;
			data = _data;
		}

		// The object
		LoadQueueObject* object;

		// The object data
		LoadQueueData data;
	};

public:
	LoadQueueClass();
	LoadQueueClass(const LoadQueueClass&);
	~LoadQueueClass();

	// Add a new item to the load queue
	void AddNew(LoadQueueObject* object, LoadQueueData data);

	// Do the update loop for this queue so we can load the queued data
	void Update();

public:

	// If we are loading anything this will be true
	bool m_Busy;

	// The queue list
	group<LoadQueueInfo> m_QueueList;
};

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// This function will run in a different thread and load the queued data
void* _LoadQueueUpdate(void* ptr);

#endif