/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_TRACKER_CLASS_DEFINITION_H
#define HKBASE_TRACKER_CLASS_DEFINITION_H

#include <Common/Base/Memory/Tracker/hkMemoryTracker.h>

class hkTrackerInit
{
	public:
		typedef hkMemoryTracker::ClassDefinition ClassDefinition;
			/// Ctor
		hkTrackerInit(const ClassDefinition& clsDef);

		static void HK_CALL initClasses();

	protected:
		const ClassDefinition& m_clsDef;
		hkTrackerInit* m_next;

		static hkTrackerInit* s_initLinkedList;
};

#if defined(HK_MEMORY_TRACKER_ENABLE)

// Everything else is assumed to be a struct..
#define HK_TRACKER_MEMBERS_BEGIN(TYPE) \
/* static */ const hkMemoryTracker::Member TYPE::s_trackerMembers[] =  \
{

#define HK_TRACKER_MEMBER(TYPE, FIELD) \
	{hkUint16(HK_OFFSET_OF(TYPE, FIELD)), hkUint16(sizeof( ((TYPE*)0)->FIELD) ), &typeid(((TYPE*)0)->FIELD) },

#define HK_TRACKER_MEMBERS_END() \
	{0, 0, HK_NULL} \
};

#define HK_TRACKER_CLASS_DEFINITION_BASE(UNIQUE_NAME, TYPE, MEM_CLS) \
/* static */const hkMemoryTracker::ClassDefinition TYPE::s_trackerClsDef = \
{ \
	&typeid(TYPE),					\
	MEM_CLS,						\
	TYPE::s_trackerMembers,				\
	HK_NULL,						\
	hkUint16(sizeof(TYPE)),			\
	hkUint16(sizeof(TYPE::s_trackerMembers) / sizeof(TYPE::s_trackerMembers[0])) - 1,		\
	sizeof(hkIsVirtual(static_cast<TYPE*>(0))) == sizeof(hkBool::CompileTimeTrueType),	\
	hkUint8(HK_ALIGN_OF(TYPE)),		\
	0								\
}; \
\
/* static */const hkTrackerInit TYPE::s_trackerInit(TYPE::s_trackerClsDef);

#define HK_TRACKER_CLASS_DEFINITION(UNIQUE_NAME, TYPE, MEM_CLS, PARENT) \
/* static */ const hkMemoryTracker::ClassDefinition TYPE::s_trackerClsDef = \
{ \
	&typeid(TYPE),					\
	MEM_CLS,						\
	TYPE::s_trackerMembers,				\
	&typeid(PARENT),				\
	hkUint16(sizeof(TYPE)),			\
	hkUint16(sizeof(TYPE::s_trackerMembers) / sizeof(TYPE::s_trackerMembers[0])) - 1,		\
	sizeof(hkIsVirtual(static_cast<TYPE*>(0))) == sizeof(hkBool::CompileTimeTrueType),	\
	hkUint8(HK_ALIGN_OF(TYPE)),		\
	hkInt16(((hkUint8*)static_cast<PARENT*>((TYPE*)16)) - (hkUint8*)16)					\
}; \
\
/* static */const hkTrackerInit TYPE::s_trackerInit(TYPE::s_trackerClsDef);


#define HK_TRACKER_ABSTRACT_CLASS_DEFINITION_BASE(UNIQUE_NAME, TYPE, MEM_CLS) \
/* static */const hkMemoryTracker::ClassDefinition TYPE::s_trackerClsDef = \
{ \
	&typeid(TYPE),					\
	MEM_CLS,						\
	TYPE::s_trackerMembers,			\
	HK_NULL,						\
	hkUint16(sizeof(TYPE)),			\
	hkUint16(sizeof(TYPE::s_trackerMembers) / sizeof(TYPE::s_trackerMembers[0])) - 1,		\
	sizeof(hkIsVirtual(static_cast<TYPE*>(0))) == sizeof(hkBool::CompileTimeTrueType),	\
	hkUint8(0),		\
	0								\
}; \
\
/* static */const hkTrackerInit TYPE::s_trackerInit(TYPE::s_trackerClsDef);

#define HK_TRACKER_ABSTRACT_CLASS_DEFINITION(UNIQUE_NAME, TYPE, MEM_CLS, PARENT) \
/* static */ const hkMemoryTracker::ClassDefinition TYPE::s_trackerClsDef = \
{ \
	&typeid(TYPE),					\
	MEM_CLS,						\
	TYPE::s_trackerMembers,			\
	&typeid(PARENT),				\
	hkUint16(sizeof(TYPE)),			\
	hkUint16(sizeof(TYPE::s_trackerMembers) / sizeof(TYPE::s_trackerMembers[0])) - 1,		\
	sizeof(hkIsVirtual(static_cast<TYPE*>(0))) == sizeof(hkBool::CompileTimeTrueType),	\
	hkUint8(0),		\
	hkInt16(((hkUint8*)static_cast<PARENT*>((TYPE*)16)) - (hkUint8*)16)					\
}; \
\
/* static */const hkTrackerInit TYPE::s_trackerInit(TYPE::s_trackerClsDef);

#else

#define HK_TRACKER_MEMBERS_BEGIN(TYPE)
#define HK_TRACKER_MEMBER(TYPE, FIELD)
#define HK_TRACKER_MEMBERS_END()

#define HK_TRACKER_CLASS_DEFINITION(UNIQUE_NAME, TYPE, MEM_CLS, PARENT)
#define HK_TRACKER_CLASS_DEFINITION_BASE(UNIQUE_NAME, TYPE, MEM_CLS)
#define HK_TRACKER_ABSTRACT_CLASS_DEFINITION(UNIQUE_NAME, TYPE, MEM_CLS, PARENT)
#define HK_TRACKER_ABSTRACT_CLASS_DEFINITION_BASE(UNIQUE_NAME, TYPE, MEM_CLS)


#endif

#endif // HKBASE_TRACKER_CLASS_DEFINITION_H

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
