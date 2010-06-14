/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_BASE_CONTAINER_ALLOCATORS_H
#define HK_BASE_CONTAINER_ALLOCATORS_H

// These helper classes exist to allow containers to use different allocators
// without necessarily storing the allocator in the container.

#if !defined(HK_PLATFORM_PS3_SPU)

		/// Forwards to the temp allocator
	struct hkContainerTempAllocator
	{
		hkMemoryAllocator& get(void*) { return hkMemoryRouter::getInstance().temp(); }
	};

		/// Forwards to the heap allocator
	struct hkContainerHeapAllocator
	{
		hkMemoryAllocator& get(const void*) { return hkMemoryRouter::getInstance().heap(); }
	};

		/// Forwards to the debug allocator
	struct hkContainerDebugAllocator
	{
		hkMemoryAllocator& get(void*) { return hkMemoryRouter::getInstance().debug(); }
	};

		/// Forwards to an allocator stored at OFFSET bytes within the container itself.
	template<int OFFSET>
	struct hkContainerStoredAllocator
	{
		hkMemoryAllocator& get(void* p) { return *reinterpret_cast<hkMemoryAllocator*>( hkAddByteOffset(p, OFFSET) ); }
	};

#else

	struct hkContainerHeapAllocator
	{
		hkMemoryAllocator& get(void*) { return hkThreadMemorySpu::getInstance(); }
	};
	typedef hkContainerHeapAllocator hkContainerDebugAllocator;
	typedef hkContainerHeapAllocator hkContainerTempAllocator;
#endif

#endif // HK_BASE_CONTAINER_ALLOCATORS_H

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
