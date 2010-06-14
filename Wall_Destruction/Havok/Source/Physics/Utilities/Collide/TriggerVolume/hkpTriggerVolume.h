/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_TRIGGER_VOLUME_H
#define HK_TRIGGER_VOLUME_H

#include <Physics/Dynamics/Collide/ContactListener/hkpContactListener.h>
#include <Physics/Dynamics/World/Listener/hkpWorldPostSimulationListener.h>
#include <Physics/Dynamics/Entity/hkpEntityListener.h>

/// This is a super-class for trigger volumes, which are rigid bodies that record collision events, but have no physical effect.
/// Passing a rigid body into the constructor of a new TriggerVolume converts it into a trigger volume and no further work need be done.
/// The object manages its own memory and keeps itself alive so long as its triggerBody is alive.
/// The trigger volume sets the world to fire collisionConstraintAdded/Removed callbacks for its duration.
class hkpTriggerVolume : public hkReferencedObject, public hkpContactListener, public hkpWorldPostSimulationListener, public hkpEntityListener
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_UTILITIES );

			/// Convert the given rigidBody into a triggerVolume.
		hkpTriggerVolume( hkpRigidBody* triggerBody );

			/// Called at the end of each physics step for any body which came in contact with the triggerVolume
			/// during this frame.
			/// You can use the m_overlappingBodies array to see if the body is still touching.
		virtual void enterEvent( hkpRigidBody* body ) = 0;
			
			/// Called at the end of each physics step for any body which left the triggerVolume in this frame.
			/// You can use the m_overlappingBodies array to see if the body is still touching.
		virtual void leaveEvent( hkpRigidBody* body ) = 0;

			/// Destructor.
		virtual ~hkpTriggerVolume();

	public:
		// hkpContactListener interface.
		virtual void contactPointCallback( const hkpContactPointEvent& event );
		virtual void collisionAddedCallback( const hkpCollisionEvent& event );
		virtual void collisionRemovedCallback( const hkpCollisionEvent& event );

		// hkpWorldPostSimulationListener interface.
		void postSimulationCallback( hkpWorld* world );

		// hkpEntityListener interface.
		virtual void entityDeletedCallback( hkpEntity* entity );
		virtual void entityAddedCallback( hkpEntity* entity );
		virtual void entityRemovedCallback( hkpEntity* entity );

	public:
			/// An array of bodies containing all of those which have entered during this frame, but also some
			/// bodies which should be discounted because their collisions with the triggerBody were invalidated
			/// by continuous physics.
		hkArray<hkpRigidBody*> m_enteringBodies;

			/// The array of bodies which touched the triggerBody at some point during continuous physics.
		hkArray<hkpRigidBody*> m_confirmedBodies;

			/// The array of bodies in contact with the triggerBody at the end of the frame.
		hkArray<hkpRigidBody*> m_overlappingBodies;
		
			/// The array of bodies which have stopped colliding with the triggerBody in this frame.
		hkArray<hkpRigidBody*> m_leavingBodies;

			/// The trigger body.
		hkpRigidBody* m_triggerBody;
};

#endif // HK_TRIGGER_VOLUME_H

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20091222)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/
