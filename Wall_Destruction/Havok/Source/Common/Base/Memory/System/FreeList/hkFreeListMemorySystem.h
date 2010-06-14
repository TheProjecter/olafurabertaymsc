/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_hkFreeListSystem_H
#define HKBASE_hkFreeListSystem_H

#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Memory/Allocator/FreeList/SystemMemoryBlockServer/hkSystemMemoryBlockServer.h>
#include <Common/Base/Memory/Allocator/FreeList/hkFreeListAllocator.h>
#include <Common/Base/Memory/Allocator/Solver/hkSolverAllocator.h>
#include <Common/Base/Memory/Allocator/Checking/hkLeakDetectAllocator.h>


	///  A memory system which wraps the free list,
	///  It also wraps the heap memory with hkThreadMemory to speed up thread local memory allocs/frees
class hkFreeListMemorySystem : public hkMemorySystem
{
	public:

		enum { THREAD_MAX=32 };

			/// Some information for each thread.
		struct ThreadData
		{
			ThreadData() : m_name(HK_NULL), m_inUse(false), m_stackMemoryAllocated(0) {}
			hkThreadMemory m_heapThreadMemory;				///
			//hkThreadMemory m_tempThreadMemory;			///
			hkMemoryRouter* m_memoryRouter;					///
			const char*		m_name;							///
			hkBool			m_inUse;						///

			hk_size_t		m_stackMemoryAllocated;			///
			hkStackMemory*  m_stackMemory;					///
		};

		hkFreeListMemorySystem();
		~hkFreeListMemorySystem();

		virtual hkMemoryRouter* mainInit(const FrameInfo& info=FrameInfo(), Flags f=FLAG_ALL);
		virtual hkResult mainQuit(Flags f=FLAG_ALL);
		virtual void threadInit(hkMemoryRouter& r, const char* name, Flags f=FLAG_ALL);
		virtual void threadQuit(hkMemoryRouter& r, Flags f=FLAG_ALL);
		virtual hkResult walkMemory(MemoryWalkCallback callback, void* param);
		virtual void garbageCollect();

		hkMemoryAllocator* getAllocator() { return m_systemAllocator; }
		void setAllocator(hkMemoryAllocator* a);

		virtual void getMemoryStatistics( MemoryStatistics& out );
		virtual void printStatistics(hkOstream& ostr);
		virtual hkResult getMemorySnapshot(hkDebugMemorySnapshot& snapshot);

	protected:

		ThreadData& newThread(const char* name);


	protected:

			//
			//	Some buffers, they should not be used directly
			//

			/// Next 3 members are hierarchy to "System" memory
			// The order is important for the destructor
		hkMemoryAllocator* m_systemAllocator;
		hkSystemMemoryBlockServer  m_freeListMemoryServer;
		hkFreeListAllocator		   m_freeListAllocator;

			/// An optional wrapper for the heap, which allows for checking the integrity of the memory reporting
		hkLeakDetectAllocator		m_leakDetectAllocator;

		FrameInfo m_frameInfo;

			//
			//	Normal Member variables
			//

			/// The heap allocator which should be used, points either to m_freeListMemory or m_leakDetectAllocator
		hkMemoryAllocator* m_heapAllocator;
			

			/// The main threads router
		hkMemoryRouter m_mainRouter;

			/// The solver memory, shared between all threads
		hkSolverAllocator m_solverAllocator;

			///	Thread local memory stuff
		ThreadData m_threadData[THREAD_MAX];

			/// Lock for thread local data
		hkCriticalSection m_threadDataLock;
};

#endif // HKBASE_hkFreeListSystem_H

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
