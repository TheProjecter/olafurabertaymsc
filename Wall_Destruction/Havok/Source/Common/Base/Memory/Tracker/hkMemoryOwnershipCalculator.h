/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_MEMORY_OWNERSHIP_CALCULATOR_H
#define HKBASE_MEMORY_OWNERSHIP_CALCULATOR_H

#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>

#include <Common/Base/Memory/Tracker/hkDefaultMemoryTracker.h>

#include <Common/Base/Memory/System/hkMemorySystem.h>


class hkMemoryOwnershipCalculator
{
    public:
        HK_DECLARE_PLACEMENT_ALLOCATOR();

		typedef hkMemoryTracker::ClassDefinition ClassDefinition;
		typedef hkMemoryTracker::Member Member;

		struct Allocation;

		struct ClassLayout
		{
			const ClassDefinition* m_clsDef;		///
			hk_size_t m_size;						///
			Member* m_members;						///
			int m_numMembers;						///
			hkUint8* m_used;						///
			int m_numPointers;						///
		};

			/// Class instance
		struct ClassInstance
		{
			const ClassDefinition* m_clsDef;		///
			const hkCppTypeInfo* m_type;			///
			const ClassLayout* m_layout;			///
			Allocation* m_allocation;				///
			hk_size_t m_size;						///
			hkBool m_isVirtual;						///
			int m_instanceIndex;					///
		};

			/// An allocation
		struct Allocation
		{
			void* m_start;							///
			hk_size_t m_size;						///
			hk_size_t m_usedSize;					///
			hk_size_t m_shared;						///
			int m_ownerIndex;						///
			ClassInstance* m_classInstance;			///
			Allocation* m_parentAllocation;			///
		};


			/// Handle a templated type
		typedef void (HK_CALL *TemplateHandler)(int ownershipIndex, hkMemoryOwnershipCalculator* calc, const hkRttiParser::Node* node, void* data, hk_size_t size);
		typedef hkBool (HK_CALL *FilterFunction)(const Allocation& alloc);

			/// Set ownership on things referenced by this chunk of memory
		void setContentsOwnership(int ownershipIndex, const ClassLayout& layout, void* data);
			/// Scans and sets ownership
		void setScanOwnership(int ownershipIndex, void* data, hk_size_t size, const hkUint8* ignore = HK_NULL);
			/// Set the arrays contents to ownership (if not already set)
		void setArrayContentsOwnership(int ownershipIndex, const hkRttiParser::Node* node, int numMembers, hk_size_t memberSize, void* data);
			/// Set the ownership for the type
		void setContentsOwnership(int ownershipIndex, const hkCppTypeInfo& type, void* data, hk_size_t size);

			/// Returns the number of allocations of the type there are
		int calcNumAllocations(const hkCppTypeInfo& type) const;
			/// Get tne num allocations from the fully qualified name of classes/structs 
		int calcNumAllocations(const char* name);

			/// Find allocs with the specified name
		int findAllocs(const char* name, void** ptrs, int numPtrs);

			/// Given a type - returns a type tree of that type. Cached - so can compare the node pointers to see if 
			/// two types are the same. Only doesn't work if not parsable (returns HK_NULL in that case)
		const hkRttiParser::Node* getTypeTree(const hkCppTypeInfo& type);
		
			/// Start working out memory ownership
		hkResult init(hkMemorySystem* memorySystem, hkDefaultMemoryTracker* tracker);

			/// Sets all of the owner indices
		void setAllOwnerIndices(int ownerIndex = -1);

			/// Works out the layout of a class from the definition
		const ClassLayout* getClassLayout(const hkSubString& name) const;

			/// Work out what size the type is
		hk_size_t calcTypeSize(const hkRttiParser::Node* node) const;


			/// Get an allocation form its starting pointer
		Allocation* findAllocation(const void* data) { return m_allocMap.getWithDefault(const_cast<void*>(data), HK_NULL); }

			/// Find the allocation that contains the address
		Allocation* findAllocationContaining(const void* data);

			/// Get all of the allocations
		const hkArrayBase<Allocation*>& getAllocations() const { return m_allocArray; }
			/// Get all of the instances of classes
		const hkArrayBase<ClassInstance>& getClassInstances() const { return m_instanceArray; }

			/// Add a handler
		void addTemplateHandler(const char* name, TemplateHandler handler);

			/// Get a handlers or return HK_NULL if one is not found
		TemplateHandler getTemplateHandler(const hkSubString& name) const;

			/// Returns true if it is the type
		hkBool32 isType(const hkCppTypeInfo& info, const char* name);

			/// If the filter returns false, the memory isn't followed
		void setFollowFilter(FilterFunction func);
			/// Get the follow filter
		FilterFunction getFollowFilter() const;

			//// Handle the array template
		static void HK_CALL handleArrayTemplate(int ownershipIndex, hkMemoryOwnershipCalculator* calc, const hkRttiParser::Node* node, void* data, hk_size_t size);
			/// Handle the ref ptr
		static void HK_CALL handleRefPtrTemplate(int ownershipIndex, hkMemoryOwnershipCalculator* calc, const hkRttiParser::Node* node, void* data, hk_size_t size);

