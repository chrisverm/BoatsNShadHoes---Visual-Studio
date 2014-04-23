#include "InputManager.h"

Input::KeyState InputManager::mouseButtons[3];

XMINT2 InputManager::previousMouse = XMINT2(-1, -1);
XMINT2 InputManager::currentMouse = XMINT2(-1, -1);
XMINT2 InputManager::deltaMouse = XMINT2(0, 0);

const XMINT2* InputManager::PreviousMouse = &InputManager::previousMouse;
const XMINT2* InputManager::CurrentMouse = &InputManager::currentMouse;
const XMINT2* InputManager::DeltaMouse = &InputManager::deltaMouse;

std::map<unsigned char, bool> InputManager::keysDown;
unsigned char InputManager::keyJustPressed;
unsigned char InputManager::keyUp;

bool InputManager::inValid;
bool InputManager::cursorShowing;

RECT InputManager::halfRect;

// Public getters.
bool InputManager::KeyUp(unsigned char key)
{ return key == keyUp; }

bool InputManager::KeyJustPressed(unsigned char key)
{ return key == keyJustPressed; }

bool InputManager::KeyDown(unsigned char key)
{ return keysDown[key]; }

bool InputManager::MouseButtonDown(MouseButton mouseButton)
{ return (mouseButtons[mouseButton] | JustPressed) == (Down | JustPressed); }

bool InputManager::MouseButtonJustPressed(MouseButton mouseButton)
{ return mouseButtons[mouseButton] == JustPressed; }

bool InputManager::MouseButtonUp(MouseButton mouseButton)
{ return mouseButtons[mouseButton] == Up; }

// MSG Proc
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
	currentMouse.x = GET_X_LPARAM(lParam);
	currentMouse.y = GET_Y_LPARAM(lParam);

	if (previousMouse.x < 0 || previousMouse.y < 0)
		previousMouse = currentMouse;

	deltaMouse.x = previousMouse.x - currentMouse.x;
	deltaMouse.y = previousMouse.y - currentMouse.y;
	
#if defined(PRINT_MOUSE_POSITIONS)
	printf("Previous: (%i, %i), Current: (%i, %i) Delta: (%i, %i) \n", previousMouse.x, previousMouse.y, currentMouse.x, currentMouse.y, deltaMouse.x, deltaMouse.y);
#endif
	previousMouse = currentMouse;
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
		keysDown.erase(wParam);
		keyUp = wParam ;
	}
	else if (!keysDown[wParam]) // key down, and not held
	{ 
		keysDown[wParam] = true;
		keyJustPressed = wParam;
	}
#if defined(PRINT_KEYBOARD_EVENTS)
	else return; // dont print multiple keydowns. (part of the above if-else)

	if (msg) printf("%c key up.\n", wParam);
	else printf("%c key down.\n", wParam);
#endif
}

// Validation
void InputManager::Validate()
{
	previousMouse = currentMouse;
	keyUp = 0;
	keyJustPressed = 0;
	
	// If cursor showing, and mouse has moved.
	if (!cursorShowing && (deltaMouse.x != 0 || deltaMouse.y != 0))
	{
		// This will trigger a mousemove msg.
		SetCursorPos(halfRect.right, halfRect.bottom);

		// Set this to reset when the mousemove msg is recieved.
		// If < 0, treated as though it hasnt been created yet.
		previousMouse.x = -1;
	}
	
	for (int i = 0; i < 3; i++)
	{ 
		if (mouseButtons[i] == JustPressed) mouseButtons[i] = Down; 
		if (mouseButtons[i] == Up) mouseButtons[i] = None; 
	}
	
	deltaMouse.x = deltaMouse.y = 0;
	inValid = false;
}

void InputManager::Invalidate()
{ inValid = true; }

// Cursor.
void InputManager::ToggleCursor()
{ ShowCursor(cursorShowing = !cursorShowing); }

void InputManager::SetCursor(bool showCursor)
{ ShowCursor(cursorShowing = showCursor); }