/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_SERIALIZE_VERSIONUTIL_H
#define HK_SERIALIZE_VERSIONUTIL_H

class hkObjectUpdateTracker;
class hkClassNameRegistry;
class hkPackfileReader;

/// Versioning utility functions and structures.
namespace hkVersionUtil
{
		/// Get the current sdk version as written in packfiles.
	const char* HK_CALL getCurrentVersion();

		/// Copies the old named member to the new named member.
		/// The members must have identical sizes.
	void HK_CALL renameMember( hkVariant& oldObj, const char* oldName, hkVariant& newObj, const char* newName );

		/// Copy defaults for new members.
		/// For each member in newClass which is not in oldClass and which
		/// has a specified default, copy it in into obj.
	void HK_CALL copyDefaults( void* obj, const hkClass& oldClass, const hkClass& newClass );

		/// Copy object from 'objectsIn' to preallocated 'objectsOut'.
		/// The 'tracker' stores information about all shared and new allocations
		/// for new objects.
		/// Return pointer to the block of memory with new objects, if successful. Otherwise, HK_NULL.
		/// NOTE: A corresponding hkClass must be specified for each 'objectsOut' element before calling this function.
		/// The index of the new object in 'objectsOut' will match the index of object copied from 'objectsIn'.
		/// The hkClass pointers in the copied object's members of the hkVariant and homogeneous array types are not updated.
	void* HK_CALL copyObjects( const hkArrayBase<hkVariant>& objectsIn, hkArray<hkVariant>& objectsOut, hkObjectUpdateTracker& tracker );

		/// Utility function to recompute member offsets.
	void HK_CALL recomputeClassMemberOffsets( hkClass*const* classes, int classVersion );
}

#endif // HK_SERIALIZE_VERSIONUTIL_H

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
