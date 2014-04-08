#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#if defined(DEBUG) | defined(_DEBUG)

// Enable / Comment out those that you want to print / not print.
#define PRINT_MOUSE_BUTTON_EVENTS
#define PRINT_KEYBOARD_EVENTS

// These print a lot, pretty much every frame any input happens at all.
//#define PRINT_MOUSE_POSITIONS
//#define PRINT_KEYS_DOWN

#endif

#include "DXGame.h"
#include <vector>
#include <WindowsX.h>
#include <DirectXMath.h>

using namespace DirectX;

enum MouseButton
{
	LeftButton= 0,
	RightButton = 1,
	MiddleButton = 2
};

enum KeyState
{
	None = 0,
	JustPressed = 1U,
	Up = 2U,
	Down = 4U,
};

class InputManager
{
	friend class DXGame;
public:
	static const XMINT2* PreviousMouse;
	static const XMINT2* CurrentMouse;
	
	static const KeyState* LeftMouseButton;
	static const KeyState* RightMouseButton;
	static const KeyState* MiddleMouseButton;

	static bool KeyUp(unsigned char);
	static bool KeyDown(unsigned char);
	static bool KeyJustPressed(unsigned char);

	static bool MouseButtonDown(MouseButton);
	static bool MouseButtonUp(MouseButton);
	static bool MouseButtonJustPressed(MouseButton);

private:
	static bool dirty;

	static KeyState mouseButtons[3];
	static DirectX::XMINT2 previousMouse;
	static DirectX::XMINT2 currentMouse;

	static std::vector<unsigned char> keysDown;
	static unsigned char keyJustPressed;
	static unsigned char keyUp;

	static void Initialize();
	static void Update();
	static void Invalidate();

	static void ProcessInputMessage(UINT, WPARAM, LPARAM);
	static void MouseMoveMsg(WPARAM, LPARAM);
	static void MouseButtonMsg(UINT, WPARAM, LPARAM);
	static void KeyboardInputMsg(UINT, WPARAM, LPARAM);

};

typedef InputManager Input;

#endif
