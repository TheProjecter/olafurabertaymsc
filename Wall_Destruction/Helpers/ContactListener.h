#ifndef COLLISION_LISTENER_H
#define COLLISION_LISTENER_H

#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>
#include <Physics/Dynamics/Collide/hkpSimpleConstraintContactMgr.h>

#ifndef SURFACE_H
	#include "Surface.h"
#endif

class ContactListener : public hkpContactListener
{
public:
	ContactListener(void);
	ContactListener(ProjectStructs::SURFEL* surfel);
	~ContactListener (void);

	void contactPointCallback( const hkpContactPointEvent& event );

private :

	void AddForce(D3DXVECTOR3 force, D3DXVECTOR3 pos, ProjectStructs::SURFEL* surfel);
	void AddForceToPhyxels(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL *surfel);
	bool AddForceToPhyxel(D3DXVECTOR3 force, D3DXVECTOR3 pos, D3DXVECTOR3 direction, ProjectStructs::PHYXEL_NODE *phyxel, ProjectStructs::SURFEL* surfel);

	ProjectStructs::SURFEL* surfel;
	D3DXVECTOR3 surfacePos;
};

#endif