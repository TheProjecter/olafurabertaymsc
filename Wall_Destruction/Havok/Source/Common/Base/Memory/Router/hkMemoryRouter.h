/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_hkMemoryRouter_H
#define HKBASE_hkMemoryRouter_H

#include <Common/Base/Config/hkConfigThread.h>
#include <Common/Base/Thread/Thread/hkThreadLocalData.h>
#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>
#include <Common/Base/Memory/Allocator/hkMemoryAllocator.h>

#if defined(HK_DEBUG_SPU) || defined(HK_DEBUG)
#	define HK_ERROR(a,b) { HK_BREAKPOINT(0); }
#	define HK_ASSERT2(a,b,c) { if(!(b)) HK_BREAKPOINT(0); }
#	define HK_CRITICAL_ASSERT2(a,b,c) { if(!(b)) HK_BREAKPOINT(0); }
#else
#	define HK_ERROR(a,b)
#	define HK_ASSERT2(a,b,c)
#	define HK_CRITICAL_ASSERT2(a,b,c)
#endif
#include <Common/Base/Memory/Util/Stack/hkStackMemory.h>
#undef HK_CRITICAL_ASSERT2
#undef HK_ASSERT2
#undef HK_ERROR

class hkOstream;

	/// All havok allocations are routed through hkMemoryRouter.
	/// Generally, each thread has a thread-local hkMemoryRouter instance which
	/// is initialized by the hkMemorySystem.
class hkMemoryRouter
{
	public:

		HK_DECLARE_PLACEMENT_ALLOCATOR();

		typedef hkMemoryAllocator Allocator;

			/// Set the thread-local instance pointer to the supplied router.
			/// Note that instances are not ref counted and the caller is responsible for cleanup.
		static void HK_CALL replaceInstance( hkMemoryRouter* m );

			/// Access the thread-local instance.
		static hkMemoryRouter& HK_CALL getInstance();

			/// Access the thread-local instance.
		static hkMemoryRouter* HK_CALL getInstancePtr();

			/// Construct an empty object.
		hkMemoryRouter();

			///
		void resetPeakMemoryStatistics();

			/// Small thread local allocation in a stack pattern.
		inline hkStackMemory& stack() { return m_stack; }

			/// Temporary allocation, thread local. Often but not always in a stack pattern.
		inline Allocator& temp() { return *m_temp; }
			///
		inline void setTemp(Allocator* a) { m_temp = a; }

			/// Allocation which is likely to persist. Not thread local.
		inline Allocator& heap() { return *m_heap; }
			///
		inline void setHeap(Allocator* a) { m_heap = a; }

			/// Allocation which may break the rules. Not thread local.
		inline Allocator& debug() { return *m_debug; }
			///
		inline void setDebug(Allocator* a) { m_debug = a; }

			/// Allocator for the constraint solver. These may request large contiguous and frame local blocks.
		inline Allocator& solver() { return *m_solver; }
			///
		inline void setSolver(Allocator* a) { m_solver = a; }

			/// Allocator shared between all threads.
		inline Allocator& global() { return *m_global; }
			///
		inline void setGlobal(Allocator* a) { m_global = a; }

			/// User data pointer
		inline void* getUserData() const { return m_userData; }
			///
		inline void setUserData(void* p) { m_userData = p; }

			/// Helper to align an allocation request, note that the corresponding free method must be used.
		static void* HK_CALL alignedAlloc( Allocator& b, int nbytes, int align );

			/// Free a block allocated with alignedAlloc.
		static void HK_CALL alignedFree( Allocator& b, void* p );

			/// Helper to make an allocation which remembers the size internally.
		static void* HK_CALL easyAlloc( Allocator& b, int nbytes );

			/// Free a block allocated with easyAlloc.
		static void HK_CALL easyFree( Allocator& b, void* p );

	protected:

		hkStackMemory m_stack;
		Allocator* m_temp;		///
		Allocator* m_heap;	///
		Allocator* m_debug;		///
		Allocator* m_solver;	///
		Allocator* m_global;	///
		void* m_userData;
};


