///////////////////////////////////////////////////////////////////////////////
// Filename: CallbackClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "CallbackClass.h"

CallbackClass::CallbackClass()
{
	// Set to false
	m_Set = false;
}

CallbackClass::CallbackClass(const CallbackClass& other)
{
	// Copy the new data
	m_Callback = other.m_Callback;
	m_Set = other.m_Set;
}

CallbackClass::~CallbackClass()
{
}

/*
template <class CallType>
void CallbackClass::Set(CallType* member, void (CallType::* const &function)(void*))
{
	// Check if we didnt already set
	if(m_Set)
	{
		return;
	}

	// Set the function
	m_Callback = makeFunctor((CBFunctor1<void*>*)0,*member,function);

	// Set the bool variable
	m_Set = true;
}
*/

bool CallbackClass::IsSet()
{
	return m_Set;
}