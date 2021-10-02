#pragma once
#include "EngineVariables.h"
#include "InputVariables.h"
class WindowContainer : virtual public EngineVariables, virtual public InputVariables
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


private:
};