#if defined(HK_MEMORY_TRACKER_ENABLE)
#include <Common/Base/Memory/Tracker/hkMemoryTracker.h>
	#define HK_MEMORY_TRACKER_ON_NEW_REFOBJECT(NBYTES, OBJ) hkMemoryTracker::getInstance().onNewReferencedObject(NBYTES, OBJ)
	#define HK_MEMORY_TRACKER_ON_DELETE_REFOBJECT(OBJ) hkMemoryTracker::getInstance().onDeleteReferencedObject(OBJ)
	#define HK_MEMORY_TRACKER_ON_NEW_OBJECT(CLASS_TYPE, NBYTES, OBJ) hkMemoryTracker::getInstance().onNewObject(typeid(CLASS_TYPE), NBYTES, OBJ);
	#define HK_MEMORY_TRACKER_ON_DELETE_OBJECT(OBJ) hkMemoryTracker::getInstance().onDeleteObject(OBJ);
#else
	#define HK_MEMORY_TRACKER_ON_NEW_REFOBJECT(NBYTES, OBJ)
	#define HK_MEMORY_TRACKER_ON_DELETE_REFOBJECT(OBJ)
	#define HK_MEMORY_TRACKER_ON_NEW_OBJECT(CLASS_TYPE, NBYTES, OBJ)
	#define HK_MEMORY_TRACKER_ON_DELETE_OBJECT(OBJ)
#endif

#if defined(HK_COMPILER_MWERKS)
#	define HK_OPERATOR_DELETE
#	define HK_OPERATOR_NONVIRTUAL_DELETE
#else
#	define HK_OPERATOR_DELETE \
		HK_FORCE_INLINE void  HK_CALL operator delete(void*, void*)		{ }	\
		HK_FORCE_INLINE void  HK_CALL operator delete[](void*, void*)	{ HK_BREAKPOINT(0);	}
#	define HK_OPERATOR_NONVIRTUAL_DELETE \
		HK_FORCE_INLINE void  HK_CALL operator delete(void*, void*)		{ } \
		HK_FORCE_INLINE void  HK_CALL operator delete[](void*, void*)	{ }
#endif

#	define HK_MUST_END_WITH_SEMICOLON class MustEndWithSemiColon

#if defined( HK_PLATFORM_PS3_SPU)
#	define HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE, ALLOCATOR) \
		HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes)	{ static int q; return &q; }	\
		HK_FORCE_INLINE void  HK_CALL operator delete(void* p)			{ } 	\
		HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
		HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p){ return p; }	\
		HK_OPERATOR_DELETE \
		HK_MUST_END_WITH_SEMICOLON
#else
#	define HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(MEMORY_CLASS, ALLOCATOR) \
		HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes) { \
			HK_ASSERT_OBJECT_SIZE_OK(nbytes); \
			hkReferencedObject* b = static_cast<hkReferencedObject*>(hkMemoryRouter::getInstance().ALLOCATOR().blockAlloc(static_cast<int>(nbytes))); \
			b->m_memSizeAndFlags = static_cast<hkUint16>(nbytes); \
			HK_MEMORY_TRACKER_ON_NEW_REFOBJECT(nbytes, b); \
			return b; }	\
		HK_FORCE_INLINE void  HK_CALL operator delete(void* p) { \
			hkReferencedObject* b = static_cast<hkReferencedObject*>(p); \
			HK_MEMORY_TRACKER_ON_DELETE_REFOBJECT(b); \
			hkMemoryRouter::getInstance().ALLOCATOR().blockFree(p, b->m_memSizeAndFlags); }	\
			HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t, void* p)	{ return p; }	\
			HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p){ HK_BREAKPOINT(0); return p; }	\
			HK_OPERATOR_DELETE \
			HK_MUST_END_WITH_SEMICOLON
#	define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, CLASS_TYPE, ALLOCATOR) \
		HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t nbytes) { \
			HK_ASSERT(0x6c787b7f, nbytes == sizeof(CLASS_TYPE) ); \
			void* obj = hkMemoryRouter::getInstance().ALLOCATOR().blockAlloc(static_cast<int>(nbytes)); \
			HK_MEMORY_TRACKER_ON_NEW_OBJECT(CLASS_TYPE, nbytes, obj); \
			return obj; }	\
		HK_FORCE_INLINE void  HK_CALL operator delete(void* p) { \
			if (p) { \
				HK_MEMORY_TRACKER_ON_DELETE_OBJECT(p); \
				hkMemoryRouter::getInstance().ALLOCATOR().blockFree(p, sizeof(CLASS_TYPE)); \
			} }\
		HK_FORCE_INLINE void* HK_CALL operator new(hk_size_t n, void* p){ HK_ASSERT(0x77bb90a1, n == sizeof(CLASS_TYPE)); return p; } \
		HK_FORCE_INLINE void* HK_CALL operator new[](hk_size_t, void* p){ return p;	} \
		HK_OPERATOR_NONVIRTUAL_DELETE \
		HK_MUST_END_WITH_SEMICOLON
