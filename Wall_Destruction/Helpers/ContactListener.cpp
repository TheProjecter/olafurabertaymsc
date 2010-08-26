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

	ImpactList::AddPreImpact(surfel, D3DXVECTOR3(force(0), force(1), force(2)), D3DXVECTOR3(event.m_contactPoint->getPosition()(0), event.m_contactPoint->getPosition()(1), event.m_contactPoint->getPosition()(2)));
}


