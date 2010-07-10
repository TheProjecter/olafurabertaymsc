#include "PhyxelObject.h"

void PhyxelObject::Init(PhyxelGrid *phyxelGrid)
{
	
	for(int i = 0; i < phyxelGrid->GetCells().GetWidth(); i++){
		for(int j = 0; j < phyxelGrid->GetCells().GetWidth(); j++){
			for(int k = 0; k < phyxelGrid->GetCells().GetWidth(); k++){
				if(phyxelGrid->GetCells()(i, j, k) != NULL){
					ProjectStructs::PHYXEL_NODE *phyxel = new ProjectStructs::PHYXEL_NODE();
					phyxel->pos = phyxelGrid->GetPositionOfIndex(i, j, k, false);
					phyxel->radius = 10.0f; // TODO change
					phyxel->mass = 4.0f/3.0f * Helpers::Globals::PI * pow(phyxel->radius, 3) * density;
					phyxel->force = D3DXVECTOR3(0.0f, 0.0f, 0.0);

					phyxelGrid->GetCells()(i, j, k)->phyxel = phyxel;
					phyxels.push_back(phyxel);
				}
			}
		}
	}

	SetUpDraw();
}

void PhyxelObject::CleanUpDrawables(){
	if(drawBeenSetup){
		sphereEffect.CleanUp();
		phyxelSphere.CleanUp();

		drawBeenSetup = false;			
	}
}

void PhyxelObject::CleanUp(){
	CleanUpDrawables();
	phyxelSphere.CleanUp();

	for(int i = 0; i < phyxels.size(); i++){
		delete phyxels[i];
	}

	phyxels.clear();
}

void PhyxelObject::SetUpDraw(){

	D3D10_INPUT_ELEMENT_DESC sphereLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	sphereEffect = Helpers::CustomEffect("PhyxelEffect.fx", "PhyxelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, sphereLayout, 1);
	sphereEffect.AddVariable("World");
	sphereEffect.AddVariable("View");
	sphereEffect.AddVariable("Projection");
	sphereEffect.AddVariable("Color");
	sphereEffect.AddVariable("Force");

	sphereEffect.SetFloatVector("Color", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	
	phyxelSphere = Sphere();
	phyxelSphere.init(0.1f, 3, 3);

	this->drawBeenSetup = true;
}

void PhyxelObject::Update(float dt){
	this->dt = dt;
}

// combined draw 
void PhyxelObject::Draw(){

	
	if(!drawBeenSetup)
		SetUpDraw();

	sphereEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	sphereEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	sphereEffect.PreDraw();

	// draw phyxels
	for(unsigned int i = 0; i< phyxels.size(); i++){
		D3DXMatrixTranslation(&world, phyxels[i]->pos.x, phyxels[i]->pos.y, phyxels[i]->pos.z);
		sphereEffect.SetFloatVector("Force", D3DXVECTOR4(phyxels[i]->force, 0.0f));
		sphereEffect.SetMatrix("World", world);
		phyxelSphere.Draw(&sphereEffect);

		if(phyxels[i]->force.x != 0.0f || phyxels[i]->force.y != 0.0f || phyxels[i]->force.z != 0.0f){
			phyxels[i]->force.x -= dt * 200.0f;
			phyxels[i]->force.y -= dt * 200.0f;
			phyxels[i]->force.z -= dt * 200.0f;

			if(phyxels[i]->force.x < 0.0f)
				phyxels[i]->force.x = 0.0f;
			if(phyxels[i]->force.y < 0.0f)
				phyxels[i]->force.y = 0.0f;
			if(phyxels[i]->force.z < 0.0f)
				phyxels[i]->force.z = 0.0f;
		}
	}
}
