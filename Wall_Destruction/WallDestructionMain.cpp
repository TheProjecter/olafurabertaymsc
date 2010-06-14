//=============================================================================
// The main application. Uses the Luna DirectX10 framework.
//
//=============================================================================

#include "d3dApp.h"
#include "d3dUtil.h"

// Keycode
#include <Common/Base/keycode.cxx>

// Classlists
#define INCLUDE_HAVOK_PHYSICS_CLASSES
#define HK_CLASSES_FILE <Common/Serialize/Classlist/hkClasses.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.cxx>

#include "PhysicsWrapper.h"
#define DIRECTINPUT_VERSION 0x0800

#if defined(DEBUG) || defined(_DEBUG)
#include "vld.h"
#endif

#include "Globals.h"
#include "SurfelObject.h"
#include "Projectiles.h"
#include "InfoText.h"
#include "MouseHandler.h"
#include "KeyboardHandler.h"
#include "ObjectHelper.h"
#include "RandRange.h"
#include "WreckingBall.h"
#include <ctime>
#include <map>

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

private:
//	SurfelObject surfelObject;
	string instructions;
	RECT instructionRect;
	std::map<std::string, MeshlessObject*, Structs::NameComparer> meshlessObjects;
	Drawables::WreckingBall wreckingBall;
	Projectiles projectiles;
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
	Helpers::MouseHandler::CleanUp();
	Helpers::KeyboardHandler::CleanUp();
	//surfelObject.CleanUp();
	Helpers::Globals::DebugInformation.CleanUp();
	
	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->CleanUp();
			delete Helpers::ObjectHelper::MeshlessObjectsIterator->second;
	}

	wreckingBall.CleanUp();
	projectiles.CleanUp();

	if( md3dDevice )
		md3dDevice->ClearState();

	PhysicsWrapper::CleanUp();
	//testRectangle.CleanUp();
}

void WallDestructionApp::Reset(){

	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->CleanUp();
			delete Helpers::ObjectHelper::MeshlessObjectsIterator->second;
	}
	wreckingBall.CleanUp();
	projectiles.CleanUp();
	PhysicsWrapper::CleanUp();

	PhysicsWrapper::Init();

	// get all the meshless objects 
	new MeshlessObject("MeshlessObjects\\Wall.xml");
	new MeshlessObject("MeshlessObjects\\Ground.xml");

	// create the wreckingball
	wreckingBall = WreckingBall(5.0f);
	wreckingBall.Init();
	projectiles.Init();

	PhysicsWrapper::FinishInit();
}

void WallDestructionApp::initApp()
{
	D3DApp::initApp();

	Helpers::Globals::Device = md3dDevice;
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
	Helpers::Globals::Window = mhMainWnd;
	Helpers::Globals::AppCamera = Camera(D3DXVECTOR3(0.0f, 0.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));	
	Helpers::Globals::AppLight.Init();	
	Helpers::Globals::DebugInformation = InfoText(this->mClientWidth / 2 - 50, 5);

	Helpers::MouseHandler::SetUp(mhAppInst, mhMainWnd);
	Helpers::KeyboardHandler::Init();
	PhysicsWrapper::Init();
	
	
	instructionRect.left = this->mClientWidth - 50;
	instructionRect.right = 0;
	instructionRect.top = 5;
	instructionRect.bottom = 0;

	instructions = "(H)elp";
	srand((unsigned)time(0));

	// get all the meshless objects 
	new MeshlessObject("MeshlessObjects\\Ground.xml");
	new MeshlessObject("MeshlessObjects\\Wall.xml");

	// create the wreckingball
	wreckingBall = WreckingBall(5.0f);
	wreckingBall.Init();

	projectiles.Init();

	PhysicsWrapper::FinishInit();
}

void WallDestructionApp::onResize()
{
	D3DApp::onResize();
	Helpers::Globals::ClientHeight = mClientHeight;
	Helpers::Globals::ClientWidth = mClientWidth;
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
	Helpers::MouseHandler::DetectInput();
	Helpers::KeyboardHandler::Update();
	
	Helpers::Globals::DebugInformation.Update(dt);
	
	Helpers::Globals::AppCamera.Update(dt);
	
	PhysicsWrapper::Update(dt);

	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->Update(dt);
	}

	wreckingBall.Update(dt);
	projectiles.Update(dt);
	
	#ifdef _DEBUG
		D3DApp::updateScene(dt);
	#endif 
		
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F1) && Helpers::Globals::SurfelDrawMethod != Helpers::SURFEL){ 
		Helpers::Globals::DebugInformation.AddText("Setting SurfelObject.Drawmethod to SURFEL");
		Helpers::Globals::SurfelDrawMethod = Helpers::SURFEL;
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F2) && Helpers::Globals::SurfelDrawMethod != Helpers::SOLID){
		Helpers::Globals::DebugInformation.AddText("Setting SurfelObject.Drawmethod to SOLID");
		Helpers::Globals::SurfelDrawMethod = Helpers::SOLID;
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F3) && Helpers::Globals::SurfelDrawMethod != Helpers::WIREFRAME){
		Helpers::Globals::DebugInformation.AddText("Setting SurfelObject.Drawmethod to WIREFRAME");
		Helpers::Globals::SurfelDrawMethod  = Helpers::WIREFRAME;
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_H)){
		Helpers::Globals::DebugInformation.AddText("Enter - Reset scene");	
		Helpers::Globals::DebugInformation.AddText("Tab - Enable camera / wrecking ball movement");	
		Helpers::Globals::DebugInformation.AddText("Caps Lock - Acquire / Unacquire mouse");	
		Helpers::Globals::DebugInformation.AddText("Left Mouse Button - Fire a projectile");	
		Helpers::Globals::DebugInformation.AddText("Mouse - Change camera's direction");	
		Helpers::Globals::DebugInformation.AddText("WASD - Move camera / Move wrecking ball");	
		Helpers::Globals::DebugInformation.AddText("F3 - Wireframe representation");
		Helpers::Globals::DebugInformation.AddText("F2 - Solid representation");
		Helpers::Globals::DebugInformation.AddText("F1 - Surfel representation");
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_CAPSLOCK)){
		if(Helpers::Globals::MOUSE_ACQUIRED){
			Helpers::MouseHandler::CleanUp();
			Helpers::Globals::DebugInformation.AddText(RED, "Mouse unacquired");
		}
		else{
			Helpers::MouseHandler::SetUp(this->mhAppInst, this->mhMainWnd);
			Helpers::Globals::DebugInformation.AddText(GREEN, "Mouse acquired");
		}
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_RETURN)){
		Helpers::Globals::DebugInformation.AddText(RED, "Resetting scene");
		Reset();
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_TAB)){
		if(!Helpers::Globals::MOVE_WRECKINGBALL){
			Helpers::Globals::DebugInformation.AddText("Moving wreckingball");
			Helpers::Globals::MOVE_WRECKINGBALL = true;
		}
		else{
			Helpers::Globals::DebugInformation.AddText("Moving camera");
			Helpers::Globals::MOVE_WRECKINGBALL = false;
		}
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
	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->Draw();
	}

	// draw text
#ifdef _DEBUG
	RECT R = {5, 5, 0, 0};
	mFont->DrawTextA(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, RED);
#endif 

	mFont->DrawTextA(0, instructions.c_str(), -1, &instructionRect, DT_NOCLIP, GREEN);

	Helpers::Globals::DebugInformation.Draw();
	mSwapChain->Present(0, 0);
}
