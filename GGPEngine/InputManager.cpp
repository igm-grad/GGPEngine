#include "InputManager.h"

InputManager::InputManager()
{
	keysDown = new unordered_set<KeyCode>();
	keysUp = new unordered_set<KeyCode>();
	keysPressed = new unordered_set<KeyCode>();
	
	prevMouseState = 0;
	currMouseState = 0;
}

InputManager::~InputManager()
{
	delete keysDown;
	delete keysUp;
	delete keysPressed;
}

#pragma region Input Checkers

bool InputManager::GetKeyDown(KeyCode key)
{
	return keysDown->find(key) != keysDown->end();
}

bool InputManager::GetKey(KeyCode key)
{
	return GetKeyDown(key) || keysPressed->find(key) != keysPressed->end();
}

bool InputManager::GetKeyUp(KeyCode key)
{
	return keysUp->find(key) != keysUp->end();
}

bool InputManager::GetMouseButtonDown(MouseButton button)
{
	bool wasNotPressed = (prevMouseState & button) == 0;
	bool isPressed = (currMouseState & button) == button;

	return wasNotPressed && isPressed;
}

bool InputManager::GetMouseButtonUp(MouseButton button)
{
	bool wasPressed = (prevMouseState & button) == button;
	bool isNotPressed = (currMouseState & button) == 0;

	return wasPressed && isNotPressed;
}

bool InputManager::GetMouseButton(MouseButton button)
{
	bool isPressed = (currMouseState & button) == button;

	return isPressed;
}

#pragma endregion


#pragma region Input Processing

void InputManager::Flush()
{
	// persist all keys pressed at this frame.
	for (const KeyCode &key : *keysDown)
	{
		keysPressed->insert(key);
	}

	// clear keys down and up
	keysDown->clear();
	keysUp->clear();

	// copy current mouse state to previous mouse state
	prevMouseState = currMouseState;
}

KeyCode InputManager::KeyCodeFromWParam(WPARAM wParam)
{
	// any exception from wParam value to keyCode value
	// must be done here
	switch (wParam)
	{
	default:
		return static_cast<KeyCode>(wParam);
	}
}

void InputManager::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	// if the previous state is set to 1
	// the keydown event is recurrent
	// and there is no need for processing it
	if (lParam & PREV_KEY_STATE_BIT)
		return;

	KeyCode code = KeyCodeFromWParam(wParam);

	keysDown->insert(code);
}

void InputManager::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	KeyCode code = KeyCodeFromWParam(wParam);

	// remove the key from pressed keys
	keysPressed->erase(code);

	// and add it to up keys
	keysUp->insert(code);
}

void InputManager::OnMouseDown(MouseButton button, WPARAM wParam, LPARAM lParam)
{
	if (button == MOUSEBUTTON_X)
	{
		// GET_XBUTTON_WPARAM returns n for XBUTTON n
		short xButton = GET_XBUTTON_WPARAM(wParam);
		
		// shift xButton to 4 bits to the left
		// before adding to curr state
		currMouseState |= xButton << 4;
	}
	else 
	{
		currMouseState |= button;
	}
}

void InputManager::OnMouseUp(MouseButton button, WPARAM wParam, LPARAM lParam)
{
	if (button == MOUSEBUTTON_X)
	{
		// GET_XBUTTON_WPARAM returns n for XBUTTON n
		short xButton = GET_XBUTTON_WPARAM(wParam);

		// shift xButton to 4 bits to the left
		// before adding to curr state
		currMouseState &= ~(xButton << 4);
	}
	else
	{
		currMouseState &= ~button;
	}
}

void InputManager::OnMouseMove(WPARAM wParam, LPARAM lParam) 
{
	mousePosition.x = GET_X_LPARAM(lParam);
	mousePosition.y = GET_Y_LPARAM(lParam);
}

#pragma endregion
