////////////////////////////////////////////////////////////////////////////////
// Filename: GUI_UserMessages.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GUI_UserMessages_H_
#define _GUI_UserMessages_H_


/////////////
// LINKING //
/////////////

//////////////
// INCLUDES //
//////////////

/////////////
// DEFINES //
/////////////

// MOUSE //

#define MOUSE_HOVER					0
#define RIGHT_MOUSE_UP				1
#define RIGHT_MOUSE_DOWN			2
#define LEFT_MOUSE_UP				3
#define LEFT_MOUSE_DOWN				4
#define RIGHT_CLICK					5
#define LEFT_CLICK					6

#define RIGHT_UP					1 << 0
#define RIGHT_DOWN					1 << 1
#define LEFT_UP						1 << 2
#define LEFT_DOWN					1 << 3

// BASE //

#define TRIGGERING_BASE_MESSAGE		15

#define TRIGGERONFOCUS				18
#define TRIGGERLOSTFOCUS			19
#define ONFOCUS						20
#define LOSTFOCUS					21
#define CREATE						22
#define ERASE						23

// KEYBOARD //

#define KEY_PRESS					30
#define KEY_RELEASE					31

#define HALF_CLICK(x)				(x == RIGHT_MOUSE_UP || x == RIGHT_MOUSE_DOWN || x == LEFT_MOUSE_UP || x == LEFT_MOUSE_DOWN)
#define MOUSE_CLICK(x)				(x == RIGHT_CLICK || x == LEFT_CLICK || HALF_CLICK(x))
#define MOUSE_MESSAGE(x)			((x == MOUSE_HOVER) || (x == RIGHT_MOUSE_UP) || (x == RIGHT_MOUSE_DOWN) || (x == LEFT_MOUSE_UP) || (x == LEFT_MOUSE_DOWN) || (x == RIGHT_CLICK) || (x == LEFT_CLICK))

#endif
