//=============================================================================
// Init Direct3D.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates the sample framework by initializing Direct3D, clearing 
// the screen, and displaying frame stats.
//
//=============================================================================

#include "d3dApp.h"
#include "d3dUtil.h"
#include "Globals.h"
#include "CustomRectangle.h"
#include "MouseHandler.h"

using namespace Drawables;

class WallDestructionApp : public D3DApp
{
public:
	WallDestructionApp(HINSTANCE hInstance);
	~WallDestructionApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	CustomRectangle rectangle;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	WallDestructionApp theApp(hInstance);

	theApp.initApp();
		
	return theApp.run();
}

WallDestructionApp::WallDestructionApp(HINSTANCE hInstance)
	: D3DApp(hInstance, "Wall Destruction - Masters Project - Olafur Thor Gunnarsson - 0900128") 
{
}

WallDestructionApp::~WallDestructionApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	MouseHandler::CleanUp();
	rectangle.CleanUp();
}

void WallDestructionApp::initApp()
{
	D3DApp::initApp();

	Helpers::Globals::Device = md3dDevice;
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::AppCamera = Camera(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	rectangle = CustomRectangle(20.0f, 20.0f, D3DXCOLOR(0.0f, 255.0f, 0.0f, 255.0f));
	rectangle.Init();

	MouseHandler::SetUp(mhAppInst, mhMainWnd);
}

void WallDestructionApp::onResize()
{
	D3DApp::onResize();
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::AppCamera.Reset();
}

void WallDestructionApp::updateScene(float dt)
{
	MouseHandler::DetectInput();

	#ifdef _DEBUG
		D3DApp::updateScene(dt);
	#endif // _DEBUG	

	Helpers::Globals::AppCamera.Update(dt);
}

void WallDestructionApp::drawScene()
{
	D3DApp::drawScene();

#ifdef _DEBUG
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawTextA(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, RED);
#endif // _DEBUG	

	rectangle.Draw();

	mSwapChain->Present(0, 0);
}
