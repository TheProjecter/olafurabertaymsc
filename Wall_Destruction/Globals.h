#ifndef GLOBALS_H
#define GLOBALS_H

#include <D3DX10.h>
#include <dxerr.h>
#include "Camera.h"

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

namespace Helpers{
	class Globals
	{
	public:
		static ID3D10Device* Device;
		static float ClientWidth;
		static float ClientHeight;
		static Camera AppCamera;
		
		static const float PI;
		static const float QUART_PI;
		static const float HALF_PI;
		static const float THREE_QUARTS_PI;		
		static const float TWO_PI;

	};
};


#endif
