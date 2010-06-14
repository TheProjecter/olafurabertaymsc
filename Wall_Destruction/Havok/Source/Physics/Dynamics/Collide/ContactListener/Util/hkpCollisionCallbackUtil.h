/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <Physics/Dynamics/Constraint/hkpConstraintListener.h>
#include <Physics/Dynamics/World/Listener/hkpWorldDeletionListener.h>
#include <Physics/Dynamics/Collide/ContactListener/Util/hkpEndOfStepCallbackUtil.h>

// This is a constraint listener which will fire hkpCollisionAdded/RemovedCallbacks as a
// contact constraint is added or removed from the world.
class hkpCollisionCallbackUtil : public hkReferencedObject, public hkpConstraintListener, public hkpWorldDeletionListener
{
	public:
		HK_DECLARE_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DYNAMICS );

			/// Add a hkpCollisionCallbackUtil to the world if one has not
			/// been added already. Increments m_count otherwise.
		static void HK_CALL addCollisionCallbackUtil( hkpWorld* world );

			/// Decrement the count of times a hkpCollisionCallbackUtil has
			/// been added to the world. It is removed when the m_count reaches zero.
			/// (To force removal, set m_count to 1 before calling this function.)
		static void HK_CALL removeCollisionCallbackUtil( hkpWorld* world );

	public:
		// hkpConstraintListener interface.
		virtual void constraintAddedCallback( hkpConstraintInstance* constraint );
		virtual void constraintRemovedCallback( hkpConstraintInstance* constraint );

		// hkpWorldDeletionListener interface.
		virtual void worldDeletedCallback( hkpWorld* world );

	public:
			/// The number of times this util has been added to the world.
		hkUint32 m_count;

			/// For convenience, this class contains the default hkpEndOfStepCallbackUtil.
		hkpEndOfStepCallbackUtil m_endOfStepCallbackUtil;

	protected:
			/// Private constructor.
		hkpCollisionCallbackUtil() : m_count( 1 ) {}
};

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
