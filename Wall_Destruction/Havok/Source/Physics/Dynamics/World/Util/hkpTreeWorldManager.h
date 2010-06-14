/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKP_TREE_WORLD_MANAGER_H
#define HKP_TREE_WORLD_MANAGER_H

#include <Common/Internal/Collide/Tree/AabbTree/hkAabbTreeData.h>

class hkJobThreadPool;
class hkJobQueue;

struct hkpWorldRayCastInput;
struct hkpWorldRayCastOutput;
class hkpRayHitCollector;
struct hkpLinearCastInput;
class hkpCdPointCollector;
class hkpCollisionFilter;

class hkKdTree;
template<typename BOUNDING_VOLUME, typename IDXTYPE>
class hkAabbTree;

class hkpTreeWorldManager : public hkReferencedObject
{
	public:
		hkpTreeWorldManager(){m_inactiveTree = HK_NULL; m_kdTree = HK_NULL;}
		virtual void updateFromWorld(hkJobQueue* jobQueue = HK_NULL, hkJobThreadPool* jobThreadPool = HK_NULL) = 0;

		virtual bool isUpToDate() const = 0;
		virtual void setUpToDate(bool upToDate) = 0;

		//
		// Raycasting and linearcasting interface.
		// The methods are analogical to those in hkpWorld.
		//

		/// Cast a ray into the world and get the closest hit.
		virtual void castRay(const hkpWorldRayCastInput& input, const hkpCollisionFilter* filter, hkpWorldRayCastOutput& output ) const = 0;

		/// Cast a ray into the world and do a callback for every hit.
		virtual void castRay(const hkpWorldRayCastInput& input, const hkpCollisionFilter* filter, hkpRayHitCollector& collector ) const = 0;

		/// Cast a shape within the world.
		/// The castCollector collects all potential hits.
		virtual void linearCast(const hkpCollidable* collA, 
			const struct hkpLinearCastInput& input, const class hkpCollidableCollidableFilter* filter,
			const struct hkpCollisionInput& collInput, struct hkpCollisionAgentConfig* config,
			class hkpCdPointCollector& castCollector, class hkpCdPointCollector* startPointCollector ) = 0;

		/// Calculate required working buffer size.
		/// The buffer have to be passed by set WorkingBuffer
		virtual int calcWorkingBufferSize(int maxObjects){return 0;}

		/// If aabbTreeWorldManager is used, this buffer have to be set before tree build. 
		virtual void setWorkingBuffer(void* ptr){}

		/// Get KdTree. 
		const hkKdTree* getKdTree() const{return m_kdTree;}

		/// Get AabbTree, which is used for inactive objects if aabbTreeWorldManager is used. Otherwise returns HK_NULL. 
		const hkAabbTree<hkAabbTreeData::hkAabbTreeAabb16, hkUint16>* getAabbTree() const{return m_inactiveTree;}

		enum Type
		{
			MANAGER_KDTREE,
			MANAGER_AABBTREE,
		};

		Type m_type;

//		hkAabbTree<hkAabbTreeData::hkAabbTreeAabb16>* m_inactiveTree;
		hkAabbTree<hkAabbTreeData::hkAabbTreeAabb16, hkUint16>* m_inactiveTree;
		hkKdTree* m_kdTree;

};


#endif

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
