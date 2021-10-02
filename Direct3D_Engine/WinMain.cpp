#include "Engine.h"


INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
	//srand(static_cast <unsigned> (time(0)));
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	if (engine.Initialize(hInstance, "DXEngine", "MyWindowClass", window_width, window_height))
	{
		while (engine.ProcessMessages() == true)
		{
			engine.Update(window_width, window_height);
		}
	}
	return 0;
}

