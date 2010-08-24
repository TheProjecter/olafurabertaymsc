#include "ImpactList.h"
#include <algorithm>
#include "Globals.h"
#include "KeyboardHandler.h"

std::vector<ProjectStructs::IMPACT*> ImpactList::impacts, ImpactList::drawableImpacts;
Helpers::CustomEffect ImpactList::impactEffect;
ID3D10Buffer *ImpactList::mVB;
bool ImpactList::hasBeenSetup, ImpactList::hasBeenInitedOrSomething = false;
int ImpactList::impactsToDraw;
D3DXMATRIX ImpactList::world;

void ImpactList::Init(){
	D3D10_INPUT_ELEMENT_DESC sphereLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	impactEffect= Helpers::CustomEffect("PhyxelEffect.fx", "PhyxelTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, sphereLayout, 1);
	impactEffect.AddVariable("World");
	impactEffect.AddVariable("View");
	impactEffect.AddVariable("Projection");

	hasBeenSetup = false;
	hasBeenInitedOrSomething = true;

	D3DXMatrixIdentity(&world);
	mVB = NULL;

	impacts = std::vector<ProjectStructs::IMPACT*>();
	drawableImpacts = std::vector<ProjectStructs::IMPACT*>();
}

void ImpactList::CleanUp(){
	if(mVB){
		mVB->Release();
		mVB = 0;
	}

	impactEffect.CleanUp();

	for(unsigned int i = 0; i<drawableImpacts.size(); i++){
		delete drawableImpacts[i];
	}
	
	impacts.clear();
	impacts.swap(std::vector<ProjectStructs::IMPACT*>());

	drawableImpacts.clear();
	drawableImpacts.swap(std::vector<ProjectStructs::IMPACT*>());
}

void ImpactList::AddImpact(ProjectStructs::IMPACT* node){
	bool containsPhyxel = false;
	for(int i = 0; i<impacts.size() && !containsPhyxel; i++){
		if(impacts[i]->phyxel == node->phyxel)
		{
			containsPhyxel = true;
		}
	}
	if(!containsPhyxel){
		node->phyxel->isChanged = true;
		impacts.push_back(node);
		drawableImpacts.push_back(node);
		hasBeenSetup = false;
	}
	else{
		delete node;
	}
}

void ImpactList::Update(float dt){
	if(Helpers::KeyboardHandler::IsSingleKeyDown(DIK_BACKSPACE)){
		for(unsigned int i = 0; i<drawableImpacts.size(); i++){
			drawableImpacts[i]->phyxel->bodyForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawableImpacts[i]->phyxel->displacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawableImpacts[i]->phyxel->displacementGradient = D3DXMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
			drawableImpacts[i]->phyxel->dotDisplacement = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			drawableImpacts[i]->phyxel->jacobian = drawableImpacts[i]->phyxel->displacementGradient;
			drawableImpacts[i]->phyxel->strain = drawableImpacts[i]->phyxel->displacementGradient;
			drawableImpacts[i]->phyxel->stress = drawableImpacts[i]->phyxel->displacementGradient;

			drawableImpacts[i]->phyxel->addedNodes.clear();
			delete drawableImpacts[i];
		}
		drawableImpacts.clear();
		hasBeenSetup = false;
	}
}

void ImpactList::Emptylist(){
	bool phyxelsHaveChanged = false;
	for(unsigned int i = 0; i<impacts.size(); i++){
		impacts[i]->phyxel->force.x = 0.0f;
		impacts[i]->phyxel->force.y = 0.0f;
		impacts[i]->phyxel->force.z = 0.0f;
		impacts[i]->phyxel->isChanged = false;
	}

	impacts.clear();
}

struct IMPACT_VERTEX{
	D3DXVECTOR3 pos;
};

bool ImpactList::SetupImpacts(){

	if(drawableImpacts.size() == 0)
		return false;;

	std::vector<IMPACT_VERTEX> vertexList;

	int drawableImpactIndex = 0;
	for(unsigned int i = 0; i<drawableImpacts.size(); i++){
		ProjectStructs::IMPACT* impact = drawableImpacts[i];
		IMPACT_VERTEX iv;
		iv.pos = impact->phyxel->pos;
		vertexList.push_back(iv);
		iv.pos = impact->phyxel->pos + impact->phyxel->displacement;
		vertexList.push_back(iv);
	}

	IMPACT_VERTEX* vertices = NULL;
	vertices = new IMPACT_VERTEX[vertexList.size()];

	for(unsigned int i = 0; i<vertexList.size(); i++){
		vertices[i] = vertexList[i];
	}

	impactsToDraw = vertexList.size();

	if(impactsToDraw == 0)
		return false;

	D3D10_BUFFER_DESC vbd;
	vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = impactsToDraw * sizeof(IMPACT_VERTEX);
	vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(Helpers::Globals::Device->CreateBuffer(&vbd, &vinitData, &mVB));
	
	return true;
}

void ImpactList::Draw(){
	if(!hasBeenInitedOrSomething)
		Init();

	if(!hasBeenSetup){
		hasBeenSetup = SetupImpacts();
	}

	if(drawableImpacts.size() == 0)
		return;
	
	impactEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	impactEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	impactEffect.SetMatrix("World", world);
	impactEffect.PreDraw();

	UINT stride = sizeof(IMPACT_VERTEX);
	UINT offset = 0;
	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	Helpers::Globals::Device->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	impactEffect.Draw(impactsToDraw);
}