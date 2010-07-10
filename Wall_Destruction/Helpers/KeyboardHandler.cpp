#include "KeyboardHandler.h"
#include "Globals.h"

namespace Helpers{
	
	LPDIRECTINPUT8 KeyboardHandler::directInput;
	LPDIRECTINPUTDEVICE8 KeyboardHandler::directInputDevice;
	char KeyboardHandler::KeyBuffer[256], KeyboardHandler::LastKeyBuffer[256];
	bool KeyboardHandler::isReady = false;

	void KeyboardHandler::Init()
	{
		directInput = NULL;
		directInputDevice= NULL;

		InitDirectInput();
		InitKeyboard();
		isReady = true;
	}

	void KeyboardHandler::CleanUp()
	{
		if(directInputDevice != NULL)
		{
			directInputDevice->Unacquire();
			directInputDevice->Release();
			directInputDevice= NULL;
		}

		if(directInput != NULL)
		{
			directInput->Release();
			directInput = NULL;
		}
	}

	void KeyboardHandler::InitDirectInput()
	{
		if(FAILED(DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&directInput,NULL)))
		{
			MessageBox(NULL, "DirectInput8Create() failed!","InitDirectInput()",MB_OK);
			Globals::CORRECTLY_SETUP = false;
		}
	}

	void KeyboardHandler::InitKeyboard(){

		if(FAILED(directInput->CreateDevice(GUID_SysKeyboard,&directInputDevice,NULL)))
		{
			MessageBox(NULL,"CreateDevice() failed!","InitKeyboard()",MB_OK);
			Globals::CORRECTLY_SETUP = false;
		}
		if(FAILED(directInputDevice->SetDataFormat(&c_dfDIKeyboard)))
		{
			MessageBox(NULL,"SetDataFormat() failed!","InitKeyboard()",MB_OK);
			Globals::CORRECTLY_SETUP = false;
		}
		if(FAILED(directInputDevice->SetCooperativeLevel(Globals::Window, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		{
			MessageBox(NULL,"SetCooperativeLevel() failed!","InitKeyboard()",MB_OK);
			Globals::CORRECTLY_SETUP = false;
		}
		if(FAILED(directInputDevice->Acquire()))
		{
			MessageBox(NULL,"Acquire() failed!","InitKeyboard()",MB_OK);
			Globals::CORRECTLY_SETUP = false;
		}
	}
	
	void KeyboardHandler::Update()
	{
		memcpy(LastKeyBuffer, KeyBuffer, sizeof(LastKeyBuffer)); 

		if(FAILED(directInputDevice->GetDeviceState(sizeof(KeyBuffer),(LPVOID)&KeyBuffer)))
		{
			MessageBox(Globals::Window,"GetDeviceState() failed!","Update()",MB_OK);			
		}
	}

	bool KeyboardHandler::IsKeyDown(int key){
		return (KeyBuffer[key] & 0x80) != 0;
	}

	bool KeyboardHandler::IsKeyUp(int key){
		return !IsKeyDown(key);
	}

	bool KeyboardHandler::IsSingleKeyDown(int key){
		return ((KeyBuffer[key] & 0x80) != 0) && !((LastKeyBuffer[key] & 0x80) != 0);
	}

	bool KeyboardHandler::IsSingleKeyUp(int key){
		return !((KeyBuffer[key] & 0x80) != 0) && ((LastKeyBuffer[key] & 0x80) != 0) ;
	}
}

