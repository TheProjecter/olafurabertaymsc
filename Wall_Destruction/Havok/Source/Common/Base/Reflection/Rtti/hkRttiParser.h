/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_RTTI_PARSER_H
#define HK_RTTI_PARSER_H

#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>
#include <Common/Base/Container/SubString/hkSubString.h>

class hkRttiParser
{
	public:

		enum Type
		{
			TYPE_UNKNOWN,				///
			TYPE_POINTER,				///
			TYPE_REFERENCE,				///

			TYPE_CLASS_TEMPLATE,		///

			// Must be kept in order
			TYPE_INT8,					///
			TYPE_INT16,					///
			TYPE_INT32,					///
			TYPE_INT64,					///

			// Must be kept in order
			TYPE_UINT8,					///
			TYPE_UINT16,				///
			TYPE_UINT32,				///
			TYPE_UINT64,				///

			TYPE_FLOAT32,				///
			TYPE_FLOAT64,				///

			TYPE_BOOL,					///
			TYPE_VOID,					///

			TYPE_NAMED,					///

			TYPE_ENUM,					///
			TYPE_CLASS,					///

			TYPE_ARRAY,					///
		};

		struct Node
		{
			// Note a templated type will hold the linked list of its contained types from the m_contains pointer.
			Type m_type;
			hkSubString m_name;			///
			int m_dimension;			///
			Node* m_contains;				///
			Node* m_next;					///
		};

		struct Allocator
		{
			virtual ~Allocator() {}
			virtual Node* newNode(Type type) = 0;
			virtual void* newBuffer(int len) = 0; 
			virtual void clear() = 0;
		};

			/// Returns true if it's a straight built in type (not pointer, reference, enum, array, class etc)
		HK_FORCE_INLINE static hkBool isBuiltInType(Type type) { return type >= TYPE_INT8 && type <= TYPE_VOID; }

			/// Works out the basic type
		static Node* HK_CALL parseType(const hkCppTypeInfo& type, Allocator& allocator);

			/// Dump out the structure of a type
		static void HK_CALL dumpType(Node* type, hkOstream& stream);
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
