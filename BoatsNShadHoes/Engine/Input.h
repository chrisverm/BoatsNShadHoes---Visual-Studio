#ifndef INPUT_H
#define INPUT_H

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

enum KeyState
{
	None = 0,
	JustPressed = 1U,
	Up = 2U,
	Down = 4U,
};

class Input
{
	friend class DXGame;
public:
	static const XMFLOAT2* PreviousMouse;
	static const XMFLOAT2* CurrentMouse;

	static bool KeyUp(unsigned char);
	static bool KeyDown(unsigned char);
	static bool KeyJustPressed(unsigned char);

private:
	static void Initialize();

	static KeyState mouseButtons[3];
	static DirectX::XMFLOAT2 previousMouse;
	static DirectX::XMFLOAT2 currentMouse;

	static std::vector<unsigned char> keysDown;
	static unsigned char keyJustPressed;
	static unsigned char keyUp;

	static LRESULT MouseInput(UINT, WPARAM, LPARAM);
	static LRESULT KeyboardInput(UINT, WPARAM, LPARAM);

	static void MouseUpdate();
};

#endif
