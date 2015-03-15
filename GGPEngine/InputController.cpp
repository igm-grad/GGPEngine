#include "InputController.h"
#include <Windows.h>

InputController::~InputController()
{
	keyBindingMap.empty();
}

void InputController::registerKeyBinding(int key, function<void()> binding)
{
	keyBindingMap[key] = binding;
}

void InputController::executeKeyBindings()
{
	for (const auto &keyBinding : keyBindingMap) {
		if (GetAsyncKeyState(keyBinding.first) & 0x8000) {
			keyBinding.second();
		}
	}
}

