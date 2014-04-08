#include "InputManager.h"

KeyState InputManager::mouseButtons[3];

const KeyState* InputManager::LeftMouseButton = &mouseButtons[0];
const KeyState* InputManager::RightMouseButton = &mouseButtons[1];
const KeyState* InputManager::MiddleMouseButton = &mouseButtons[2];

XMINT2 InputManager::previousMouse;
XMINT2 InputManager::currentMouse;
XMINT2 InputManager::deltaMouse;

const XMINT2* InputManager::PreviousMouse = &InputManager::previousMouse;
const XMINT2* InputManager::CurrentMouse = &InputManager::currentMouse;
const XMINT2* InputManager::DeltaMouse = &InputManager::deltaMouse;

std::vector<unsigned char> InputManager::keysDown;
unsigned char InputManager::keyJustPressed;
unsigned char InputManager::keyUp;

bool InputManager::dirty;

// Public stuff.
bool InputManager::KeyUp(unsigned char key)
{ return key == keyUp; }

bool InputManager::KeyJustPressed(unsigned char key)
{ return key == keyJustPressed; }

bool InputManager::KeyDown(unsigned char key)
{ return std::find(keysDown.begin(), keysDown.end(), key) != keysDown.end(); }

bool InputManager::MouseButtonDown(MouseButton mouseButton)
{ return mouseButtons[mouseButton] ^ JustPressed == Down; }

bool InputManager::MouseButtonJustPressed(MouseButton mouseButton)
{ return mouseButtons[mouseButton] == JustPressed; }

bool InputManager::MouseButtonUp(MouseButton mouseButton)
{ return mouseButtons[mouseButton] == Up; }

// Private stuff.
void InputManager::ProcessInputMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		MouseButtonMsg(msg, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		MouseMoveMsg(wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		KeyboardInputMsg(msg, wParam, lParam);

	default:
		break;
	}

	Invalidate();
}

void InputManager::MouseMoveMsg(WPARAM wParam, LPARAM lParam)
{
	previousMouse = currentMouse;
	currentMouse.x = GET_X_LPARAM(lParam);
	currentMouse.y = GET_Y_LPARAM(lParam);

	deltaMouse.x = previousMouse.x - currentMouse.x;
	deltaMouse.y = previousMouse.y - currentMouse.y;

#if defined(PRINT_MOUSE_POSITIONS)
	printf("Previous: (%i, %i), Current: (%i, %i) Delta: (%i, %i) \n", previousMouse.x, previousMouse.y, currentMouse.x, currentMouse.y, deltaMouse.x, deltaMouse.y);
#endif
}

void InputManager::MouseButtonMsg(UINT msg, WPARAM wParam, LPARAM lParam)
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

void InputManager::KeyboardInputMsg(UINT msg, WPARAM wParam, LPARAM lParam)
{
	msg ^= 0x100;

	if (msg) // key up	 
	{
		keysDown.erase( find(keysDown.begin(), keysDown.end(), wParam));
		keyUp = wParam ;
	}
	else if (std::find(keysDown.begin(), keysDown.end(), wParam) == keysDown.end())// key down, and not already down
	{ 
		keysDown.push_back(wParam); 
		keyJustPressed = wParam;
	}
#if defined(PRINT_KEYBOARD_EVENTS)
	else return; // dont print multiple keydowns.

	if (msg) printf("%c key up.\n", wParam);
	else printf("%c key down.\n", wParam);
#endif
}

void InputManager::Update()
{
	previousMouse = currentMouse;
	keyUp = 0;
	keyJustPressed = 0;
	deltaMouse.x = deltaMouse.y = 0;
	for (int i = 0; i < 3; i++)
	{ 
		if (mouseButtons[i] == JustPressed) mouseButtons[i] = Down; 
		if (mouseButtons[i] == Up) mouseButtons[i] = None; 
	}

	
	dirty = false;
}

void InputManager::Invalidate()
{ dirty = true; }