#endif

//
// In debug, use some compile time trickery to ensure correct allocator is used.
// Use -Wshadow to catch nonvirtual allocators in virtual classes.
//
#if defined(HK_DEBUG)

#   define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
        HK_FORCE_INLINE const hkReferencedObject* iMustBeDerivedFromReferencedObject() const { return static_cast<const hkReferencedObject*>(this); } \
        HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE, heap)
#   define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(MEMORY_CLASS, THIS_CLASS) \
        HK_FORCE_INLINE void iShouldNotHaveVtable() const { int m_memSizeAndFlags = 0; m_memSizeAndFlags--; /* if you get this error, you derive from hkReferencedObject! */ } \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, THIS_CLASS, heap)

#   define HK_DECLARE_CLASS_DEBUG_ALLOCATOR(TYPE) \
        HK_FORCE_INLINE const hkReferencedObject* iMustBeDerivedFromReferencedObject() const { return static_cast<const hkReferencedObject*>(this); } \
        HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE, debug)
#   define HK_DECLARE_NONVIRTUAL_CLASS_DEBUG_ALLOCATOR(MEMORY_CLASS, THIS_CLASS) \
        HK_FORCE_INLINE void iShouldNotHaveVtable() const { int m_memSizeAndFlags = 0; m_memSizeAndFlags--; /* if you get this error, you derive from hkReferencedObject! */ } \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, THIS_CLASS, debug)


#else

#   define HK_DECLARE_CLASS_ALLOCATOR(TYPE) \
		HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE, heap)
#   define HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(MEMORY_CLASS,THIS_CLASS) \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, THIS_CLASS, heap)

#   define HK_DECLARE_CLASS_DEBUG_ALLOCATOR(TYPE) \
		HK_DECLARE_CLASS_ALLOCATOR_UNCHECKED(TYPE, debug)
#   define HK_DECLARE_NONVIRTUAL_CLASS_DEBUG_ALLOCATOR(MEMORY_CLASS,THIS_CLASS) \
        HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR_BY_SIZE_UNCHECKED(MEMORY_CLASS, THIS_CLASS, debug)
#endif

