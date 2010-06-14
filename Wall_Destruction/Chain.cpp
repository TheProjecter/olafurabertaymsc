#include "Chain.h"
#include "PhysicsWrapper.h"

void Chain::Init(D3DXVECTOR3 start, D3DXVECTOR3 end, int count){
	
	this->start = start;
	this->end = end;

	delta = (end - start) / ((float)count);

// 	this->DiffuseColor = D3DXVECTOR3(0.3f, 0.3f, 0.3f);
// 	this->SpecularColor = D3DXVECTOR3(0.6f, 0.6f, 0.6f);
// 	this->SpecularPower = 2.0f;

	this->SetMaterialInfo(0.600672f, 0.668533f);

	for(int i = 0; i<count; i++){
		
		Sphere* chainSphere = new Sphere();
		//chainSphere->init(max(delta.x, max(delta.y, delta.z))/ 2.0f, 10, 10);
		chainSphere->init(0.5f, 10, 10);
		chainSphere->SetPosition((end - (float)i*delta).x, (end - (float)i*delta).y, (end - (float)i*delta).z);

		chainSpheres.push_back(chainSphere);
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
	};

	effect = Helpers::CustomEffect("SphereEffect.fx", "SphereTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 3);
	effect.AddVariable("World");
	effect.AddVariable("View");
	effect.AddVariable("InvView");
	effect.AddVariable("Projection");
	// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0075_1_S.jpg?id=38616&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
	effect.AddTexture("tex", "Textures\\WreckingBall.jpg");

	effect.AddVariable("LightPos");
	effect.AddVariable("AmbientColor");
	effect.AddVariable("CameraPos");
	effect.AddVariable("LightDirection");
	effect.AddVariable("A");
	effect.AddVariable("B");
	effect.AddVariable("rhoOverPi");
	effect.AddVariable("LightColor");		

	effect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
	effect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
	effect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
	effect.SetFloat("A", this->A);
	effect.SetFloat("B", this->B);
	effect.SetFloat("rhoOverPi", this->rhoOverPi);
	effect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());

	// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0075_1_S.jpg?id=38616&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
	effect.SetTexture("tex", "Textures\\WreckingBall.jpg");
}

void Chain::Update(float dt){
	for(unsigned int i = 0; i<chainSpheres.size(); i++){
		chainSpheres[i]->SetWorld(PhysicsWrapper::GetWorld(chainSpheres[i]->GetRigidBody()));
	}
}

void Chain::Draw(){	
	effect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	effect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	effect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());

	for(unsigned int i = 0; i<chainSpheres.size(); i++){
		effect.SetMatrix("World", chainSpheres[i]->GetWorld());		
		effect.PreDraw();
		chainSpheres[i]->Draw(&effect);
	}
}

void Chain::CleanUp(){
	for(unsigned int i = 0; i<chainSpheres.size(); i++){
		chainSpheres[i]->CleanUp();
		delete chainSpheres[i];
	}

	effect.CleanUp();
}

