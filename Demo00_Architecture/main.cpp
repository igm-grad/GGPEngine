#include "../GGPEngine/CoreEngine.h"
#include <stdlib.h>

#include <Windows.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>

#pragma comment(lib, "mono-2.0.lib")

#pragma region Win32 Entry Point (WinMain)

// Win32 Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Config mono
	mono_set_dirs("../external/mono/lib", "../external/mono/etc");
	mono_config_parse(NULL);

	// Load mono domain (need to run v4.0 to run .net framework 4.5 code)
	MonoDomain* domain		= mono_jit_init_version("Demo00", "v4.0");
	MonoAssembly* assembly	= mono_domain_assembly_open(domain, "Demo00_Assembly.dll");
	MonoImage* image		= mono_assembly_get_image(assembly);

	// Get method to execute from image
	MonoMethodDesc* methodDesc	= mono_method_desc_new("GPPEngine.Library:Greet(string)", true);
	MonoMethod* method			= mono_method_desc_search_in_image(methodDesc, image);

	// create arg list
	void* args[1];
	args[0] = mono_string_new(domain, "GPP Engine");

	// invoke method
	MonoString* result = (MonoString*)mono_runtime_invoke(method, NULL, args, NULL);
	char* str = mono_string_to_utf8(result);

	// print results
	printf("%s", str);

	CoreEngine* e = new CoreEngine(hInstance, prevInstance, cmdLine, showCmd);

	e->Initialize();

	Mesh* m = e->createMesh("..\\models\\sphere.obj");

	GameObject* o = e->createGameObject();
	o->mesh = m;

	MSG msg = { 0 };
	// Loop until we get a quit message from windows
	while (msg.message != WM_QUIT)
	{
		// Peek at the next message (and remove it from the queue)
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			// Handle this message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			e->Update();
		}
	}


}

#pragma endregion