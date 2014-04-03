#include "Input.h"

KeyState Input::mouseButtons[3];

XMFLOAT2 Input::previousMouse;
XMFLOAT2 Input::currentMouse;

const XMFLOAT2* Input::PreviousMouse = &Input::previousMouse;
const XMFLOAT2* Input::CurrentMouse = &Input::currentMouse;

std::vector<unsigned char> Input::keysDown;
unsigned char Input::keyJustPressed;
unsigned char Input::keyUp;

// Public stuff.

bool Input::KeyUp(unsigned char key)
{ return key == keyUp; }

bool Input::KeyJustPressed(unsigned char key)
{ return key == keyJustPressed; }

bool Input::KeyDown(unsigned char key)
{ return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end(); }

// Private stuff.

LRESULT Input::MouseInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg != WM_MOUSEMOVE)
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
#endif
	}

	previousMouse = currentMouse;
	currentMouse.x = GET_X_LPARAM(lParam);
	currentMouse.y = GET_Y_LPARAM(lParam);

#if defined(PRINT_MOUSE_POSITIONS)
	printf("Previous - (%f, %f), Current - (%f, %f) \n", previousMouse.x, previousMouse.y, currentMouse.x, currentMouse.y);
#endif

	return 0;
}

LRESULT Input::KeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
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

	return 0;
}

void Input::MouseUpdate()
{
	previousMouse = currentMouse;
}