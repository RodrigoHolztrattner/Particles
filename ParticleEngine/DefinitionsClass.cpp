///////////////////////////////////////////////////////////////////////////////
// Filename: DefinitionsClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "DefinitionsClass.h"


DefinitionsClass::DefinitionsClass()
{
}

DefinitionsClass::DefinitionsClass(const DefinitionsClass& other)
{
}

DefinitionsClass::~DefinitionsClass()
{
}

bool DefinitionsClass::Initialize()
{
	bool result;

	// Set an external ptr to the definitions struct
	s_Definitions = &m_Definitions;

	s_Definitions->fullScreen = true;
	s_Definitions->heightResolution = 900;
	s_Definitions->widthResolution = 1600;
	s_Definitions->screenDepth = 1000.0f;
	s_Definitions->screenNear = 1.0f;
	s_Definitions->vsync = false;

	s_Definitions->fullScreen = false;

	return true;
}

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

Definitions* GetSystemDefinitions()
{
	return s_Definitions;
}