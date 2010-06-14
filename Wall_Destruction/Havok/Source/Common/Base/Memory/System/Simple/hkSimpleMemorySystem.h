/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_hkSimpleMemorySystem_H
#define HKBASE_hkSimpleMemorySystem_H

#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/Solver/hkSolverAllocator.h>

	/// Sample memory system which forwards all requests to a single base allocator.
	/// This implementation won't perform well because of contention and has no
	/// provisions for dealing with fragmentation.
class hkSimpleMemorySystem : public hkMemorySystem
{
	public:

			///
		hkSimpleMemorySystem();

		virtual hkMemoryRouter* mainInit(const FrameInfo& info=FrameInfo(), Flags f=FLAG_ALL);
		virtual hkResult mainQuit(Flags f=FLAG_ALL);
		virtual void threadInit(hkMemoryRouter& r, const char* name, Flags f=FLAG_ALL);
		virtual void threadQuit(hkMemoryRouter& r, Flags f=FLAG_ALL);

			/// Not implemented
		virtual hkResult walkMemory(MemoryWalkCallback callback, void* param) { return HK_FAILURE; }

			/// Forwards request to the base allocator.
		virtual void printStatistics(hkOstream& ostr);

			/// Get the base allocator.
		hkMemoryAllocator* getAllocator() { return m_allocator; }

			/// Set the base allocator.
		void setAllocator(hkMemoryAllocator* a) { HK_ASSERT(0x62b32838, m_allocator==HK_NULL || a == HK_NULL ); m_allocator = a; }

	protected:

		FrameInfo m_frameInfo;
		hkMemoryAllocator* m_allocator;
		hkMemoryRouter m_mainRouter;
		hkSolverAllocator m_solverAllocator;
};

#endif // HKBASE_hkSimpleMemorySystem_H

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
