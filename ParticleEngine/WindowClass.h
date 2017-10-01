////////////////////////////////////////////////////////////////////////////////
// Filename: WindowClass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WindowClass_H_
#define _WindowClass_H_


/////////////
// LINKING //
/////////////
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <iostream>
#include <stdio.h>

#include "GroupClass.h"
#include "CallbackClass.h"
#include "DefinitionsClass.h"
#include "GUI_UserMessages.h"

// #include "Global.h"
// #include "GUI_UserMessages.h"

/////////////
// DEFINES //
/////////////

/////////////
// STRUCTS //
/////////////

// The window message struct type
struct WindowMessage
{
	// The message
	unsigned int message;

	// The long param
	unsigned int longParam;

	// The short param
	unsigned int shortParam;
};

// The window struct type
struct Window
{
	// The window size
	unsigned int width, height;

	// The window histance
	HINSTANCE hinstance;

	// The window handle
	HWND hwnd;

	// The mouse position
	unsigned int mouseX, mouseY;

	// The window message array
	group<WindowMessage> messages;
};

////////////////////////////////////////////////////////////////////////////////
// Class name: WindowClass
////////////////////////////////////////////////////////////////////////////////
class WindowClass
{
private:

public:
	WindowClass();
	WindowClass(const WindowClass&);
	~WindowClass();

	// The initialize function
	bool Initialize();

	// The message handler for our window
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	// The main loop starter
	void Run(CallbackClass& mainUpdate);

private:
	
	bool Frame();
	bool HandleInput(float);
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:

	// Our window data
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

/////////////////////
private: // GLOBAL //
/////////////////////

	// The window struct
	Window m_Window;
};

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

// Return the window struct
Window* GetSystemWindow();
#define SystemWindow GetSystemWindow()

//////////////////////
// GLOBAL VARIABLES //
//////////////////////

// The window struct
static Window* s_Window;

/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/////////////
// GLOBALS //
/////////////
static WindowClass* ApplicationHandle = 0;

#endif
