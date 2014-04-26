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
//#define PRINT_MOUSE_WHEEL_TICKS

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
	friend class DX;

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

public:

	/*
	Pointers to mouse movement variables.
	These are pointers to const XMINT2's.
	Which means they point to the values, but you cant modify them.
	*/
	static const XMINT2* PreviousMouse;
	static const XMINT2* CurrentMouse;
	static const XMINT2* DeltaMouse;

	static const short* ScrollDelta;
	
	static bool KeyUp(unsigned char);
	static bool KeyDown(unsigned char);
	static bool KeyJustPressed(unsigned char);

	static bool MouseButtonDown(MouseButton);
	static bool MouseButtonUp(MouseButton);
	static bool MouseButtonJustPressed(MouseButton);

	static void ToggleCursorVisibility();
	static void ToggleCursorLocking();
	static void SetCursorVisibility(bool);
	static void SetCursorLocking(bool);

// End Public
private:

	static bool cursorShowing;
	static bool cursorLocking;
	static RECT halfRect;
	static KeyState mouseButtons[3];
	static short scrollDelta;

	static DirectX::XMINT2 previousMouse;
	static DirectX::XMINT2 currentMouse;
	static DirectX::XMINT2 deltaMouse;

	//(fun thing about this kind of map, if the key isnt included in the map it returns false, that way we can keep its size down).
	static std::map<unsigned char, bool> keysDown;
	static unsigned char keyJustPressed;
	static unsigned char keyUp;

	static bool inValid;
	static void Validate();
	static void Invalidate();

	static void ProcessInputMessage(UINT, WPARAM, LPARAM);
	static void MouseMoveMsg(WPARAM, LPARAM);
	static void MouseButtonMsg(UINT, WPARAM, LPARAM);
	static void KeyboardInputMsg(UINT, WPARAM, LPARAM);
	static void MouseWheelMsg(WPARAM);
	static void MouseLeaveMsg();

// End Private.
};

// Convienience.
typedef InputManager Input;

#endif