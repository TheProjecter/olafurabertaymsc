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
#include "SurfelObject.h"
//#include "CustomRectangle.h"
#include "InfoText.h"
#include "MouseHandler.h"
#include "KeyboardHandler.h"
#include <ctime>

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
	SurfelObject surfelObject;
	string instructions;
	RECT instructionRect;
//	CustomRectangle testRectangle;
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
	instructionRect.left = 5;
	instructionRect.right = 0;
	instructionRect.top = 35;
	instructionRect.bottom = 0;
	
	instructions = "F1 - Surfel representation\nF2 - Solid representation\nF3 - Wireframe representation\nSpace - Randomize surfels\nWASD - Move camera\nMouse - Change camera's direction";
	srand((unsigned)time(0));
} 

WallDestructionApp::~WallDestructionApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	Helpers::MouseHandler::CleanUp();
	Helpers::KeyboardHandler::CleanUp();
	surfelObject.CleanUp();
	Helpers::Globals::information.CleanUp();
	//testRectangle.CleanUp();
}

void WallDestructionApp::initApp()
{
	D3DApp::initApp();

	Helpers::Globals::Device = md3dDevice;
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::Window = mhMainWnd;
	Helpers::Globals::AppCamera = Camera(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	
	Helpers::MouseHandler::SetUp(mhAppInst, mhMainWnd);
	Helpers::KeyboardHandler::Init();

	surfelObject.SetDrawMethod(SURFEL);

	surfelObject.Init();

	Helpers::Globals::information = InfoText(this->mClientWidth / 2.0f - 50, 5);

	/*testRectangle = CustomRectangle(10, 10, D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f));
	testRectangle.Init();*/
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
	Helpers::MouseHandler::DetectInput();
	Helpers::KeyboardHandler::Update();
	Helpers::Globals::information.Update(dt);
	Helpers::Globals::AppCamera.Update(dt);
	
	#ifdef _DEBUG
		D3DApp::updateScene(dt);
	#endif 
		
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F1) && surfelObject.GetDrawMethod() != SURFEL){ 
		Helpers::Globals::information.AddText("Setting SurfelObject.Drawmethod to SURFEL");
		surfelObject.SetDrawMethod(SURFEL);
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F2) && surfelObject.GetDrawMethod() != SOLID){
		Helpers::Globals::information.AddText("Setting SurfelObject.Drawmethod to SOLID");
		surfelObject.SetDrawMethod(SOLID);
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F3) && surfelObject.GetDrawMethod() != WIREFRAME){
		Helpers::Globals::information.AddText("Setting SurfelObject.Drawmethod to WIREFRAME");
		surfelObject.SetDrawMethod(WIREFRAME);
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_SPACE)){
		Helpers::Globals::information.AddText("Randomizing surfels", RED);
		surfelObject.RandomizeSurfels();
	}

}

void WallDestructionApp::drawScene()
{
	D3DApp::drawScene();

#ifdef _DEBUG
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawTextA(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, RED);
#endif // _DEBUG	

	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	mFont->DrawTextA(0, instructions.c_str(), -1, &instructionRect, DT_NOCLIP, GREEN);

	surfelObject.Draw();
	Helpers::Globals::information.Draw();
	//testRectangle.Draw();

	mSwapChain->Present(0, 0);
}
