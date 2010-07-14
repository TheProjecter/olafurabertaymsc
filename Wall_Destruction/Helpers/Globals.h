#ifndef GLOBALS_H
#define GLOBALS_H

#include <D3DX10.h>
#include <dxerr.h>
#include "Camera.h"
#include <sstream>
#include "InfoText.h"
#include "Light.h"

#ifdef _UNICODE
#define _T(x) x
#else 
#define _T(x)  L##x
#endif 

//*****************************************************************************
// Simple d3d error checker for book demos.
//*****************************************************************************

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
		DXTraceW(__FILE__, (DWORD)__LINE__, hr, DXGetErrorDescriptionW(hr), true); \
		}                                                      \
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef ERR
	#define ERR(x, errorString)	\
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			std::stringstream out;							\
			out << "Error - file: " << __FILE__ << " - line: " << (DWORD) __LINE__;	\
			MessageBox(0, ((std::string)errorString).c_str(), out.str().c_str(), 0);             \
		}                                                      \
	}
	#endif
#else
	#ifndef ERR
	#define ERR(x, errorString) (x)
	#endif
#endif

// begin - these definitions are taken from GS-NPatch-Tesselation
#define STR(x) #x
#define DEF_STR(x) STR(x)
#define DEF_MACRO(x) { #x, DEF_STR(x) }
// end 

using namespace Drawables;

namespace Helpers{

	enum DrawMethod {SOLID, WIREFRAME, SURFEL};

	class Globals
	{
	public:
		static ID3D10Device* Device;
		static int ClientWidth;
		static int ClientHeight;
		static Camera AppCamera;
		static HWND Window;

		static InfoText DebugInformation;
		static DrawMethod SurfelDrawMethod;
		static DrawMethod SurfelRenderMethod;
		static Light AppLight;
		
		static const float PI;
		static const float QUART_PI;
		static const float HALF_PI;
		static const float THREE_QUARTS_PI;		
		static const float TWO_PI;

		static bool CORRECTLY_SETUP;
		static bool MOUSE_ACQUIRED;
		static bool MOVE_WRECKINGBALL;
		static bool DRAW_PHYXEL_GRID;
		static bool DRAW_PHYXELS;
		static bool SHOW_DEBUG;
		static bool SHOW_INFO;
		static bool SHOW_ERRORS;
		static ID3D10RenderTargetView* RenderTargetView;
		static ID3D10DepthStencilView* DepthStencilView;
	};
};


#endif
