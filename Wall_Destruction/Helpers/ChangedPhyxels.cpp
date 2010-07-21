#include "ChangedPhyxels.h"
#include <algorithm>
#include "Globals.h"
#include "KeyboardHandler.h"

void ChangedPhyxels::Init(){
	D3D10_INPUT_ELEMENT_DESC sphereLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	phyxelEffect= Helpers::CustomEffect("PhyxelEffect.fx", "PhyxelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, sphereLayout, 1);
	phyxelEffect.AddVariable("World");
	phyxelEffect.AddVariable("View");
	phyxelEffect.AddVariable("Projection");

	hasBeenSetup = false;

	D3DXMatrixIdentity(&world);
	mVB = NULL;
}

void ChangedPhyxels::CleanUp(){
	//phyxelSphere.CleanUp();
	if(mVB){
		mVB->Release();
		mVB = 0;
	}

	phyxelEffect.CleanUp();
	
	phyxels.clear();
	phyxels.swap(std::vector<ProjectStructs::PHYXEL_NODE*>());

	drawablePhyxels.clear();
	drawablePhyxels.swap(std::vector<ProjectStructs::PHYXEL_NODE*>());
}

void ChangedPhyxels::AddPhyxel(ProjectStructs::PHYXEL_NODE* node){

	if(node->isChanged)
		return;

	node->isChanged = true;

	phyxels.push_back(node);
	drawablePhyxels.push_back(node);
}

void ChangedPhyxels::AddDrawablePhyxel(ProjectStructs::PHYXEL_NODE* node){
//	drawablePhyxels.push_back(node);
}

void ChangedPhyxels::Update(float dt){
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_BACKSPACE)){
		for(int i = 0; i<drawablePhyxels.size(); i++){
			drawablePhyxels[i]->bodyForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawablePhyxels[i]->displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawablePhyxels[i]->displacementGradient = D3DXMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			drawablePhyxels[i]->dotDisplacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawablePhyxels[i]->jacobian = drawablePhyxels[i]->displacementGradient;
			drawablePhyxels[i]->strain = drawablePhyxels[i]->displacementGradient;
			drawablePhyxels[i]->stress = drawablePhyxels[i]->displacementGradient;
		}
		hasBeenSetup = false;
	}
}

void ChangedPhyxels::Emptylist(){
	bool phyxelsHaveChanged = false;
	for(unsigned int i = 0; i<phyxels.size(); i++){
		phyxels[i]->force.x = 0.0f;
		phyxels[i]->force.y = 0.0f;
		phyxels[i]->force.z = 0.0f;
		phyxels[i]->isChanged = false;
		phyxels.erase(phyxels.begin() + i);
		i--;
		phyxelsHaveChanged = true;
	}

	if(phyxelsHaveChanged){
		hasBeenSetup = false;
	}
}

struct PHYXEL_VERTEX{
	D3DXVECTOR3 pos;
};

bool ChangedPhyxels::SetupPhyxels(){

	if(drawablePhyxels.size() == 0)
		return false;;

	PHYXEL_VERTEX *vertices = NULL;
	vertices = new PHYXEL_VERTEX[drawablePhyxels.size() * 2];

	int drawablePhyxelIndex = 0;
	for(unsigned int i = 0; i<drawablePhyxels.size(); i++){
		ProjectStructs::PHYXEL_NODE *node = drawablePhyxels[i];
		vertices[drawablePhyxelIndex++].pos = node->pos;
		vertices[drawablePhyxelIndex++].pos = node->pos + node->displacement;
	}


	phyxelsToDraw = drawablePhyxelIndex;

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = phyxelsToDraw * sizeof(PHYXEL_VERTEX);
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(Helpers::Globals::Device->CreateBuffer(&vbd, &vinitData, &mVB));
	
	return true;
}

void ChangedPhyxels::Draw(){
	if(!hasBeenSetup){
		hasBeenSetup = SetupPhyxels();
	}

	if(drawablePhyxels.size() == 0)
		return;
	
	phyxelEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	phyxelEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	phyxelEffect.SetMatrix("World", world);
	phyxelEffect.PreDraw();

	UINT stride = sizeof(PHYXEL_VERTEX);
	UINT offset = 0;
	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	Helpers::Globals::Device->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	phyxelEffect.Draw(phyxelsToDraw);
}



