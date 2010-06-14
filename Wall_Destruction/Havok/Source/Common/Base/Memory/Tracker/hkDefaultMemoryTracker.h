/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_DEFAULT_MEMORY_TRACKER_H
#define HKBASE_DEFAULT_MEMORY_TRACKER_H

#include <Common/Base/Container/PointerMap/hkMap.h>
#include <Common/Base/Container/PointerMap/hkMap.cxx>

#include <Common/Base/Container/PointerMultiMap/hkMultiMap.h>
#include <Common/Base/Container/PointerMultiMap/hkMultiMap.cxx>

#include <Common/Base/Memory/Allocator/FreeList/hkFreeList.h>

#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

#include <Common/Base/Memory/Tracker/hkMemoryTracker.h>

#include <Common/Base/Reflection/Rtti/hkRttiParserAllocator.h>

#include <Common/Base/Reflection/Registry/hkClassNameRegistry.h>

class hkOstream;

struct hkDefaultMemoryTrackerAllocator
{
	hkMemoryAllocator& get(void*) { return *s_allocator; }
	static hkMemoryAllocator* s_allocator;
};

class hkDefaultMemoryTracker : public hkMemoryTracker
{
	public:
		HK_DECLARE_PLACEMENT_ALLOCATOR();

		// A class allocation
		struct ClassAlloc
		{
			enum 
			{
				FLAG_REFERENCED_OBJECT = 0x1,
			};
			const hkCppTypeInfo* m_type;			///
			hk_size_t m_size;						///
			void* m_ptr;							///
			int m_flags;							///
		};
		typedef hkMap<void*, ClassAlloc*, hkMapOperations<void*>, hkDefaultMemoryTrackerAllocator> ClassAllocMapType;

		// hkMemoryTracker implementation
        virtual void onNewReferencedObject(hk_size_t size, void* ptr);
        virtual void onDeleteReferencedObject(void* ptr);
        virtual void onNewObject(const hkCppTypeInfo& type, hk_size_t size, void* ptr);
        virtual void onDeleteObject(void* ptr);
		virtual void enterScope(void* ptr, const hkCppTypeInfo& type, hk_size_t data);
		virtual void leaveScope(void* ptr);

        virtual void addClassDefinition(const ClassDefinition& clsDef);
		virtual const ClassDefinition* getClassDefinition(const hkCppTypeInfo& type);
        virtual const ClassDefinition* findClassDefinition(const hkSubString& name);

			/// Returns map of all of the class allocations
		const ClassAllocMapType& getClassAllocations() const { return m_classAllocMap; }

			/// Make displayable text from memory size
		static void HK_CALL memorySizeToText(hk_size_t size, hkStringBuf& string);

			/// Ctor
		hkDefaultMemoryTracker(hkMemoryAllocator* allocator);
			/// Dtor
		~hkDefaultMemoryTracker();

	protected:
		struct ClassEntry
		{
			const ClassDefinition* m_definition;
			hkSubString m_name;
			// The actual storage of the name is after the class definition
		};

		ClassAlloc* _addClassAlloc(const hkCppTypeInfo* type, hk_size_t size, void* ptr, int flags);
		void _removeClassAlloc(void* ptr);

        typedef hkMultiMap<hkUlong, const ClassEntry*, hkMapOperations<hkUlong>, hkDefaultMemoryTrackerAllocator> ClassHashMultiMap;
		typedef hkMultiMap<hkUlong, const ClassEntry*, hkMapOperations<hkUlong>, hkDefaultMemoryTrackerAllocator> NameHashMultiMap;
		typedef hkMap<hkUlong, ScopeInfo*, hkMapOperations<hkUlong>, hkDefaultMemoryTrackerAllocator> ScopeMap;

		hkAllocatorFreeListMemoryServer m_freeListMemoryServer;
		hkFreeList m_classAllocFreeList;
		hkFreeList m_scopeFreeList;

		ClassAllocMapType m_classAllocMap;

		ClassHashMultiMap m_classHashMultiMap;
		NameHashMultiMap m_nameHashMultiMap;
		ScopeMap m_scopeMap;

		hkRttiParserAllocator m_parserAllocator;

		hkMemoryAllocator* m_allocator;
		mutable hkCriticalSection m_criticalSection;
};

#endif // HKBASE_MEMORY_TRACKER_H

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
