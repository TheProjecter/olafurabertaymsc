/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_CPP_RTTI_INFO_UTIL_H
#define HK_CPP_RTTI_INFO_UTIL_H

// Abstraction to gain access in a platform independent way to C++ Rtti type info
// Use the typeid opertator to get a const hkCppRttiInfo& for a type
//
// For this to work correctly - C++ rtti must be enabled, specifically if typeid is 
// going to be used on dynamic types.

// Visual studio
#	if defined(_CPPRTTI)
#		define HK_HAS_CPP_RTTI
#		include <typeinfo.h>
typedef std::type_info hkCppTypeInfo;
#	endif

// Gcc

// Make a something available -> will keep other interfaces working
#if !defined(HK_HAS_CPP_RTTI)
typedef int hkCppTypeInfo;
#define HK_GET_TYPE_ID(x) (HK_NULL)
#else
#define HK_GET_TYPE_ID(x) (&typeid(x))
#endif

struct hkCppTypeInfoHash
{
	inline static unsigned hash( const hkCppTypeInfoHash& key, unsigned mod )
	{
		return key.m_hash & mod;
	}
	inline static void invalidate( hkCppTypeInfoHash& key ) { key.m_type = HK_NULL; }
	inline static hkBool32 isValid( const hkCppTypeInfoHash& key ) { return key.m_type != HK_NULL; }
	inline static hkBool32 equal( const hkCppTypeInfoHash& key0, const hkCppTypeInfoHash& key1 ) { return key0.m_hash == key1.m_hash && *key0.m_type == *key1.m_type; }

		/// Default ctor
	hkCppTypeInfoHash():
		m_type(HK_NULL),
		m_hash(0)
	{
	}
		/// Ctor
	hkCppTypeInfoHash(const hkCppTypeInfo& info);

	const hkCppTypeInfo* m_type;
	unsigned m_hash;
};

class hkCppRttiInfoUtil
{
	public:
		static const char* HK_CALL getName(const hkCppTypeInfo& info);
		static const char* HK_CALL getRawName(const hkCppTypeInfo& info);
			/// Returns the type of a virtual object. Only works if dynamic rtti information is switched on the build
			/// If cannot determine the type will return HK_NULL
		static const hkCppTypeInfo* HK_CALL getVirtualClassType(void* ptr); 

};

#endif // HK_CPP_RTTI_INFO_UTIL_H

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
