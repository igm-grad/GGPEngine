#pragma once
#include <Windows.h>

class InputManager
{
public:
	InputManager();
	~InputManager();

	void OnMouseDown(WPARAM btnState, int x, int y) {};
	void OnMouseUp(WPARAM btnState, int x, int y) {};
	void OnMouseMove(WPARAM btnState, int x, int y) {};
};

