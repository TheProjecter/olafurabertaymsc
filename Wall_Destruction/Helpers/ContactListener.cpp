#include "ContactListener.h"
#include <Physics/Dynamics/Entity/hkpRigidBody.h>	
#include "Globals.h"
#include "FractureManager.h"
#include "WreckingBall.h"
#include "Chain.h"
#include "MathHelper.h"

ContactListener::ContactListener(void)
{
}

ContactListener::ContactListener(ProjectStructs::SURFEL* surfel)
{
	this->surfel = surfel;
	this->surfel->contactListener = this;
}

ContactListener::~ContactListener(void)
{
}

void ContactListener::contactPointCallback( const hkpContactPointEvent& event ){

	if(event.m_type != hkpContactPointEvent::TYPE_EXPAND_MANIFOLD && event.m_type != hkpContactPointEvent::TYPE_MANIFOLD)
		return;

	hkVector4 force = hkVector4(0.0f, 0.0f, 0.0f, 0.0f);
	hkpRigidBody* body;
	hkpPropertyValue propertyValue;
	D3DXVECTOR3 u;

	// calculate the force
	if(!event.m_bodies[0]->isFixed()){
		body = event.m_bodies[0];
	}
	else {
		body = event.m_bodies[1];
	}

	if(body->hasProperty(HK_OBJECT_IS_PROJECTILE))
		u = ((ProjectStructs::PROJECTILE*)(body->getProperty(HK_OBJECT_IS_PROJECTILE).getPtr()))->lastVelocity;
	else if(body->hasProperty(HK_OBJECT_IS_WRECKINGBALL))
		u = ((Drawables::WreckingBall*)(body->getProperty(HK_OBJECT_IS_WRECKINGBALL).getPtr()))->GetLastVelocity();
	else 
		return;

	// F = m*a
	// F = m * ( v - u )
	force(0) = body->getMass() * (u[0]);
	force(1) = body->getMass() * (u[1]);
	force(2) = body->getMass() * (u[2]);

	// the force is divided by delta time in the fracture manager

	AddForce(D3DXVECTOR3(force(0), force(1), force(2)), D3DXVECTOR3(event.m_contactPoint->getPosition()(0), event.m_contactPoint->getPosition()(1), event.m_contactPoint->getPosition()(2)), surfel);
}

void ContactListener::AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel){

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

void ContactListener::AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){
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

bool ContactListener::AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel){

	D3DXVECTOR3 phyxelPos = phyxel->pos;

	D3DXVECTOR3 f = force * FractureManager::CalculateWeight(pos, phyxelPos, 3.0f * phyxel->supportRadius);

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

	ImpactList::AddImpact(impact);		

	return true;
}


