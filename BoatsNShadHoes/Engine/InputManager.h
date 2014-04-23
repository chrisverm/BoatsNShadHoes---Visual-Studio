#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Windows.h>
#include <map>
#include <WindowsX.h>
#include <DirectXMath.h>

// Debug Macros.
#if defined(DEBUG) | defined(_DEBUG)

// Enable / Comment out those that you want to print / not print.
#define PRINT_MOUSE_BUTTON_EVENTS
#define PRINT_KEYBOARD_EVENTS

// These print a lot, pretty much every frame any input happens at all.
//#define PRINT_MOUSE_POSITIONS
//#define PRINT_KEYS_DOWN

#endif

/*
Enumeration of the 3 default mouse buttons "MB."

LeftMB (left click).
RightMB (right click).
MiddleMB (scroll wheel click).
*/
enum MouseButton
{
	LeftMB= 0,
	RightMB = 1,
	MiddleMB = 2
};

using namespace DirectX;

class InputManager
{
private:

/*
Enumeration of the different states a key/button can be in.
Right now only used with Mouse buttons
*/
enum KeyState
{
	None = 0,
	JustPressed = 1U,
	Up = 2U,
	Down = 4U 
};
	friend class DXGame;
	friend class DX;
public:

	/*
	Pointers to mouse movement variables.
	These are pointers to const XMINT2's.
	Which means they point to the values, but you cant modify them.
	*/
	static const XMINT2* PreviousMouse;
	static const XMINT2* CurrentMouse;
	static const XMINT2* DeltaMouse;
	
	/*
	Gets whether the specfied unsigned char -> key is up/down/justPressed.
	*/
	static bool KeyUp(unsigned char);
	static bool KeyDown(unsigned char);
	static bool KeyJustPressed(unsigned char);

	/*
	Gets whether the specified mouse button is up/down/justPressed.
	*/
	static bool MouseButtonDown(MouseButton);
	static bool MouseButtonUp(MouseButton);
	static bool MouseButtonJustPressed(MouseButton);

	static void ToggleCursor();
	static void SetCursor(bool);

// End Public
private:

	static bool cursorShowing;
	static RECT halfRect;

	/*
	Holds data about the 3 mouse buttons.
	*/
	static KeyState mouseButtons[3];

	/*
	Mouse data.
	Current and previous positions of the mouse, and a delta for when the two are different.
	*/
	static DirectX::XMINT2 previousMouse;
	static DirectX::XMINT2 currentMouse;
	static DirectX::XMINT2 deltaMouse;

	/*
	Map of what keys are down.
	(fun thing about this kind of map, if the key isnt included in the map it returns false, that way we can keep its size down).
	*/
	static std::map<unsigned char, bool> keysDown;
	static unsigned char keyJustPressed;
	static unsigned char keyUp;

	/*
	When Invalid, Validate will be called at the end of the next frame.
	Validate updates "previous" states (should only called in DXGame), (sets previous mouse to current, etc etc).
	*/
	static bool inValid;
	static void Validate();
	static void Invalidate();

	/*
	Handle/Process all the system messages.
	*/
	static void ProcessInputMessage(UINT, WPARAM, LPARAM);
	static void MouseMoveMsg(WPARAM, LPARAM);
	static void MouseButtonMsg(UINT, WPARAM, LPARAM);
	static void KeyboardInputMsg(UINT, WPARAM, LPARAM);

// End Private.
};

// Convienience.
typedef InputManager Input;

#endif