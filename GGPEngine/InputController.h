#pragma once
#include <map>
#include <functional>

using namespace std;

class InputController
{
public:
	map<int, function<void()>>	keyBindingMap;

	static InputController& sharedInstance()
	{
		static InputController sharedInstance;
		return sharedInstance;
	}

	InputController(){};
	~InputController();
	InputController(InputController const&)		= delete;
	void operator=(InputController const&)		= delete;

	void registerKeyBinding(int key, function<void()> binding);
	void executeKeyBindings();
};

