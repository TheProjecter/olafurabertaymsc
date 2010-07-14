//=============================================================================
// The main application. Uses the Luna DirectX10 framework.
//
//=============================================================================
#include "d3dApp.h"
#include "d3dUtil.h"

#ifdef _DEBUG
#ifdef MEMORY_LEAK_CHECK
//#include "vld/vld.h"
#endif
#endif

// Keycode
#include <Common/Base/keycode.cxx>

// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES 
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

#include "PhysicsWrapper.h"
#define DIRECTINPUT_VERSION 0x0800


#include "Globals.h"
#include "ChangedPhyxels.h"
#include "Projectiles.h"
#include "InfoText.h"
#include "MouseHandler.h"
#include "KeyboardHandler.h"
#include "RandRange.h"
#include "WreckingBall.h"
#include "CSGTree.h"
#include "SpriteViewPort.h"
#include <ctime>
#include <map>
#include "DrawableTex2D.h"

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
	void Reset();
	void CleanApp();

private:
	string instructions;
	RECT instructionRect;

	MeshlessObject *ground;
	MeshlessObject *wall;

	Drawables::WreckingBall wreckingBall;
	Projectiles projectiles;
	bool resetLastFrame;

	DrawableTex2D DepthPass, AttributePass, NormalizationPass;
	SpriteViewPort depthMapVP, normalMapVP, occlusionMapVP;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	WallDestructionApp theApp(hInstance);
	

	theApp.initApp();

	if(!Helpers::Globals::CORRECTLY_SETUP)
		PostQuitMessage(0);
		
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
}

void WallDestructionApp::CleanApp(){

	wreckingBall.CleanUp();
	projectiles.CleanUp();

	Helpers::MouseHandler::CleanUp();
	Helpers::KeyboardHandler::CleanUp();

	Helpers::Globals::DebugInformation.CleanUp();

	ChangedPhyxels::CleanUp();

	PhysicsWrapper::CleanUp();

	ground->CleanUp();
	wall->CleanUp();

	delete wall;
	delete ground;
}

void WallDestructionApp::Reset(){

	Helpers::Globals::DebugInformation.StartTimer();

	wall->CleanUp();
	ground->CleanUp();

	delete wall;
	delete ground;

	wreckingBall.CleanUp();
	projectiles.CleanUp();
	ChangedPhyxels::CleanUp();
	PhysicsWrapper::CleanUp();

	PhysicsWrapper::Init();

	// get all the meshless objects 
	wall = new MeshlessObject("MeshlessObjects\\Wall.xml");
	ground = new MeshlessObject("MeshlessObjects\\Ground.xml");

	// create the wreckingball
	wreckingBall = WreckingBall(5.0f);
	wreckingBall.Init();
	projectiles.Init();

	PhysicsWrapper::FinishInit();

	ChangedPhyxels::Init();

	Helpers::Globals::DebugInformation.EndTimer(INFO_TYPE, "Restart ");
	resetLastFrame = true;
}

void WallDestructionApp::initApp()
{
	D3DApp::initApp();

	Helpers::Globals::Device = md3dDevice;
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::Window = mhMainWnd;
	Helpers::Globals::AppCamera = Camera();	
	Helpers::Globals::AppLight.Init();	
	Helpers::Globals::DebugInformation = InfoText(this->mClientWidth / 2 - 50, 5);

	Helpers::MouseHandler::SetUp(mhAppInst, mhMainWnd);
	Helpers::KeyboardHandler::Init();
	PhysicsWrapper::Init();
	ChangedPhyxels::Init();
	
	
	instructionRect.left = this->mClientWidth - 50;
	instructionRect.right = 0;
	instructionRect.top = 5;
	instructionRect.bottom = 0;

	instructions = "(H)elp";
	srand((unsigned)time(0));

	// get all the meshless objects 
	ground = new MeshlessObject("MeshlessObjects\\Ground.xml");
	wall = new MeshlessObject("MeshlessObjects\\Wall.xml");

	// create the wreckingball
	wreckingBall = WreckingBall(5.0f);
	wreckingBall.Init();

	projectiles.Init();

	PhysicsWrapper::FinishInit();

	resetLastFrame = false;
}

void WallDestructionApp::onResize()
{
	D3DApp::onResize();
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::RenderTargetView = this->mRenderTargetView;
	Helpers::Globals::DepthStencilView = this->mDepthStencilView;
	Helpers::Globals::Device = md3dDevice;
	Helpers::Globals::AppCamera.Reset();

	Helpers::Globals::DebugInformation.SetRect(this->mClientWidth / 2 - 50, 5);

	instructionRect.left = this->mClientWidth - 50;
	instructionRect.right = 0;
	instructionRect.top = 5;
	instructionRect.bottom = 0;	
}

