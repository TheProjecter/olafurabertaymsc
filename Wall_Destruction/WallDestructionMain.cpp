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

#ifdef DETECT_MEMORY_LEAKS
	//#include "vld.h"
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

private:
	string instructions;
	RECT instructionRect;
	std::map<std::string, MeshlessObject*, Structs::NameComparer> meshlessObjects;
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
	Helpers::MouseHandler::CleanUp();
	Helpers::KeyboardHandler::CleanUp();

	Helpers::Globals::DebugInformation.CleanUp();
	PhysicsWrapper::CleanUp();
	
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

	//testRectangle.CleanUp();
}

void WallDestructionApp::Reset(){

	Helpers::Globals::DebugInformation.StartTimer();

	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->CleanUp();
			delete Helpers::ObjectHelper::MeshlessObjectsIterator->second;
	}

	Helpers::ObjectHelper::MeshlessObjects.clear();

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

	Helpers::Globals::DebugInformation.EndTimer("Restart ");
	resetLastFrame = true;
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

	DepthPass.init(Helpers::Globals::ClientWidth, Helpers::Globals::ClientHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AttributePass.init(Helpers::Globals::ClientWidth, Helpers::Globals::ClientHeight, 3, DXGI_FORMAT_R32G32B32A32_FLOAT);
	NormalizationPass.init(Helpers::Globals::ClientWidth, Helpers::Globals::ClientHeight, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	depthMapVP = SpriteViewPort(0.3f, 0.3f, D3DXVECTOR2(0.7f, -0.99f));
	normalMapVP = SpriteViewPort(0.3f, 0.3f, D3DXVECTOR2(0.4f, -0.99f));
	occlusionMapVP = SpriteViewPort(0.3f, 0.3f, D3DXVECTOR2(0.1f, -0.99f));

	this->depthMapVP.Init();
	this->normalMapVP.Init();
	this->occlusionMapVP.Init();

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
	
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F1)){
		if(Helpers::Globals::SurfelRenderMethod == Helpers::WIREFRAME){
			Helpers::Globals::DebugInformation.AddText("Setting SurfelObject.RenderMethod to SOLID");
			Helpers::Globals::SurfelRenderMethod = Helpers::SOLID;
		}
		else{
			Helpers::Globals::DebugInformation.AddText("Setting SurfelObject.RenderMethod to WIREFRAME");
			Helpers::Globals::SurfelRenderMethod = Helpers::WIREFRAME;
		}		
	}
	else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_H)){
		Helpers::Globals::DebugInformation.AddText(GREEN, "Enter - Reset scene");	
		Helpers::Globals::DebugInformation.AddText(LIGHT_YELLOW_GREEN, "Tab - Enable camera / wrecking ball movement");	
		Helpers::Globals::DebugInformation.AddText(GREEN, "Caps Lock - Acquire / Unacquire mouse");	
		Helpers::Globals::DebugInformation.AddText(LIGHT_YELLOW_GREEN, "Left Mouse Button - Fire a projectile");	
		Helpers::Globals::DebugInformation.AddText(GREEN, "Mouse - Change camera's direction");	
		Helpers::Globals::DebugInformation.AddText(LIGHT_YELLOW_GREEN, "WASD - Move camera / Move wrecking ball");	
		Helpers::Globals::DebugInformation.AddText(GREEN, "F1 - Surfel / Wireframe representation");
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
	/*else if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_F4)){
		if(!Helpers::Globals::DRAW_OCTREE){
			Helpers::Globals::DebugInformation.AddText(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), "Drawing Octree");
			Helpers::Globals::DRAW_OCTREE = true;

		}
		else{
			Helpers::Globals::DebugInformation.AddText(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), "Not Drawing Octree");
			Helpers::Globals::DRAW_OCTREE= false;
		}
	}*/
	else if(Helpers::KeyboardHandler::IsKeyDown(DIK_R)){
		Surface::RadiusScale += 0.01f;
		
		if(Surface::RadiusScale > 2.0f)
			Surface::RadiusScale = 2.0f;

		Helpers::Globals::DebugInformation.AddText(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f), "Radius Scale = %f", Surface::RadiusScale );
	}
	else if(Helpers::KeyboardHandler::IsKeyDown(DIK_F)){
		Surface::RadiusScale -= 0.01f;
		if(Surface::RadiusScale < 0.01f)
			Surface::RadiusScale = 0.01f;

		Helpers::Globals::DebugInformation.AddText(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), "Radius Scale = %f", Surface::RadiusScale );
	}

	if(Helpers::MouseHandler::IsButtonPressed(Helpers::MouseHandler::MOUSE_LEFTBUTTON)){
		projectiles.Add();
	}
}

void WallDestructionApp::drawScene()
{
	D3DApp::drawScene();

	// draw to shadow map
/*	DepthPass.begin();

	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->DrawDepth();
	}

	projectiles.DrawDepth();
	wreckingBall.DrawDepth();

	DepthPass.end();
*/
	projectiles.Draw(DepthPass.colorMap(0));
	wreckingBall.Draw(DepthPass.colorMap(0));

/*	AttributePass.begin();
	// draw attributes of the meshless objects
	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->DrawAttributes(DepthPass.colorMap(0));
	}
	AttributePass.end();

	NormalizationPass.begin();
*/	// Normalize the attributes 
	for(Helpers::ObjectHelper::MeshlessObjectsIterator = Helpers::ObjectHelper::MeshlessObjects.begin(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator != Helpers::ObjectHelper::MeshlessObjects.end(); 
		Helpers::ObjectHelper::MeshlessObjectsIterator++){
			Helpers::ObjectHelper::MeshlessObjectsIterator->second->Draw();
	}
//	NormalizationPass.end();

//	spriteViewPort.Draw(depthMap.colorMap(1));

/*	depthMapVP.Draw(DepthPass.colorMap(0));
	occlusionMapVP.Draw(AttributePass.colorMap(0));
	normalMapVP.Draw(AttributePass.colorMap(1));
*/
	// draw text
#ifdef _DEBUG
	RECT R = {5, 5, 0, 0};
	mFont->DrawTextA(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, RED);
#endif 

	mFont->DrawTextA(0, instructions.c_str(), -1, &instructionRect, DT_NOCLIP, GREEN);

	Helpers::Globals::DebugInformation.Draw();
	mSwapChain->Present(0, 0);
}
