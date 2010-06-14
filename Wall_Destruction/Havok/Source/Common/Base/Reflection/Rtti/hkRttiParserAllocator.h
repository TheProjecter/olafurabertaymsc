/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_RTTI_PARSER_ALLOCATOR_H
#define HK_RTTI_PARSER_ALLOCATOR_H

#include <Common/Base/Reflection/Rtti/hkRttiParser.h>
#include <Common/Base/Memory/Allocator/FreeList/hkFreeList.h>

/// Implements teh hkRttiParser::Allocator interface - providing memory allocation
/// for the parser.
///
/// One properties of the allocator
/// o It tracks all the allocations - so all allocations can be freed with clear (and thus
///   a Node hierarchy deallocated)
/// o Has no freeing methods - this enables some very simple / fast implementations (for
//    example just having an internal array of memory for nodes, and buffer

class hkRttiParserAllocator: public hkRttiParser::Allocator
{
	public:
		HK_DECLARE_PLACEMENT_ALLOCATOR();

		virtual hkRttiParser::Node* newNode(hkRttiParser::Type type);
		virtual void* newBuffer(int size);
		virtual void clear();

			/// Ctor
		hkRttiParserAllocator(hkMemoryAllocator* allocator);

			/// Dtor
		~hkRttiParserAllocator();

	protected:
		struct Entry
		{
			int m_size;
			Entry* m_next;
		};

        hkAllocatorFreeListMemoryServer m_freeListMemoryServer;

		hkFreeList m_nodeFreeList;
		hkMemoryAllocator* m_allocator;

		Entry* m_entries;
};

#endif // HK_RTTI_PARSER_H

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
