#include "WreckingBall.h"
#include "Globals.h"
#include "PhysicsWrapper.h"
#include "KeyboardHandler.h"

namespace Drawables{
	void WreckingBall::Init(){
		D3DXMatrixRotationYawPitchRoll(&rotationMatrix, 0.0f, 0.0f, 0.0f);
		D3DXMatrixTranslation(&translationMatrix, 0.0f, 5.0f, 0.0f);

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
		};

		this->SetMaterialInfo(0.600672f, 0.668533f);

		wreckingBallEffect = Helpers::CustomEffect("SphereEffect.fx", "SphereTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 3);

		wreckingBallEffect.AddVariable("World");
		wreckingBallEffect.AddVariable("View");
		wreckingBallEffect.AddVariable("Projection");

		// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0075_1_S.jpg?id=38616&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
		wreckingBallEffect.AddTexture("tex", "Textures\\WreckingBall.jpg");
		wreckingBallEffect.AddVariable("LightPos");
		wreckingBallEffect.AddVariable("AmbientColor");
		wreckingBallEffect.AddVariable("CameraPos");
		wreckingBallEffect.AddVariable("LightDirection");
		wreckingBallEffect.AddVariable("A");
		wreckingBallEffect.AddVariable("B");
		wreckingBallEffect.AddVariable("rhoOverPi");
		wreckingBallEffect.AddVariable("LightColor");		
		wreckingBallEffect.AddVariable("depthMap");
		wreckingBallEffect.AddVariable("windowWidth");
		wreckingBallEffect.AddVariable("windowHeight");


		wreckingBallEffect.SetFloat("windowWidth", Helpers::Globals::ClientWidth);
		wreckingBallEffect.SetFloat("windowHeight", Helpers::Globals::ClientHeight);

		wreckingBallEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
		wreckingBallEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
		wreckingBallEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
		wreckingBallEffect.SetFloat("A", this->A);
		wreckingBallEffect.SetFloat("B", this->B);
		wreckingBallEffect.SetFloat("rhoOverPi", this->rhoOverPi);
		wreckingBallEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());

		// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0075_1_S.jpg?id=38616&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
		wreckingBallEffect.SetTexture("tex", "Textures\\WreckingBall.jpg");

		D3D10_INPUT_ELEMENT_DESC depthLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		};

		depthEffect = Helpers::CustomEffect("SphereEffect.fx", "SphereDepthTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, depthLayout, 1);
		depthEffect.AddVariable("World");
		depthEffect.AddVariable("View");
		depthEffect.AddVariable("Projection");
		depthEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		sphere = Sphere();
		sphere.init(radius, 30, 30);
		sphere.SetPosition(0.0f, 10.0f, -20.0f);
		this->chain = Chain();
		this->chain.Init(sphere.GetPositionVector() + D3DXVECTOR3(0.0f, radius, 0.0f), sphere.GetPositionVector() + D3DXVECTOR3(0.0f, 20.0f + radius, 0.0f), 4);

		PhysicsWrapper::AddWreckingBall(this);
	}

	void WreckingBall::Draw( ID3D10ShaderResourceView* depthMap )
	{
		wreckingBallEffect.SetTexture("depthMap", depthMap);
		wreckingBallEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
		wreckingBallEffect.SetMatrix("World", this->sphere.GetWorld());
		wreckingBallEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		wreckingBallEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());

		wreckingBallEffect.PreDraw();

		sphere.Draw(&wreckingBallEffect);

		this->chain.Draw(depthMap);
	}

	void WreckingBall::DrawDepth(){
		depthEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
		depthEffect.SetMatrix("World", this->sphere.GetWorld());
		
		depthEffect.PreDraw();
			
		sphere.DrawDepth(&depthEffect);

		this->chain.DrawDepth();
	}

	void WreckingBall::Update(float dt){
		this->sphere.SetWorld(PhysicsWrapper::GetWorld(this->sphere.GetRigidBody()));

		this->chain.Update(dt);

		if(Helpers::Globals::MOVE_WRECKINGBALL){
			if(Helpers::KeyboardHandler::IsKeyDown(DIK_W)){
				PhysicsWrapper::SetPosition(this->chain[0]->GetRigidBody(), D3DXVECTOR3(0.0f, 0.0f, -15.0f*dt), true);
			}
			else if(Helpers::KeyboardHandler::IsKeyDown(DIK_S)){
				PhysicsWrapper::SetPosition(this->chain[0]->GetRigidBody(), D3DXVECTOR3(0.0f, 0.0f, 15.0f*dt), true);
			}

			if(Helpers::KeyboardHandler::IsKeyDown(DIK_A)){
				PhysicsWrapper::SetPosition(this->chain[0]->GetRigidBody(), D3DXVECTOR3(-15.0f*dt, 0.0f, 0.0f), true);
			}
			else if(Helpers::KeyboardHandler::IsKeyDown(DIK_D)){
				PhysicsWrapper::SetPosition(this->chain[0]->GetRigidBody(), D3DXVECTOR3(15.0f*dt, 0.0f, 0.0f), true);			
			}
		}
	}

	void WreckingBall::CleanUp(){
		wreckingBallEffect.CleanUp();
		sphere.CleanUp();
		this->chain.CleanUp();
	}

	void WreckingBall::ResetBuffers()
	{
		sphere.init(radius, 30, 30);
	}
}
