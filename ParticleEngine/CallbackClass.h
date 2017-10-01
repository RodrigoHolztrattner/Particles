////////////////////////////////////////////////////////////////////////////////
// Filename: CallbackClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CallbackClass_H_
#define _CallbackClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#include "Callback.h"

/////////////
// DEFINES //
/////////////

////////////////////////////////////////////////////////////////////////////////
// Class name: CallbackClass
////////////////////////////////////////////////////////////////////////////////
class CallbackClass
{
private:

public:
	CallbackClass();
	CallbackClass(const CallbackClass&);
	~CallbackClass();

	// Set the function for this callback
	template <class CallType>
	void Set(CallType& member, void (CallType::* const &function)(void*))
	{
		// Check if we didnt already set
		if(m_Set)
		{
			return;
		}

		// Set the function
		m_Callback = makeFunctor((CBFunctor1<void*>*)0,member,function);

		// Set the bool variable
		m_Set = true;
	}

	// Set the callback
	CallbackClass(const CBFunctor1<void*> &uponClickDoThis):m_Callback(uponClickDoThis){}

	// The function that will run
	void Run(void* data){m_Callback(data);}

	// Return the set value
	bool IsSet();
	
private:

	// The callback function variable
	CBFunctor1<void*> m_Callback;

	// If the callback is set this will be true
	bool m_Set;
};

#endif
