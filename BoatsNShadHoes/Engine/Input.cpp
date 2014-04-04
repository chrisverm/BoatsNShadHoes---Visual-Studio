#include "Input.h"

KeyState Input::mouseButtons[3];

XMINT2 Input::previousMouse;
XMINT2 Input::currentMouse;

const XMINT2* Input::PreviousMouse = &Input::previousMouse;
const XMINT2* Input::CurrentMouse = &Input::currentMouse;

std::vector<unsigned char> Input::keysDown;
unsigned char Input::keyJustPressed;
unsigned char Input::keyUp;

bool Input::dirty;
// Public stuff.

bool Input::KeyUp(unsigned char key)
{ return key == keyUp; }

bool Input::KeyJustPressed(unsigned char key)
{ return key == keyJustPressed; }

bool Input::KeyDown(unsigned char key)
{ return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end(); }

// Private stuff.

void Input::ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		MouseButton(msg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		MouseMove(wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		KeyboardInput(msg, wParam, lParam);

	default:
		break;
	}

	Invalidate();
}

void Input::MouseMove(WPARAM wParam, LPARAM lParam)
{
	previousMouse = currentMouse;
	currentMouse.x = GET_X_LPARAM(lParam);
	currentMouse.y = GET_Y_LPARAM(lParam);

#if defined(PRINT_MOUSE_POSITIONS)
	printf("Previous - (%f, %f), Current - (%f, %f) \n", previousMouse.x, previousMouse.y, currentMouse.x, currentMouse.y);
#endif
}

void Input::MouseButton(UINT msg, WPARAM wParam, LPARAM lParam)
{
	msg ^= 0x0200;

	mouseButtons[msg/3] = KeyState(msg%3);

#if defined(PRINT_MOUSE_BUTTON_EVENTS)
	int button = msg/3;
	if (button == 0) printf("Left Mouse Button");
	else if (button == 1) printf("Right Mouse Button");
	else if (button == 2) printf("Middle Mouse Button");
		
	if (mouseButtons[button] == Up) printf(" is now up.\n");
	if (mouseButtons[button] == JustPressed) printf(" was just pressed.\n");

	XMINT2 blah;
	blah.x = GET_X_LPARAM(lParam);
	blah.y = GET_Y_LPARAM(lParam);

	if (blah.x != currentMouse.x || blah.y != currentMouse.y ) printf("the fuck?\n");
#endif
}

void Input::KeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	msg ^= 0x100;

	if (msg) // key up	 
	{
		keysDown.erase( find(keysDown.begin(), keysDown.end(), wParam));
		keyUp = wParam ;
	}
	else if (std::find(keysDown.begin(), keysDown.end(), wParam) == keysDown.end())// key down.
	{ 
		keysDown.push_back(wParam); 
		keyJustPressed = wParam;
	}

#if defined(PRINT_KEYBOARD_EVENTS)
	if (msg) printf("%c key up.\n", wParam);
	else printf("%c key down.\n", wParam);
#endif
}

void Input::Update()
{
	previousMouse = currentMouse;
	keyUp = 0;
	keyJustPressed = 0;
	dirty = false;
}

void Input::Invalidate()
{ dirty = true; }