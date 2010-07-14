#include "ContactListener.h"
#include "Globals.h"

ContactListener::ContactListener(void)
{
}

ContactListener::ContactListener(Surface *surface, int surfelIndex)
{
	this->surface = surface;
	this->surfelIndex = surfelIndex;
}

ContactListener::~ContactListener(void)
{
}


void ContactListener::contactPointCallback( const hkpContactPointEvent& event ){

//	event.accessVelocities(0);

	hkVector4 force = hkVector4(0.0f, 0.0f, 0.0f, 0.0f);
	if(event.m_bodies[0]->isFixed()){
		force.addMul4(event.m_bodies[1]->getMass(), event.m_bodies[1]->getLinearVelocity());
	}
	else {
		force.addMul4(event.m_bodies[0]->getMass(), event.m_bodies[0]->getLinearVelocity());
	}
	
	//Helpers::Globals::DebugInformation.AddText("f = (%f, %f, %f)", force(0), force(1), force(2));
	surface->AddForce(D3DXVECTOR3(force(0), force(1), force(2)), D3DXVECTOR3(event.m_contactPoint->getPosition()(0), event.m_contactPoint->getPosition()(1), event.m_contactPoint->getPosition()(2)), surfelIndex);

/*
	if(event.m_type == hkpContactPointEvent::TYPE_MANIFOLD)
		Helpers::Globals::DebugInformation.AddText("Manifold Bang");
	if(event.m_type == hkpContactPointEvent::TYPE_EXPAND_MANIFOLD)
		Helpers::Globals::DebugInformation.AddText("Expand Manifold Bang");	
 */	
}
