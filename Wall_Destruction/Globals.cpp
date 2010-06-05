#include "Globals.h"

namespace Helpers{
	ID3D10Device* Globals::Device;
	float Globals::ClientWidth;
	float Globals::ClientHeight;
	Camera Globals::AppCamera;
	bool Globals::CORRECTLY_SETUP = true;
	HWND Globals::Window;

	InfoText Globals::information;


	const float Globals::PI = D3DX_PI;
	const float Globals::QUART_PI = PI*0.25f;
	const float Globals::HALF_PI = PI*0.5f;
	const float Globals::THREE_QUARTS_PI = PI*0.75f;		
	const float Globals::TWO_PI = 2*PI;
}