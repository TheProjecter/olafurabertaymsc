/*
	Implementation taken from http://www.two-kings.de/tutorials/dinput/dinput02.html
*/
#ifndef KEYBOARD_HELPER_H
#define KEYBOARD_HELPER_H

#include <dinput.h>
#include <D3DX10.h>

namespace Helpers{

	class KeyboardHandler
	{
	public:
		static void Init();
		static void CleanUp();

		static void Update();
		static bool IsKeyUp(int);
		static bool IsKeyDown(int);
		static bool IsSingleKeyDown(int);
		static bool IsSingleKeyUp(int);

	private:
		static void InitDirectInput();
		static void InitKeyboard();

		static LPDIRECTINPUT8 directInput;
		static LPDIRECTINPUTDEVICE8 directInputDevice;

		static char KeyBuffer[256], LastKeyBuffer[256];
	};
}

#endif