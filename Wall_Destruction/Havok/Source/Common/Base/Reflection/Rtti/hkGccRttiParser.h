/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_STANDARD_RTTI_PARSER_H
#define HK_STANDARD_RTTI_PARSER_H

#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>
#include <Common/Base/Container/SubString/hkSubString.h>

#include <Common/Base/Reflection/Rtti/hkRttiParser.h>

class hkGccRttiParser
{
	public:
		typedef hkRttiParser::Type Type;
		typedef hkRttiParser::Node Node;
		typedef hkRttiParser::Allocator Allocator;

			/// Parses a gcc mangled type (as returned from typeid().name)
			/// Does not support function types
			/// Does not support non types as template parameters.
		static Node* HK_CALL parseType(const hkSubString& subString, Allocator& allocator);

	protected:
		HK_FORCE_INLINE static hkBool isDigit(char c) { return c >= '0' && c <= '9'; }

		static Type HK_CALL _parseBuiltInType(char c);
		static Type HK_CALL _parseBuiltInType(hkSubString& subString);
		static Node* HK_CALL _parseType(hkSubString& subString, Allocator& allocator);
		static hkBool HK_CALL _parseScopedName(hkSubString& subString, hkSubString& nameOut, Allocator& allocator);
};

#endif // HK_STANDARD_RTTI_PARSER_H

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
