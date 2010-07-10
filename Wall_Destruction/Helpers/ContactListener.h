#ifndef COLLISION_LISTENER_H
#define COLLISION_LISTENER_H

#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>
#include <Physics/Dynamics/Collide/hkpSimpleConstraintContactMgr.h>
#include "Surface.h"

class ContactListener : public hkpContactListener
{
public:
	ContactListener(void);
	ContactListener(Surface *surface, int surfelIndex);
	~ContactListener (void);

	void contactPointCallback( const hkpContactPointEvent& event );

private :
	Surface *surface;
	int surfelIndex;
};

#endif