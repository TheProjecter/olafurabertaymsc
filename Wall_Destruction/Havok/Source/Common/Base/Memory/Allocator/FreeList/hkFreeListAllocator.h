/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_FREELIST_MEMORY
#define HK_FREELIST_MEMORY

#include <Common/Base/Memory/Allocator/Thread/hkThreadMemory.h>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>
#include <Common/Base/Config/hkConfigMemoryStats.h>
#include <Common/Base/Memory/Allocator/FreeList/hkFreeList.h>
#include <Common/Base/Memory/Allocator/FreeList/hkLargeBlockAllocator.h>

/// This class implements the hkMemory interface, and uses freelists as the basis of 
/// small memory allocations, and the large block allocator for large memory allocations.
///
/// NOTE! Changing the hard memory limit - actually will set the memory limit on the hkMemoryBlockServer
/// that the hkFreeListAllocator uses for underlying memory. This class caches this as the 'hard memory limit' 
/// in the constructor. If you try changing the limit in the server - this will not be seen by the 
/// instance of hkFreeListAllocator and so in general will cause incorrect behaviour. 
/// Therefore change memory limits only thru the setMemoryHardLimit method and not directly on the 
/// server.

class hkFreeListAllocator : public hkMemoryAllocator
{
	public:

		HK_DECLARE_PLACEMENT_ALLOCATOR();

		enum { SOFT_LIMIT_MAX = 0x7fffffff };

			/// Specify the server that we are using
		hkFreeListAllocator(hkMemoryBlockServer* server);

			///
		~hkFreeListAllocator();

		void init( hkMemoryBlockServer* server );
			///
		virtual void quit();

			// hkMemoryAllocator

		virtual void* blockAlloc(int nbytes);

		virtual void blockFree(void*, int nbytes);

		virtual void* bufAlloc(int& reqNumInOut);

		virtual void blockAllocBatch(void** blocksOut, int nblocks, int nbytes);

		virtual void blockFreeBatch(void** blocks, int nblocks, int nbytes);

		virtual void printStatistics(hkOstream* c);

		virtual int getAllocatedSize( const void* p, int nbytes);

		virtual void reservePagesFromSystemMemory(int numBytes);

		virtual void garbageCollect();

		virtual void incrementalGarbageCollect(int numBlocks);

		virtual void optimize();

		virtual void setMemorySoftLimit(hk_size_t maxMemory);

		virtual hk_size_t getMemorySoftLimit();

		virtual bool canAllocTotal(int size);

		virtual void releaseUnusedPagesToSystemMemory( void );

		virtual hkBool isOk() const;

		virtual void getMemoryStatistics( MemoryStatistics& u );

		virtual void resetPeakMemoryStatistics();

		typedef void (HK_CALL *MemoryWalkCallback)(void* start, hk_size_t size,hkBool allocated,int pool,void* param);
		virtual hkResult walkMemory(MemoryWalkCallback callback,void* param);

	protected:


			/// NOTE! The new/delete are special they are designed to use a restricted memory system, such that
			/// memory freed with _deleteFreeList will only be available if it was the last allocation. Ie its organised
			/// like stack memory

			/// Uses the simple contained memory allocation scheme
			/// Also ensures there is only one freelist for each element size
		hkFreeList* _newFreeList(hk_size_t elementSize,hk_size_t alignment,hk_size_t blockSize);

			/// Deletes the freelist, without using up memory
		void _deleteFreeList(hkFreeList* freeList);

		static HK_FORCE_INLINE hkBool _comparePointers( void* a, void* b ) { return (char*)a < (char*)b; }

		bool _hasMemoryAvailable(hk_size_t size);

		enum
		{
				/// The size smallest differential freelist size
			FREELIST_ALIGNMENT = 16,
				/// The maximum memory allocation size that will be handled via a freelist -> the rest go to the large
				/// block allocator
				/// The size is designed to be big enough to hold a complete hkpRigidBody in free list
			MAX_FREELIST_SIZE = hkThreadMemory::MEMORY_MAX_SIZE_SMALL_BLOCK,
				/// Shift to go from a size to a freelist
			FREELIST_SHIFT = 4,
				/// The total amount of size->freelist lookup entries
			MAX_FREELISTS = (MAX_FREELIST_SIZE >> FREELIST_SHIFT)+1,
				/// The total amount of unique freelists that is supported
			MAX_UNIQUE_FREELISTS = MAX_FREELISTS
		};


		mutable hkCriticalSection m_criticalSection;
		hk_size_t m_totalBytesInFreeLists;
		hk_size_t m_peakInUse;

			/// The server being used
		hkMemoryBlockServer* m_server;

			/// The large block allocator
		hkLargeBlockAllocator m_largeAllocator;

			/// Freelists for the small sizes
		hkFreeList* m_sizeToFreeList[MAX_FREELISTS];

		hkFreeList* m_freeLists[MAX_FREELISTS];
		int m_numFreeLists;

			// We want this allocator to be self contained. So the memory to actually hold the freelists is allocated on
			// here. We just use a simple stack style memory allocation scheme to store the freelist objects
		hkFreeList* m_topFreeList;
		hkFreeList* m_lastFreeList;
		hkFreeList m_freeListMemory[MAX_UNIQUE_FREELISTS];

		hk_size_t m_softLimit;

		int m_incrementalFreeListIndex;			

	public:
			/// When HK_DEBUG is defined, returned memory is scrubbed with this value
		static hkUint32 s_fillReturnedToUser;
};


#endif // HK_FREELIST_MEMORY

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
