/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_hkMemoryInitUtil_H
#define HKBASE_hkMemoryInitUtil_H

#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>

class hkMemoryAllocator;
class hkThreadMemory;
class hkMemoryRouter;
class hkMemorySystem;
struct hkSingletonInitNode;

	/// Helper methods to initialize and quit the memory system.
	/// These methods exist mainly for brevity in our demos - the memory system initialization
	/// may be inlined in your setup code.
	/// All init methods accept a "base" allocator which defaults to an instance of hkMallocAllocator.
	/// All Havok allocations eventually use memory from this allocator. You may redirect at this coarse
	/// level by passing a custom implementation of hkMemoryAllocator.
namespace hkMemoryInitUtil
{
		/// The default allocator through which all allocations are directed if a
		/// base allocator is not given to one of the init methods.
	extern hkMemoryAllocator* defaultSystemAllocator;

		/// Initialize with an hkFreeListMemorySystem.
	hkMemoryRouter* HK_CALL initFreeList(hkMemoryAllocator* base=defaultSystemAllocator, const hkMemorySystem::FrameInfo& info=hkMemorySystem::FrameInfo() );

		/// Initialize with an hkCheckingMemorySystem.
		/// This checks for leaks and some other common errors and is several orders of magnitude
		/// slower than the free list system.
	hkMemoryRouter* HK_CALL initChecking(hkMemoryAllocator* base=defaultSystemAllocator, const hkMemorySystem::FrameInfo& info=hkMemorySystem::FrameInfo());

		/// Initialize with an hkSimpleMemorySystem.
		/// This is slower than the free list system and is intended as an example.
	hkMemoryRouter* HK_CALL initSimple(hkMemoryAllocator* base=defaultSystemAllocator, const hkMemorySystem::FrameInfo& info=hkMemorySystem::FrameInfo());

		/// Initialize with an hkOptimizerMemorySystem.
		/// This is mainly for internal use to detect where the memory system is being used
		/// sub-optimally.
	hkMemoryRouter* HK_CALL initOptimizer(hkMemoryAllocator* base=defaultSystemAllocator, const hkMemorySystem::FrameInfo& info=hkMemorySystem::FrameInfo());

		/// Will destroy the memory allocator, by calling mainQuit() and destroying the allocator. 
		/// Should only be called if an init call has been made.
	hkResult HK_CALL quit();

		/// Initialize with the default system, currently hkFreeListMemorySystem.
	inline hkMemoryRouter* HK_CALL initDefault(hkMemoryAllocator* base=defaultSystemAllocator, const hkMemorySystem::FrameInfo& info=hkMemorySystem::FrameInfo()) { return initFreeList(base,info); }

		/// When using Checking mem, if you load DLLs dynamically you should let the stacktracer know by calling this function
	void HK_CALL refreshDebugSymbols();

		/// Init the memory tracker
	void HK_CALL initMemoryTracker();
		/// Quit the memory tracker
	void HK_CALL quitMemoryTracker();

		/// Cross dll sync info
	struct SyncInfo
	{
		hkMemoryRouter* m_memoryRouter;
		hkMemorySystem* m_memorySystem;
		hkSingletonInitNode* m_singletonList;
	};
}

#endif // HKBASE_hkMemoryInitUtil_H

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
