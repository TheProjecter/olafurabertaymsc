#include "MouseHandler.h"
#include <dinput.h>
#include "Globals.h"

#define SAMPLE_BUFFER_SIZE  16

DIMOUSESTATE MouseHandler::MouseState;  // mouse state
LPDIRECTINPUTDEVICE MouseHandler::MouseDevice;    // the pointer to the mouse device	

bool MouseHandler::SetUp(HINSTANCE hInstance, HWND hwnd){

	LPDIRECTINPUT di = NULL; 
	HRESULT             hr; 

	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, NULL);    


	hr = di->CreateDevice(GUID_SysMouse, &MouseDevice, NULL);
	if (FAILED(hr)) { 
		::MessageBox(0, "Failed to create the mouse device!", 0, 0);
		return false;
	}
	hr = MouseDevice->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) { 
		::MessageBox(0, "Failed to set the mouse data format!", 0, 0);
		return false;
	}
	hr = MouseDevice->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) { 
		::MessageBox(0, "Failed to set the cooperative level!",	0, 0);
		return false;
	}

	HANDLE g_hMouseEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (g_hMouseEvent == NULL) {
		::MessageBox(0, "Failed to create mouse event!", 0, 0);
		return false;
	}

	hr = MouseDevice->SetEventNotification(g_hMouseEvent);

	if (FAILED(hr)) { 
		::MessageBox(0, "Failed to set event notification!",0, 0);
		return false;
	}


	DIPROPDWORD dipdw;
	// the header
	dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj        = 0;
	dipdw.diph.dwHow        = DIPH_DEVICE;
	// the data
	dipdw.dwData            = SAMPLE_BUFFER_SIZE;

	hr = MouseDevice->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

	if (FAILED(hr)) {
		return FALSE;
	}
	di->Release();
	return true;
}

void MouseHandler::CleanUp(){
	MouseDevice->Unacquire();    // make sure the mouse in unacquired
	MouseDevice->Release();		
}

void MouseHandler::DetectInput(){

	memset(&MouseState, 0, sizeof(DIMOUSESTATE)); 
	if (!MouseDevice) return; 

	HRESULT hr = MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState); 

	if(FAILED(hr)){
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) 
		{ 
			// device lost... reacquire 
			hr = MouseDevice->Acquire(); 
			if (!FAILED(hr)) 
			{ 
				MouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &MouseState);
			}
		} 
	}
}

bool MouseHandler::IsButtonPressed(int button){
	if(MouseState.rgbButtons[button] & 0x80)
		return true;

	return false;
}
