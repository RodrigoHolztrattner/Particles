////////////////////////////////////////////////////////////////////////////////
// Filename: DefinitionsClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DefinitionsClass_H_
#define _DefinitionsClass_H_

/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////
#include "MemoryMapClass.h"

/////////////
// DEFINES //
/////////////

/////////////
// STRUCTS //
/////////////

// The definitions struct
struct Definitions
{
	// The resolution
	unsigned int widthResolution, heightResolution;

	// The full screen variable
	bool fullScreen;

	// The vsync variable
	bool vsync;

	// The screen near and depth
	float screenNear, screenDepth;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: DefinitionsClass
////////////////////////////////////////////////////////////////////////////////
class DefinitionsClass
{
private:

public:
	DefinitionsClass();
	DefinitionsClass(const DefinitionsClass&);
	~DefinitionsClass();

	bool Initialize();

/////////////////////
private: // GLOBAL //
/////////////////////

	// The definitions struct
	Definitions m_Definitions;

};

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// Return the definitions struct
Definitions* GetSystemDefinitions();
#define SystemDefinitions GetSystemDefinitions()

//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// The definitions data
static Definitions* s_Definitions;

#endif
