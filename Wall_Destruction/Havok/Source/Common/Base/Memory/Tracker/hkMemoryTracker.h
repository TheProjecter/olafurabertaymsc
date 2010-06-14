/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_MEMORY_TRACKER_H
#define HKBASE_MEMORY_TRACKER_H

#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>

struct hkSubString;
class hkMemorySystem;

/// Note: There is a weakness in C++ when it comes to using typeid - and therefore the Member definitions for 
/// member variables. The problem is typeid(T) == typeid(T&). Normally thats not too important - but if the
/// code is trying to follow through memory it makes a huge difference - a reference is a pointer, if its not a
/// reference then its actually the object!
///
/// On gcc, there is the typeof operator which returns the type of a variable, and with that if the type is 
/// a reference. On visual studio there seems to be no such way of determining type.
/// Determining a variable type is a reference is easy - but there is no way to get the type of the 
/// variable via templates without using const & - and thus not gaining that information.
///
/// The work around is to use the size of the member. If the type referenced is the same size as a reference - then 
/// there remains no way to determine if it is a reference or not. 

class hkMemoryTracker 
{
    public:
        HK_DECLARE_PLACEMENT_ALLOCATOR();

		struct Member
        {
            hkUint16 m_offset;                  ///
            hkUint16 m_memberSize;              ///
            const hkCppTypeInfo* m_type;
        };
        struct ClassDefinition
        {
            const hkCppTypeInfo* m_type;            ///
            HK_MEMORY_CLASS m_memoryClass;          ///
            const Member* m_members;                ///
            const hkCppTypeInfo* m_parentType;      ///
            
			hkUint16 m_size;						///
			hkUint16 m_numMembers;                  ///
			hkUint8 m_isVirtual;					///
			hkUint8 m_alignment;					///
			hkInt16 m_parentOffset;					///
        };
		struct ScopeInfo
		{
			void* m_ptr;
			const hkCppTypeInfo* m_type;
			hk_size_t m_data;
		};

            /// New hkReferenced derived object
        virtual void onNewReferencedObject(hk_size_t size, void* ptr) = 0;
            /// Delete hkReferenced derived object
        virtual void onDeleteReferencedObject(void* ptr) = 0;

            /// New object
        virtual void onNewObject(const hkCppTypeInfo& type, hk_size_t size, void* ptr) = 0;
            /// Delete hkReferenced derived object
        virtual void onDeleteObject(void* ptr) = 0;

            /// Add a class definition. NOTE the clsDef structure, and its members are assumed to be stay in scope for
            /// the life of the tracker.
        virtual void addClassDefinition(const ClassDefinition& clsDef)= 0;

			/// Get the class definition from the type
		virtual const ClassDefinition* getClassDefinition(const hkCppTypeInfo& type) = 0;
            /// Find a class definition
        virtual const ClassDefinition* findClassDefinition(const hkSubString& name) = 0;

			/// Enter scope
		virtual void enterScope(void* ptr, const hkCppTypeInfo& type, hk_size_t data) = 0;
			/// leave scope
		virtual void leaveScope(void* ptr) = 0;

			/// Find a class definition from a regular c string
		const ClassDefinition* findClassDefinition(const char* name);

            /// Virtual Dtor
        virtual ~hkMemoryTracker() {}

            /// Get the instance
        static hkMemoryTracker& HK_CALL getInstance() { return *s_singleton; }
            /// Set the instance
        static void HK_CALL setInstance(hkMemoryTracker* tracker) { s_singleton = tracker; }


			/// Returns true if the system has the allocation in its memory walk (if doesn't implement memory walk will return true)
		static hkBool HK_CALL hasAllocationInMemoryWalk(hkMemorySystem* system, void* ptr);

    protected:

        static hkMemoryTracker* s_singleton;
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