void WallDestructionApp::updateScene(float dt)
{
	if(resetLastFrame){
		resetLastFrame = false;
		return;
	}

	Helpers::MouseHandler::DetectInput();
	Helpers::KeyboardHandler::Update();
	
	Helpers::Globals::DebugInformation.Update(dt);
	
	Helpers::Globals::AppCamera.Update(dt);
	
	PhysicsWrapper::Update(dt);

	wall->Update(dt);
	ground->Update(dt);

	wreckingBall.Update(dt);
	projectiles.Update(dt);

	ChangedPhyxels::Update(dt);
	
	D3DApp::updateScene(dt);
	
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F1)){
		if(Helpers::Globals::SurfelRenderMethod == Helpers::SURFEL){
			Helpers::Globals::DebugInformation.AddText(ALWAYS, "Setting SurfelObject.RenderMethod to SOLID");
			Helpers::Globals::SurfelRenderMethod = Helpers::SOLID;
		}
		else if(Helpers::Globals::SurfelRenderMethod == Helpers::SOLID){
			Helpers::Globals::DebugInformation.AddText(ALWAYS, "Setting SurfelObject.RenderMethod to WIREFRAME");
			Helpers::Globals::SurfelRenderMethod = Helpers::WIREFRAME;
		}
		else{
			Helpers::Globals::DebugInformation.AddText(ALWAYS, "Setting SurfelObject.RenderMethod to SURFEL");
			Helpers::Globals::SurfelRenderMethod = Helpers::SURFEL;
		}		
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F5)){
		Helpers::Globals::SHOW_DEBUG = !Helpers::Globals::SHOW_DEBUG;
	
		if(Helpers::Globals::SHOW_DEBUG)
			Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "Showing Debug Output");	
		else
			Helpers::Globals::DebugInformation.AddText(ALWAYS, RED, "Hiding Debug Output");	
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F6)){
		Helpers::Globals::SHOW_INFO= !Helpers::Globals::SHOW_INFO;

		if(Helpers::Globals::SHOW_INFO)
			Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "Showing Info Output");	
		else
			Helpers::Globals::DebugInformation.AddText(ALWAYS, RED, "Hiding Info Output");	
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F7)){
		Helpers::Globals::SHOW_ERRORS = !Helpers::Globals::SHOW_ERRORS;

		if(Helpers::Globals::SHOW_ERRORS)
			Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "Showing Error Output");	
		else
			Helpers::Globals::DebugInformation.AddText(ALWAYS, RED, "Hiding Error Output");	
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_H)){
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "F - Decrease surfel scale");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "R - Increase surfel scale");	
		
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "Enter - Reset scene");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "Left Mouse Button - Fire a projectile");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "Caps Lock - Acquire / Unacquire mouse");	
		
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "Mouse - Change camera's direction");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "Tab - Enable camera / wrecking ball movement");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "(Right Shift + W / S = Move wrecking ball up / down)");			
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "WASD - Move camera / Move wrecking ball ");	

		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "F7 - Enable / Disable Error Output");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "F6 - Enable / Disable Info Output");	
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "F5 - Enable / Disable Debug Output");	

		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "F3 - Draw Phyxels");
		Helpers::Globals::DebugInformation.AddText(ALWAYS, LIGHT_YELLOW_GREEN, "F2 - Draw PhyxelGrid");
		Helpers::Globals::DebugInformation.AddText(ALWAYS, GREEN, "F1 - Surfel / Wireframe / Solid representation");
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_CAPSLOCK)){
		if(Helpers::Globals::MOUSE_ACQUIRED){
			Helpers::MouseHandler::CleanUp();
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, RED, "Mouse unacquired");
		}
		else{
			Helpers::MouseHandler::SetUp(this->mhAppInst, this->mhMainWnd);
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, GREEN, "Mouse acquired");
		}
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_RETURN)){
		Reset();
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_TAB)){
		if(!Helpers::Globals::MOVE_WRECKINGBALL){
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, "Moving wreckingball");
			Helpers::Globals::MOVE_WRECKINGBALL = true;
		}
		else{
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, "Moving camera");
			Helpers::Globals::MOVE_WRECKINGBALL = false;
		}
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F2)){
		if(!Helpers::Globals::DRAW_PHYXEL_GRID){
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), "Drawing PhyxelGrid");
			Helpers::Globals::DRAW_PHYXEL_GRID = true;

		}
		else{
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), "Not Drawing PhyxelGrid");
			Helpers::Globals::DRAW_PHYXEL_GRID= false;
		}
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F3)){
		if(!Helpers::Globals::DRAW_PHYXELS){
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), "Drawing Phyxels");
			Helpers::Globals::DRAW_PHYXELS= true;

		}
		else{
			Helpers::Globals::DebugInformation.AddText(INFO_TYPE, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), "Not Drawing Phyxels");
			Helpers::Globals::DRAW_PHYXELS = false;
		}
	}
	else if(Helpers::KeyboardHandler::IsKeyDown(DIK_R)){
		Surface::RadiusScale += 0.01f;
		
		if(Surface::RadiusScale > 2.0f)
			Surface::RadiusScale = 2.0f;

		Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), "Radius Scale = %f", Surface::RadiusScale );
	}
	else if(Helpers::KeyboardHandler::IsKeyDown(DIK_F)){
		Surface::RadiusScale -= 0.01f;
		if(Surface::RadiusScale < 0.01f)
			Surface::RadiusScale = 0.01f;

		Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), "Radius Scale = %f", Surface::RadiusScale );
	}

	if(Helpers::MouseHandler::IsButtonPressed(Helpers::MouseHandler::MOUSE_LEFTBUTTON)){
		projectiles.Add();
	}
}

void WallDestructionApp::drawScene()
{
	D3DApp::drawScene();
	projectiles.Draw();
	wreckingBall.Draw();

	ground->Draw();
	wall->Draw();

	if(Helpers::Globals::DRAW_PHYXELS)
		ChangedPhyxels::Draw();

	// draw text
	RECT R = {5, 5, 0, 0};
	mFont->DrawTextA(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, RED);

	mFont->DrawTextA(0, instructions.c_str(), -1, &instructionRect, DT_NOCLIP, GREEN);

	Helpers::Globals::DebugInformation.Draw();
	mSwapChain->Present(0, 0);
}