			/// Ctor
		hkMemoryOwnershipCalculator();

			/// Dtor
		virtual ~hkMemoryOwnershipCalculator() {}

			/// If set, if a memory chunk is reached of an unknown type it will be scanned for pointers.
		hkBool m_allowScan;

    protected:
		struct TemplateEntry
		{
			TemplateHandler m_handler;
			const char* m_name;
		};

		static void HK_CALL _findAllocsCallback(void* start, hk_size_t size, hkBool allocated, int pool, void* param);


        HK_FORCE_INLINE static hkBool32 _compareClassInstances(const ClassInstance* a, const ClassInstance* b)
        {
			if (*a->m_type == *b->m_type)
			{
				return false;
			}

			const char* aName = hkCppRttiInfoUtil::getRawName(*a->m_type);
			const char* bName = hkCppRttiInfoUtil::getRawName(*b->m_type);
				
			return hkString::strCmp(aName, bName) < 0;
        }

		static hkBool HK_CALL _alwaysFollowFilter(const Allocation& alloc)
		{
			return true;
		}

		HK_FORCE_INLINE static hkBool32 _compareMembers(const Member& a, const Member& b)
        {
            return a.m_offset < b.m_offset;
        }
		
		HK_FORCE_INLINE static hkBool32 _compareAllocations(const Allocation* a, const Allocation* b)
        {
            return a->m_start < b->m_start;
        }
		void _calcLayout(ClassLayout& layout, const ClassDefinition& clsDef);
		ClassLayout* _createLayout(const ClassDefinition* clsDef, ClassInstance** instances, int numInstances);

		hkBool _checkAllocations();
		void _addLayout(const hkCppTypeInfo& type, ClassLayout* layout);

		Allocation* _getAllocation(void* ptr);
			/// Sets up the alloc map - including ordering the allocations
		void _initAllocMap();
		
		static hkBool32 HK_CALL _isEasyAlloc(const Allocation& alloc, void* ptr);

		static int HK_CALL _calcNumMembers(const ClassDefinition* clsDef, hkMemoryTracker* tracker);
		void _setPointerOwnership(int ownershipIndex, const hkRttiParser::Node* node, void* data);
		void _setTemplateContentsOwnership(int ownershipIndex, const hkRttiParser::Node* node, void* data, hk_size_t memberSize);
		void _setContentsOwnership(int ownershipIndex, const hkRttiParser::Node* node, void* data, hk_size_t size);

			// Determines from the type if this is something that can be scanned for pointers
		hkBool _canScan(const hkRttiParser::Node* node);
			// Determines from the type if this is something that is known to contain pointers, or it understood from a traversal perspective
		hkBool _isTraversable(const hkRttiParser::Node* node);

		hkBool _isPossiblePointer(int pointerIndex, ClassInstance** instances, int numInstances);
		void _validatePointers(ClassLayout& layout, ClassInstance** instances, int numInstances);

			/// Create and add a class instance associated with the allocation
		ClassInstance* _addClassInstance(const hkDefaultMemoryTracker::ClassAlloc& classAlloc, Allocation& alloc, const hkCppTypeInfo* type);

		void _dumpStackTrack(void* ptr, hkOstream& stream);

        typedef hkArray<Allocation*, hkDefaultMemoryTrackerAllocator> AllocArrayType;
		typedef hkArray<ClassInstance, hkDefaultMemoryTrackerAllocator> ClassInstanceArrayType;
		typedef hkMap<void*, Allocation*, hkMapOperations<void*>, hkDefaultMemoryTrackerAllocator> AllocMapType;
		
		typedef hkArray<Member, hkDefaultMemoryTrackerAllocator> MemberArrayType;
		typedef hkArray<hkUint8, hkDefaultMemoryTrackerAllocator> UsedArrayType;

		typedef hkMap<hkSubString, ClassLayout*, hkSubString, hkDefaultMemoryTrackerAllocator> LayoutMapType;

		typedef hkMap<hkCppTypeInfoHash, const hkRttiParser::Node*, hkCppTypeInfoHash, hkDefaultMemoryTrackerAllocator> TypeTreeMapType;

		typedef hkArray<TemplateEntry, hkDefaultMemoryTrackerAllocator> TemplateArrayType;
		
		hkMemoryTracker* m_tracker;
		hkAllocatorFreeListMemoryServer m_freeListMemoryServer;

		MemberArrayType m_memberArray;
		UsedArrayType m_usedArray;

		hkFreeList m_layoutFreeList;
		hkFreeList m_allocFreeList;

		AllocArrayType m_allocArray;
		AllocMapType m_allocMap;
		ClassInstanceArrayType m_instanceArray;
		LayoutMapType m_layoutMap;						
		TypeTreeMapType m_treeTypeMap;

		// Holds all of the template handlers
		TemplateArrayType m_templateHandlers;

		// The parser allocator
		hkRttiParserAllocator m_parserAllocator;

		FilterFunction m_followFilter;
};

#endif // HKBASE_MEMORY_OWNERSHIP_CALCULATOR_H

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
