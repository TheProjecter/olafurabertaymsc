#include "ChangedPhyxels.h"
#include <algorithm>
#include "Globals.h"

std::vector<ProjectStructs::PHYXEL_NODE*> ChangedPhyxels::phyxels;
Helpers::CustomEffect ChangedPhyxels::phyxelEffect;
Sphere ChangedPhyxels::phyxelSphere;
D3DXMATRIX ChangedPhyxels::world;

void ChangedPhyxels::Init(){
	D3D10_INPUT_ELEMENT_DESC sphereLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	phyxelEffect= Helpers::CustomEffect("PhyxelEffect.fx", "PhyxelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, sphereLayout, 1);
	phyxelEffect.AddVariable("World");
	phyxelEffect.AddVariable("View");
	phyxelEffect.AddVariable("Projection");
	phyxelEffect.AddVariable("Color");
	phyxelEffect.AddVariable("Force");

	phyxelEffect.SetFloatVector("Color", D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	phyxelSphere = Sphere();
	phyxelSphere.init(0.1f, 3, 3);

}

void ChangedPhyxels::CleanUp(){
	phyxelSphere.CleanUp();
	phyxelEffect.CleanUp();
	
	phyxels.clear();
	phyxels.swap(std::vector<ProjectStructs::PHYXEL_NODE*>());

}

void ChangedPhyxels::AddPhyxel(ProjectStructs::PHYXEL_NODE* node){

	if(find(phyxels.begin(), phyxels.end(), node) == phyxels.end())
		phyxels.push_back(node);

}

void ChangedPhyxels::Update(float dt){
	for(int i = 0; i<phyxels.size(); i++){
		if(phyxels[i]->force.x != 0.0f || phyxels[i]->force.y != 0.0f || phyxels[i]->force.z != 0.0f ){
			
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
		else{
			phyxels.erase(phyxels.begin() + i);
			i--;
		}
	}
}

void ChangedPhyxels::Draw(){
	phyxelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	phyxelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	phyxelEffect.PreDraw();

	// draw phyxels
	for(unsigned int i = 0; i< phyxels.size(); i++){
		D3DXMatrixTranslation(&world, phyxels[i]->pos.x, phyxels[i]->pos.y, phyxels[i]->pos.z);
		phyxelEffect.SetFloatVector("Force", D3DXVECTOR4(phyxels[i]->force, 0.0f));
		phyxelEffect.SetMatrix("World", world);
		phyxelSphere.Draw(&phyxelEffect);
	}
}


