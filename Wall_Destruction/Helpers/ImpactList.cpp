#include "ImpactList.h"
#include <algorithm>
#include "Globals.h"
#include "KeyboardHandler.h"
#include "MathHelper.h"
#include "FractureManager.h"

std::vector<ProjectStructs::IMPACT*> ImpactList::impacts, ImpactList::drawableImpacts;
std::map<ProjectStructs::SURFEL*, PreImpactStruct> ImpactList::preImpacts;
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

void ImpactList::AddPreImpact(ProjectStructs::SURFEL* surfel, D3DXVECTOR3 force, D3DXVECTOR3 pos){
	if(preImpacts.find(surfel) == preImpacts.end())
	{
		PreImpactStruct preimpact;
		preimpact.force = force;
		preimpact.pos = pos;
		preimpact.contactPoints = 1;

		preImpacts[surfel] = preimpact;
	}
	else{
		preImpacts[surfel].force += force;
		preImpacts[surfel].pos += pos;
		preImpacts[surfel].contactPoints++;
	}
}

void ImpactList::CalculateImpacts(){
	std::map<ProjectStructs::SURFEL*, PreImpactStruct>::iterator surfelForceIterator = preImpacts.begin();

	for( ; surfelForceIterator != preImpacts.end(); surfelForceIterator++){
		surfelForceIterator->second.force /= surfelForceIterator->second.contactPoints;
		surfelForceIterator->second.pos /= surfelForceIterator->second.contactPoints;

		AddForce(surfelForceIterator->second.force / (float)surfelForceIterator->second.contactPoints, surfelForceIterator->second.pos / (float)surfelForceIterator->second.contactPoints, surfelForceIterator->first);
	}
	preImpacts.clear();
}


void ImpactList::AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel){

	if(surfel == NULL)
		return;

	D3DXVECTOR3 direction;
	D3DXVec3Normalize(&direction, &force);
	direction.x = ceil(direction.x);
	direction.y = ceil(direction.y);
	direction.z = ceil(direction.z);

	for(unsigned int i = 0; i < surfel->intersectingCells.size(); i++){
		AddForceToPhyxels(force, pos, direction, surfel->intersectingCells[i]->phyxel, surfel);			
		surfel->intersectingCells[i]->phyxel->isChanged = true;
	}
}

void ImpactList::AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){
	if(phyxel != NULL || force == MathHelper::GetZeroVector()){

		std::vector<ProjectStructs::PHYXEL_NODE*> goToNeighborPhyxels;
		for(unsigned int i = 0; i < phyxel->neighbours.GetSize(); i++){
			if(phyxel->neighbours[i] && !phyxel->neighbours[i]->isChanged && AddForceToPhyxel(force, pos, direction, phyxel->neighbours[i], surfel)){
				phyxel->neighbours[i]->isChanged = true;
				goToNeighborPhyxels.push_back(phyxel->neighbours[i]);
			}
		}

		for(unsigned int i = 0; i < goToNeighborPhyxels.size(); i++){
			AddForceToPhyxels(force, pos, direction, goToNeighborPhyxels[i], surfel);
		}
	}
}

bool ImpactList::AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){

	D3DXVECTOR3 phyxelPos = phyxel->pos;

	D3DXVECTOR3 f = force * FractureManager::CalculateWeight(/*surfel->vertex->*/pos, phyxelPos, 3.0f * phyxel->supportRadius);

	if(D3DXVec3Length(&f) < 0.001f)
		return false;	

	if(phyxel->isChanged){
		phyxel->force.x += f.x;					
		phyxel->force.y += f.y;					
		phyxel->force.z += f.z;	
	}
	else{
		phyxel->force.x = f.x;					
		phyxel->force.y = f.y;					
		phyxel->force.z = f.z;	
	}

	ProjectStructs::IMPACT* impact = new ProjectStructs::IMPACT;
	phyxel->isChanged = true;
	impact->phyxel = phyxel;
	impact->impactPos = pos;
	impact->surfel = surfel;

	AddImpact(impact);		

	return true;
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