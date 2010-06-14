/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_hkMemoryAllocator_H
#define HKBASE_hkMemoryAllocator_H

#define HK_DECLARE_PLACEMENT_ALLOCATOR() \
	HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t, void* p) { return p; } \
	HK_FORCE_INLINE void HK_CALL operator delete(void*, void*) {} \
	HK_FORCE_INLINE void HK_CALL operator delete(void*) {}

// trick to have hkDeallocate() work with a void* argument (sizeof(void) is undefined, but makes sense to be 1)
template<typename T> struct hkSizeOfTypeOrVoid { enum { val = sizeof(T) }; };
template<> struct hkSizeOfTypeOrVoid<void> { enum { val = 1 }; };

	/// Base class for all memory allocation implementations.
	/// A derived class may simply override blockAlloc and blockFree. In some
	/// cases the allocator may be able to supply more memory than requested
	/// or sometimes be able to reallocate without copying. If so, the buf* methods
	/// may be overridden for efficiency.
class hkMemoryAllocator
{
	public:

		HK_DECLARE_PLACEMENT_ALLOCATOR();

			/// A struct holding memory allocation information.
		struct MemoryStatistics
		{
			MemoryStatistics(): m_allocated(0), m_inUse(0), m_peakInUse(0), m_available(0), m_totalAvailable(0), m_largestBlock(0){}

				/// Infinite as far as the memory system is concerned.
			static const hk_size_t INFINITE_SIZE = (hk_size_t(0) - 1);

				/// Total amount of allocated memory. Allocated memory means that it is 'owned' by the hkMemory reporting it -
				/// but the actual memory inside may be available or used. Thus m_used + m_available <= m_allocated. The difference will 
				/// be due to overhead for managing the memory.
			hk_size_t m_allocated;

				/// Total used memory
			hk_size_t m_inUse;

				/// The peak usage
			hk_size_t m_peakInUse;

				/// Total available from the _allocated_ memory. This is NOT the total amount of memory that is potentially available,
				/// only the amount that remains available from what has been allocated.
			hk_size_t m_available;

				/// The approx total amount of memory that is available. The value is the amount of available memory (m_available)
				/// plus the approx amount of memory that is potentially available but not yet allocated.
				/// Depending on the allocator being used, this value may be meaningless. For example for an allocator with virtual memory
				/// the 'total amount of available' is somewhat meaningless. In those scenarios, m_totalMemory will be INFINITE_SIZE
			hk_size_t m_totalAvailable;

				/// Largest block available. On some memory allocators this is meaningless (for example with virtual memory). In that
				/// case this will be set to INFINITE_SIZE
			hk_size_t m_largestBlock;
		};


			///
		virtual ~hkMemoryAllocator();

		
			/// Allocate numBytes.
		virtual void* blockAlloc( int numBytes ) =0;

			/// Free numBytes.
		virtual void blockFree( void* p, int numBytes ) =0;

			/// Request a buffer of reqNumBytesInOut.
			/// The implementation may modify reqNumInOut to return a larger
			/// block than requested.
			/// Default implementation forwards to blockAlloc.
		virtual void* bufAlloc( int& reqNumBytesInOut );

			/// Free a buffer.
			/// Default implementation forwards to blockFree.
			/// \a numElem is the number of elements as returned by the bufAlloc in reqNumInOut
		virtual void bufFree( void* p, int numBytes );

			/// Reallocate a buffer.
			/// Default implementation is blockAlloc, memCpy, blockFree.
		virtual void* bufRealloc( void* pold, int oldNumBytes, int& reqNumBytesInOut );
		

			/// Allocate several blocks at the same time.
			/// May be more efficient than individual allocations if the allocator must be locked.
		virtual void blockAllocBatch(void** ptrsOut, int numPtrs, int blockSize);

			/// Free several blocks at the same time.
		virtual void blockFreeBatch(void** ptrsIn, int numPtrs, int blockSize);

			/// Returns true if a certain allocation can be done.
			/// Currently only used and implemented for the SolverAllocator
		virtual bool canAllocSingleBlock( int numBytes );

			/// Returns true if a total amount of memory of at least 'size' is available.
		virtual bool canAllocTotal( int numBytes );

			/// Work out memory statistics. This function tries to work out fairly accurately details about memory usage -
			/// this functions performance may not be great depending on the underlying implementation.
			/// See hkMemoryStatisticcs for details of the information returned.
			/// NOTE! That if you have a memory limit set (via setMemoryLimit) the values returned will reflect that limit
			/// you must have the limit set to zero if you want to find out the 'true' memory statistics.
		virtual void getMemoryStatistics( MemoryStatistics& u )= 0;

			/// Return the padded size. Most memory system will round up the allocation size or add extra headers and footers.
			/// This function should return the (padded) size which potentially is useable by the user
			///
			/// The size (and any size between the original allocated size, and the value returned from getAllocatedSize) can be passed
			/// to this method, and it will produce the same result. Also note the block/bufFree can take any value between the original
			/// allocated size, and the size returned from getAllocatedSize, and work correctly.
		virtual int getAllocatedSize(const void* obj, int nbytes) = 0;

			/// Reset internal counts of mem usage.
		virtual void resetPeakMemoryStatistics() {}

			/// Allocate a buffer which is a multiple of SIZE_ELEM.
		template<typename TYPE>
		inline TYPE* _bufAlloc( int& reqNumInOut )
		{
			const int SIZE_ELEM = hkSizeOfTypeOrVoid<TYPE>::val;
			int n = reqNumInOut * SIZE_ELEM;
			void* p = bufAlloc( n );
			reqNumInOut = n / SIZE_ELEM;
			return static_cast<TYPE*>(p);
		}

			/// Free a buffer which is a multiple of SIZE_ELEM.
		template<typename TYPE>
		inline void _bufFree( void* p, int numElem )
		{
			const int SIZE_ELEM = hkSizeOfTypeOrVoid<TYPE>::val;
			bufFree( p, numElem * SIZE_ELEM );
		}

			/// Reallocate a buffer which is a multiple of SIZE_ELEM.
		template<typename TYPE>
		inline TYPE* _bufRealloc( void* pold, int oldNum, int& reqNumInOut )
		{
			const int SIZE_ELEM = hkSizeOfTypeOrVoid<TYPE>::val;
			int n = reqNumInOut*SIZE_ELEM;
			void* p = bufRealloc( pold, oldNum*SIZE_ELEM, n );
			reqNumInOut = n / SIZE_ELEM;
			return static_cast<TYPE*>(p);
		}

			/// Allocate a block to hold n TYPEs
		template<typename TYPE>
		inline TYPE* _blockAlloc( int n )
		{
			return static_cast<TYPE*>( blockAlloc(n*hkSizeOfTypeOrVoid<TYPE>::val) );
		}
			/// Free a block of n TYPEs
		template<typename TYPE>
		inline void _blockFree( TYPE* p, int n )
		{
			blockFree( p, n*hkSizeOfTypeOrVoid<TYPE>::val );
		}

	public:

		/// Status of the memory manager.
		enum MemoryState
		{
			MEMORY_STATE_OK,			///
			MEMORY_STATE_OUT_OF_MEMORY	///
		};
};

#endif // HKBASE_hkMemoryAllocator_H

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
