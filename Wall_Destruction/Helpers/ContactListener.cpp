#include "ContactListener.h"
#include <Physics/Dynamics/Entity/hkpRigidBody.h>	
#include "Globals.h"
#include "WreckingBall.h"
#include "Chain.h"
#include "MathHelper.h"

ContactListener::ContactListener(void)
{
}

ContactListener::ContactListener(Surface *surface, int surfelIndex, int edgeIndex)
{
	this->surface = surface;
	this->surfelIndex = surfelIndex;
	this->edgeIndex = edgeIndex;
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
	/*else if(body->hasProperty(HK_OBJECT_IS_CHAIN))
		u = ((Chain*)(body->getProperty(HK_OBJECT_IS_CHAIN).getPtr()))->GetLastVelocity();
*/
	// F = m*a
	// F = m * ( v - u )/t
	D3DXVECTOR3 v = D3DXVECTOR3(body->getLinearVelocity()(0), body->getLinearVelocity()(1), body->getLinearVelocity()(2));
	force(0) = -body->getMass() * (v[0] - u[0]);
	force(1) = -body->getMass() * (v[1] - u[1]);
	force(2) = -body->getMass() * (v[2] - u[2]);
/*
	if(MathHelper::Sign(v[0]) != MathHelper::Sign(u[0]))
		force(0) *= -1;
	if(MathHelper::Sign(v[1]) != MathHelper::Sign(u[1]))
		force(1) *= -1;
	if(MathHelper::Sign(v[2]) != MathHelper::Sign(u[2]))
		force(2) *= -1;
*/	
	surface->AddForce(D3DXVECTOR3(force(0), force(1), force(2)), D3DXVECTOR3(event.m_contactPoint->getPosition()(0), event.m_contactPoint->getPosition()(1), event.m_contactPoint->getPosition()(2)), surfelIndex, edgeIndex);

}

