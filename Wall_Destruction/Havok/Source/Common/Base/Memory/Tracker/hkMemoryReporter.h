/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_MEMORY_REPORTER_H
#define HKBASE_MEMORY_REPORTER_H

#include <Common/Base/Reflection/Rtti/hkCppRttiInfoUtil.h>
#include <Common/Base/Memory/Tracker/hkMemoryOwnershipCalculator.h>



class hkMemoryReporter
{
    public:
        HK_DECLARE_PLACEMENT_ALLOCATOR();
		typedef hkMemoryOwnershipCalculator::Allocation Allocation;
		typedef hkMemoryOwnershipCalculator::FilterFunction FilterFunction;

		class VirtualType
		{
			virtual ~VirtualType() {}
		};

		struct MemorySize
		{
			 MemorySize(hk_size_t size):m_size(size) {}
			 hk_size_t m_size;
		};

		struct Info
		{
			Info():
				m_followFilter(HK_NULL),
				m_startFilter(HK_NULL),
				m_dumpRemaining(true)
			{
			}

			FilterFunction m_followFilter;			///
			FilterFunction m_startFilter;			///
			
			hkBool m_dumpRemaining;
		};

	
			/// Report memory usage by time
		static void HK_CALL reportTypeUsage(hkMemorySystem* system, hkMemoryOwnershipCalculator& calc, const Info& info, hkOstream& stream);

			/// For printing memory sizes
		friend hkOstream& HK_CALL operator<<(hkOstream& stream, const hkMemoryReporter::MemorySize& size);

			/// Convert size into text
		static void HK_CALL memorySizeToText(hk_size_t size, hkStringBuf& string);


    protected:

		struct TotalInfo
		{
			hk_size_t m_totalUsed;
			hk_size_t m_totalAlloced;
			hk_size_t m_totalShared;
		};

		static void HK_CALL _assignIndexOwnership(hkMemoryOwnershipCalculator& calc, hkArrayBase<const hkMemoryOwnershipCalculator::ClassInstance*>& instances);
		static void HK_CALL _assignChildOwnership(hkMemoryOwnershipCalculator& calc, FilterFunction startFilter);
		static void HK_CALL _assignOwnership(hkMemoryOwnershipCalculator& calc, FilterFunction startFilter, hkBool isVirtual);

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
