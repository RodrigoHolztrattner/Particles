///////////////////////////////////////////////////////////////////////////////
// Filename: LoadQueueObject.cpp
///////////////////////////////////////////////////////////////////////////////
#include "LoadQueueClass.h"

LoadQueueObject::LoadQueueObject()
{
	// Set load done to false
	m_LoadDone = false;
}

LoadQueueObject::LoadQueueObject(const LoadQueueObject& other)
{
	// Copy the data
	m_LoadDone = other.m_LoadDone;
}

LoadQueueObject::~LoadQueueObject()
{
	// Check if the object was created
//	assert(m_LoadDone);
}

LoadQueueClass::LoadQueueClass()
{
	// Set busy to false
	m_Busy = false;
}

LoadQueueClass::LoadQueueClass(const LoadQueueClass& other)
{
}

LoadQueueClass::~LoadQueueClass()
{
}

void LoadQueueClass::AddNew(LoadQueueObject* object, LoadQueueData data)
{
	// Add it to the list
	m_QueueList.Add(LoadQueueInfo(object, data));
}

void LoadQueueClass::Update()
{
	pthread_t newThread;
	int result;

	// Check if we have any data to load (and if we are not loading anything yet)
	if(m_QueueList.Size() > 0 && !m_Busy)
	{
		// Set busy to true
		m_Busy = true;

		// Create a new thread to load the data
		result = pthread_create(&newThread, NULL, _LoadQueueUpdate, this);
	}
}

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

void* _LoadQueueUpdate(void* ptr)
{
	LoadQueueClass* loadQueue;

	// Cast the ptr
	loadQueue = (LoadQueueClass*)ptr;

	// Run the loop until we finish loading
	while(loadQueue->m_QueueList.Size() > 0)
	{
		// Call the load function for this object
		loadQueue->m_QueueList[0].object->Load(loadQueue->m_QueueList[0].data);

		// Set load done to true
		loadQueue->m_QueueList[0].object->m_LoadDone = true;

		// Remove this object
		loadQueue->m_QueueList.Remove(0);
	}

	// Set the budy variable at the load queue to false
	loadQueue->m_Busy = false;

	return 0;
}

