#include "ImpactList.h"
#include <algorithm>
#include "Globals.h"
#include "KeyboardHandler.h"
#include "MathHelper.h"
#include "FractureManager.h"

std::map<ProjectStructs::SURFEL*, PreImpactStruct> ImpactList::preImpacts;

std::vector<ProjectStructs::PHYXEL_NODE*> ImpactList::drawablePhyxels;
std::map<ProjectStructs::PHYXEL_NODE*, bool> ImpactList::affectedPhyxels, ImpactList::newlyAffectedPhyxels;
std::map<ProjectStructs::SURFEL*, bool> ImpactList::affectedSurfels;

std::vector<ProjectStructs::PHYXEL_NODE*> ImpactList::affectedPhyxelsVector;
std::vector<ProjectStructs::SURFEL*> ImpactList::affectedSurfelsVector;

std::map<ProjectStructs::PHYXEL_NODE*, std::vector<ProjectStructs::SURFEL*>> ImpactList::phyxelToSurfels;
std::map<ProjectStructs::SURFEL*, std::vector<ProjectStructs::PHYXEL_NODE*>> ImpactList::surfelsToPhyxel;

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

}

void ImpactList::CleanUp(){
	if(mVB){
		mVB->Release();
		mVB = 0;
	}

	impactEffect.CleanUp();
	
	affectedPhyxels.clear();
	affectedSurfels.clear();
	
	drawablePhyxels.clear();
	drawablePhyxels.swap(std::vector<ProjectStructs::PHYXEL_NODE*>());
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

		AddForce(surfelForceIterator->second.force, surfelForceIterator->second.pos, surfelForceIterator->first);
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
	if(phyxel != NULL && force != MathHelper::GetZeroVector()){
	//	AddForceToPhyxel(force, pos, direction, phyxel, surfel);
		std::vector<ProjectStructs::PHYXEL_NODE*> goToNeighborPhyxels;
		for(unsigned int i = 0; i < phyxel->neighbours.GetSize(); i++){
			if(phyxel->neighbours[i] && !phyxel->neighbours[i]->isChanged && AddForceToPhyxel(force, pos, direction, phyxel->neighbours[i], surfel)){
				phyxel->neighbours[i]->isChanged = true;
				goToNeighborPhyxels.push_back(phyxel->neighbours[i]);	
			}
		}

		for(unsigned int i = 0; i < goToNeighborPhyxels.size(); i++){
			AddForceToPhyxels(force, pos , direction, goToNeighborPhyxels[i], surfel);
		}
	}
}

bool ImpactList::AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){

// 
// 	phyxel->displacement *= 0.0f;

	D3DXVECTOR3 phyxelPos = phyxel->pos + phyxel->totalDisplacement + phyxel->displacement;

	D3DXVECTOR3 f = force * FractureManager::CalculateWeight(pos, phyxelPos, phyxel->supportRadius);

	if(D3DXVec3Length(&f) <= 1.0f)
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

	AddAffectedPhyxel(phyxel);
	
	return true;
}

void ImpactList::Emptylist(){
	affectedPhyxels.clear();
	affectedSurfels.clear();

	affectedPhyxelsVector.clear();
	affectedSurfelsVector.clear();

	phyxelToSurfels.clear();
	surfelsToPhyxel.clear();
}

struct IMPACT_VERTEX{
	D3DXVECTOR3 pos;
};

bool ImpactList::SetupImpacts(){

	if(drawablePhyxels.size() == 0)
		return false;;

	std::vector<IMPACT_VERTEX> vertexList;

	for(unsigned int i = 0; i<drawablePhyxels.size(); i++){
		ProjectStructs::PHYXEL_NODE* phyxel = drawablePhyxels[i];
		IMPACT_VERTEX iv;
		iv.pos = phyxel->pos;
		vertexList.push_back(iv);
		iv.pos = phyxel->pos + phyxel->totalDisplacement + phyxel->displacement;
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

	if(drawablePhyxels.size() == 0)
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

void ImpactList::AddAffectedPhyxel( ProjectStructs::PHYXEL_NODE * phyxel )
{
	if((phyxel->bodyForce != MathHelper::GetZeroVector() || phyxel->force!= MathHelper::GetZeroVector()) && affectedPhyxels.find(phyxel) == affectedPhyxels.end()){
		if(find(drawablePhyxels.begin(), drawablePhyxels.end(), phyxel) == drawablePhyxels.end()){
			drawablePhyxels.push_back(phyxel);
		}

/*
		for(int i = 0; i<phyxel->parent->surfels.size(); i++){
			surfelsToPhyxel[phyxel->parent->surfels[i]].push_back(phyxel);
		}

		phyxelToSurfels[phyxel].insert(phyxelToSurfels[phyxel].end(), phyxel->parent->surfels.begin(), phyxel->parent->surfels.end());
*/
		affectedPhyxels[phyxel] = true;
		for(int i = 0; i<phyxel->parent->surfels.size(); i++){
	//		if(find(affectedSurfels.begin(), affectedSurfels.end(), phyxel->parent->surfels[i]) == affectedSurfels.end())
			affectedSurfels[phyxel->parent->surfels[i]] = true;
		}
		hasBeenSetup = false;
	}
}

void ImpactList::AddAffectedSurfel( SURFEL* surfel )
{
	affectedSurfels[surfel] = true;
	affectedSurfelsVector.clear();
}