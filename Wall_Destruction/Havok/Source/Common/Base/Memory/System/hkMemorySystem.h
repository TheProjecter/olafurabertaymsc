/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_hkMemorySystem_H
#define HKBASE_hkMemorySystem_H

class hkDebugMemorySnapshot;
class hkThreadMemory;

struct hkMemoryPointerInfo;

	/// Interface to high level memory operations.
	/// These operations apply to the system as a whole i.e. initialization
	/// and shutdown, memory statistics and debugging methods.
class hkMemorySystem
{
	public:

		HK_DECLARE_PLACEMENT_ALLOCATOR();

			/// Construction info for hkMemoryRouter::construct()
		struct FrameInfo
		{
			FrameInfo();

				/// The local stack size. 
			int	m_stackSize;

				/// If set, a buffer will be allocated and given to hkSolverAllocator::s_solverAllocator
			int m_solverBufferSize;
		};

			/// Flags for init and quit
		enum FlagBits
		{
				/// Apply to the router instance only
			FLAG_ROUTER = 1,
				/// Apply to all frame local allocators
			FLAG_FRAME = 2,
			FLAG_ALL = FLAG_ROUTER|FLAG_FRAME
		};
		typedef hkFlags<FlagBits, int> Flags;

			/// Used to traverse over all of the memory.
			/// start is the start of a block
			/// size is the size of the block in bytes (note the size may be larger than requested in an allocation)
			/// If allocated is true then the block is allocated, else its free
			/// pool identifies which 'pool' a block of memory belongs to. What that means is implementation specific
			/// param is the parameter passed into the memory walking method
		typedef void (HK_CALL *MemoryWalkCallback)(void* start, hk_size_t size,hkBool allocated,int pool,void* param);

			/// Destructor, usually empty as the real work is done in mainQuit.
		virtual ~hkMemorySystem();

			/// Memory system methods.
			/// These are not normal methods of hkMemoryRouter because each instance is
			/// thread local, but the methods operate on the global system.
			/// The code which initializes the memory system will set these pointers
			/// as only it knows how they should be implemented.
		virtual hkMemoryRouter* mainInit(const FrameInfo& info=FrameInfo(), Flags f=FLAG_ALL) = 0;

			/// Shut down the memory system.
			/// Some implementations wil return HK_FAILURE if leaks are detected.
		virtual hkResult mainQuit(Flags f=FLAG_ALL) = 0;

			/// Method for threads to construct their local instances.
		virtual void threadInit(hkMemoryRouter& r, const char* name, Flags f=FLAG_ALL) = 0;
			/// Method for threads to deinitialize their local instances.
		virtual void threadQuit(hkMemoryRouter& r, Flags f=FLAG_ALL) = 0;


			/// Print memory usage statistics to the given ostream.
		virtual void printStatistics(hkOstream& ostr) = 0;

			/// Traverses all of the memory used by the subsystem (allocated and free). The order that allocations
            /// are returned is undefined. Not all memory allocators can walk memory - if an implementation cannot
            /// it will return HK_FAILURE from this method.			
		virtual hkResult walkMemory(MemoryWalkCallback callback, void* param) = 0;

			/// Try to free any cached memory e.g. in freelists.
		virtual void garbageCollect();

			///
		virtual void advanceFrame() {}

		// Optional debugging interfaces.

			/// Optional interface to search for the originally allocated memory block within
			/// which the supplied memory block is located. This method returns the originally allocated
			/// memory block's base address. It will assert if it cannot find a matching block.
			/// Returns null if the operation is not supported.
		virtual const void* debugFindBaseAddress(const void* p, int nbytes) { return HK_NULL; }

			/// Optional interface to retrieve a snapshot of all allocations.
		virtual hkResult getMemorySnapshot(hkDebugMemorySnapshot& snapshot) { return HK_FAILURE; }

			/// Optional interface to ensure block is not freed until unlocked.
			/// Multiple locks stack. If this method is implemented, attempts to free
			/// this block without first unlocking will assert.
		virtual void debugLockBaseAddress(const void* baseAddress) { }

			/// Optional interface to unlock a previously locked block.
		virtual void debugUnlockBaseAddress(const void* baseAddress) { }

			/// Optional interface to associate a piece of data with an allocation.
			/// The meaning of the tag depends on the caller.
		virtual void debugTagAddress(const void* baseAddress, const void* tag) { }

			/// Get the pointer information assoicated with the ptr. Returns HK_FAILURE if not implemented, or the address wasn't allocated 
			/// by this system.
		virtual hkResult getAllocationCallStack(void* ptr, hkMemoryPointerInfo& info) { return HK_FAILURE; }

			/// Set the global instance pointer
		static void HK_CALL replaceInstance( hkMemorySystem* m );

			/// Get the global instance
		static hkMemorySystem& HK_CALL getInstance();

			/// Get the global instance pointer
		static hkMemorySystem* HK_CALL getInstancePtr();

	public:
		struct MemoryStatistics 
		{
			MemoryStatistics(): m_sizeStackAllocatedFromHeap(0), m_sumBytesInThreadMemory(0), m_solverAllocatedFromHeap(0){}
			hk_size_t	 m_sizeStackAllocatedFromHeap;	///
			hk_size_t	 m_sumBytesInThreadMemory;		///
			hk_size_t	 m_solverAllocatedFromHeap;		///
		};
		virtual void getMemoryStatistics( MemoryStatistics& out ){};

	protected:
		/// Internal utility function to print the memory statistics
		struct _PrintStatisticsInput
		{
			_PrintStatisticsInput() : m_heapAllocator(0), m_sumAllocatedStackMemory(0), m_softLimit(0) {}
			hkMemoryAllocator* m_heapAllocator;
			hkArray<hkThreadMemory*>::Debug m_threadMemories;
			hkArray<hkStackMemory*>::Debug m_stacks;
			hk_size_t				 m_sumAllocatedStackMemory; // the sum of all allocated stacks if m_stacks are not available
			hk_size_t				 m_softLimit;
		};
		void _printStatistics( hkOstream& ostr, const _PrintStatisticsInput& input  );
	protected:

		static hkMemorySystem* s_instance;
};

#endif // HKBASE_hkMemorySystem_H

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