// Deprecated functions

	/// Deprecated, use one of the hkMem*Alloc functions.
	template <typename TYPE>
	HK_FORCE_INLINE TYPE* HK_CALL hkAllocate(int numberOfObjects, int cl)
	{
		hkMemoryRouter& a = hkMemoryRouter::getInstance();
		return static_cast<TYPE*>( hkMemoryRouter::easyAlloc( a.heap(), numberOfObjects*hkSizeOfTypeOrVoid<TYPE>::val ));
	}

	/// Convenience inline function to deallocate memory of the correct type
	template <typename TYPE>
	HK_FORCE_INLINE void HK_CALL hkDeallocate(TYPE* ptr)
	{
		hkMemoryRouter& a = hkMemoryRouter::getInstance();
		hkMemoryRouter::easyFree( a.heap(), static_cast<void *>(ptr));
	}

	/// Convenience inline function to allocate aligned memory of the correct type
	template <typename TYPE>
	HK_FORCE_INLINE TYPE* HK_CALL hkAlignedAllocate(int alignment, int numberOfObjects, int cl)
	{
		hkMemoryRouter& a = hkMemoryRouter::getInstance();
		return static_cast<TYPE*>( hkMemoryRouter::alignedAlloc(a.heap(), numberOfObjects*hkSizeOfTypeOrVoid<TYPE>::val, alignment));
	}

	/// Convenience inline function to deallocate memory of the correct type
	template <typename TYPE>
	HK_FORCE_INLINE void HK_CALL hkAlignedDeallocate(TYPE* ptr)
	{
		hkMemoryRouter& a = hkMemoryRouter::getInstance();
		hkMemoryRouter::alignedFree( a.heap(), static_cast<void *>(ptr));
	}

	/// Convenience function to allocate memory of the correct type
	template <typename TYPE>
	HK_FORCE_INLINE TYPE* HK_CALL hkAllocateChunk(int numberOfObjects, int cl)
	{
		return static_cast<TYPE*>(hkMemoryRouter::getInstance().heap().blockAlloc(numberOfObjects*hkSizeOfTypeOrVoid<TYPE>::val));
	}

	/// Convenience function to deallocate memory of the correct type
	template <typename TYPE>
	HK_FORCE_INLINE void HK_CALL hkDeallocateChunk(TYPE* ptr, int numberOfObjects, int cl)
	{
		hkMemoryRouter::getInstance().heap().blockFree(static_cast<void*>(ptr), numberOfObjects*hkSizeOfTypeOrVoid<TYPE>::val);
	}

	

	template <typename TYPE>
	HK_FORCE_INLINE int HK_CALL hkStackQuerySize()
	{
		hkMemoryRouter& mem = hkMemoryRouter::getInstance();
		return mem.stack().getFreeStackSize();
	}

	template <typename TYPE>
	HK_FORCE_INLINE TYPE* HK_CALL hkAllocateStack(int n, const char* what=HK_NULL )
	{
		hkMemoryRouter& mem = hkMemoryRouter::getInstance();
		return static_cast<TYPE*>( mem.stack().allocateStack( HK_NEXT_MULTIPLE_OF(128,n*hkSizeOfTypeOrVoid<TYPE>::val), what) );
	}

	template <typename TYPE>
	HK_FORCE_INLINE void HK_CALL hkShrinkAllocatedStack(TYPE* ptr, int n)
	{
		hkMemoryRouter& mem = hkMemoryRouter::getInstance();
		mem.stack().shrinkAllocatedStack(ptr, HK_NEXT_MULTIPLE_OF(128,n*hkSizeOfTypeOrVoid<TYPE>::val) );
	}

	template <typename TYPE>
	HK_FORCE_INLINE void HK_CALL hkDeallocateStack(TYPE* ptr)
	{
		hkMemoryRouter::getInstance().stack().deallocateStack( static_cast<void*>(ptr) );
	}
// end deprecated

#define HK_TEMPLATE_ALLOCATOR(POOL, pool) \
	template <typename TYPE> HK_FORCE_INLINE TYPE* HK_CALL hkMem##POOL##BlockAlloc(int n) \
	{ \
		return hkMemoryRouter::getInstance().pool()._blockAlloc<TYPE>( n ); \
	} \
	template <typename TYPE> HK_FORCE_INLINE void HK_CALL hkMem##POOL##BlockFree( TYPE* t, int n) \
	{ \
		hkMemoryRouter::getInstance().pool()._blockFree<TYPE>( t, n ); \
	} \
	template <typename TYPE> HK_FORCE_INLINE TYPE* HK_CALL hkMem##POOL##BufAlloc(int& reqInOut) \
	{ \
		return hkMemoryRouter::getInstance().pool()._bufAlloc<TYPE>( reqInOut ); \
	} \
	template <typename TYPE> HK_FORCE_INLINE void HK_CALL hkMem##POOL##BufFree( TYPE* t, int n) \
	{ \
		hkMemoryRouter::getInstance().pool()._bufFree<TYPE>( t, n ); \
	} \
	template <typename TYPE> HK_FORCE_INLINE TYPE* HK_CALL hkMem##POOL##BufRealloc( TYPE* t, int nold, int& reqInOut) \
	{ \
		return hkMemoryRouter::getInstance().pool()._bufRealloc<TYPE>( t, nold, reqInOut ); \
	}
HK_TEMPLATE_ALLOCATOR(Debug, debug)
HK_TEMPLATE_ALLOCATOR(Solver, solver)
HK_TEMPLATE_ALLOCATOR(Heap, heap)
HK_TEMPLATE_ALLOCATOR(Temp, temp)
HK_TEMPLATE_ALLOCATOR(Global, global)

#if defined(HK_DEBUG) && !defined(HK_PLATFORM_PS3_SPU)
extern void HK_CALL HK_ASSERT_OBJECT_SIZE_OK_FUNC(hk_size_t nbytes);
#	define HK_ASSERT_OBJECT_SIZE_OK(A) HK_ASSERT_OBJECT_SIZE_OK_FUNC(A)
#else
#	define HK_ASSERT_OBJECT_SIZE_OK(A)
#endif

#endif // HKBASE_hkMemoryRouter_H

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
