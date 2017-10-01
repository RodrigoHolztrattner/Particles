///////////////////////////////////////////////////////////////////////////////
// Filename: WindowClass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "WindowClass.h"

WindowClass::WindowClass()
{
}

WindowClass::WindowClass(const WindowClass& other)
{
}

WindowClass::~WindowClass()
{
}

bool WindowClass::Initialize()
{
	int screenWidth, screenHeight;

	// Set an external ptr to the window struct
	s_Window = &m_Window;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Set the window handle and hinstance at the global data
	s_Window->hwnd = m_hwnd;
	s_Window->hinstance = m_hinstance;

	// Set the width and the height
	s_Window->width = screenWidth;
	s_Window->height = screenHeight;

	// Set the main loop function at the global data
//	Data()->mainLoop = makeFunctor(&Data()->mainLoop.func,*this,&WindowClass::Run);

	return true;
}

void MapPositionScreenToClient(unsigned int& x, unsigned int& y)
{
	x = x * GetSystemDefinitions()->widthResolution / s_Window->width;
	y = y * GetSystemDefinitions()->heightResolution / s_Window->height;
}

void WindowClass::Run(CallbackClass& mainUpdate)
{
	MSG msg;
	bool done;
	POINT p;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Reset the message array
		s_Window->messages.Zero();
		
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Get the cursor position
			GetCursorPos(&p);

			// Map it to the right place
			ScreenToClient(m_hwnd, &p);

			// Set the mouse position
			s_Window->mouseX = p.x;
			s_Window->mouseY = p.y;

			// Map the screen resolution
			MapPositionScreenToClient(s_Window->mouseX, s_Window->mouseY);

			// Run the main update
			mainUpdate.Run(0);
		}
	}

	return;
}

bool WindowClass::Frame()
{
	return true;
}

LRESULT CALLBACK WindowClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	WindowMessage tempMessage;

	switch(umsg)
	{
		case WM_RBUTTONUP:
		{
			// Set the temp message data
			tempMessage.message = RIGHT_MOUSE_UP;
			tempMessage.longParam = LOWORD(lparam);
			tempMessage.shortParam = HIWORD(lparam);

			// Map the coordinates
			MapPositionScreenToClient(tempMessage.longParam, tempMessage.shortParam);

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

		case WM_RBUTTONDOWN:
		{
			// Set the temp message data
			tempMessage.message = RIGHT_MOUSE_DOWN;
			tempMessage.longParam = LOWORD(lparam);
			tempMessage.shortParam = HIWORD(lparam);

			// Map the coordinates
			MapPositionScreenToClient(tempMessage.longParam, tempMessage.shortParam);

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

		case WM_LBUTTONUP:
		{
			// Set the temp message data
			tempMessage.message = LEFT_MOUSE_UP;
			tempMessage.longParam = LOWORD(lparam);
			tempMessage.shortParam = HIWORD(lparam);

			// Map the coordinates
			MapPositionScreenToClient(tempMessage.longParam, tempMessage.shortParam);

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

		case WM_LBUTTONDOWN:
		{
			// Set the temp message data
			tempMessage.message = LEFT_MOUSE_DOWN;
			tempMessage.longParam = LOWORD(lparam);
			tempMessage.shortParam = HIWORD(lparam);

			// Map the coordinates
			MapPositionScreenToClient(tempMessage.longParam, tempMessage.shortParam);

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

		case WM_KEYDOWN:
		{
			// Set the temp message data
			tempMessage.message = KEY_PRESS;
			tempMessage.longParam = wparam;
			tempMessage.shortParam = lparam;

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

		case WM_KEYUP:
		{
			// Set the temp message data
			tempMessage.message = KEY_RELEASE;
			tempMessage.longParam = wparam;
			tempMessage.shortParam = lparam;

			// Put this message at the message array
			s_Window->messages.Add(tempMessage);

			break;
		}

	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void WindowClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Set the position of the window to the top left corner.
	posX = posY = 0;
	
	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(GetSystemDefinitions()->fullScreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
/*	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 800;
		screenHeight = 600;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}
*/
	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(true);

	return;
}

void WindowClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(GetSystemDefinitions()->fullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

//////////////////////
// GLOBAL FUNCTIONS //
//////////////////////

Window* GetSystemWindow()
{
	return s_Window;
}