#include "Globals.h"

using namespace Drawables;

namespace Helpers{
	// directx stuff
	ID3D10Device* Globals::Device;
	int Globals::ClientWidth;
	int Globals::ClientHeight;
	ID3D10RenderTargetView* Globals::RenderTargetView;
	ID3D10DepthStencilView* Globals::DepthStencilView;
	HWND Globals::Window;

	// application stuff
	Camera Globals::AppCamera;
	Light Globals::AppLight;
	bool Globals::CORRECTLY_SETUP = true;
	bool Globals::MOUSE_ACQUIRED = false; // set as false, so the mouse gets acquired in the beginning
	bool Globals::MOVE_WRECKINGBALL = false;
	bool Globals::DRAW_OCTREE = false;

	InfoText Globals::DebugInformation;
	DrawMethod Globals::SurfelDrawMethod = SOLID;
	DrawMethod Globals::SurfelRenderMethod = SOLID;

	// constants
	const float Globals::PI = (float)D3DX_PI;
	const float Globals::QUART_PI = PI*0.25f;
	const float Globals::HALF_PI = PI*0.5f;
	const float Globals::THREE_QUARTS_PI = PI*0.75f;		
	const float Globals::TWO_PI = 2*PI;
}