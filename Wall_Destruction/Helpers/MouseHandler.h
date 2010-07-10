/*
	Implementation of mouse handling taken from http://www.directxtutorial.com/Tutorial9/E-DirectInput/dx9E2.aspx
*/
#ifndef MOUSE_HANDLER_H
#define MOUSE_HANDLER_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>

namespace Helpers{
	class MouseHandler
	{
	public:
		static bool SetUp(HINSTANCE hInstance, HWND hwnd);	
		static void CleanUp();
		static void DetectInput();
		static bool IsButtonPressed(int button);
	
		static const int MOUSE_LEFTBUTTON  = 0;
		static const int MOUSE_RIGHTBUTTON =  1;
		static const int MOUSE_MIDDLEBUTTON=  2;
		static const int MOUSE_4BUTTON      = 3;
		static const int MOUSE_5BUTTON      = 4;
		static const int MOUSE_6BUTTON      = 5;
		static const int MOUSE_7BUTTON      = 6;
		static const int MOUSE_8BUTTON      = 7;

		static LPDIRECTINPUTDEVICE MouseDevice;
		static DIMOUSESTATE MouseState;
	};
}

#endif