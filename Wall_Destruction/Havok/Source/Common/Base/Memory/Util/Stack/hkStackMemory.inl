/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#if !defined(HK_PLATFORM_PS3_SPU)
hkStackMemory::hkStackMemory()
{
	m_next = HK_NULL;
	m_start = HK_NULL;
	m_end = 0;
#if defined (HK_PLATFORM_SIM)
	m_numAllocInfos = 0;
#endif
}

HK_FORCE_INLINE hkStackMemory::~hkStackMemory()
{
#if defined (HK_PLATFORM_SIM)
	HK_ASSERT2(0x5f82931b, !m_numAllocInfos, "Not all allocations got freed");
#endif
}
#endif

void hkStackMemory::initMemory(void* p, int size)
{
	m_start = p;
	m_next = p;
	m_end = hkAddByteOffset(p, size);

#if defined (HK_PLATFORM_SIM)
	hkMemUtil::memSet(p, 0xcd, size);
	m_maxStackSize = size;
#endif
}

#if !defined (HK_PLATFORM_SIM)

// See .cpp for out of line implementaion for HK_PLATFORM_SIM
HK_FORCE_INLINE void* hkStackMemory::allocateStack(int numBytes, const char* what)
{
	HK_ASSERT2(0xaf8365de, !(numBytes & 0x7f) , "Allocation-size should be a multiple of 128.");
	if ( numBytes > getFreeStackSize() )
	{
		HK_ERROR(0xaf8365df, "Out of stack memory.");
	}

	void* current = m_next;
	m_next = hkAddByteOffset(current, numBytes);
	return current;
}

#endif

void* hkStackMemory::allocateStackRoundSizeTo128WithCriticalAssert(int numBytes, const char* what)
{
	HK_CRITICAL_ASSERT2(0xaf8365df, getFreeStackSize() >= numBytes, "Out of stack memory.");

	int allocationSize = HK_NEXT_MULTIPLE_OF(128, numBytes);
	return allocateStack( allocationSize, what );
}

void* hkStackMemory::allocateStackRoundSizeTo128(int numBytes, const char* what)
{
	int allocationSize = HK_NEXT_MULTIPLE_OF(128, numBytes);
	return allocateStack( allocationSize, what );
}




void hkStackMemory::deallocateStack(void* p)
{
#if defined (HK_PLATFORM_SIM)
	{
		AllocInfo& allocInfo = m_allocInfos[--m_numAllocInfos];
		HK_ASSERT2(0xaf83d35f, allocInfo.m_p == p, "Deallocating invalid memory pointer." );
		hkMemUtil::memSet(allocInfo.m_p, 0xcd, allocInfo.m_size);
		hkMemUtil::memSet(&allocInfo, 0x0, sizeof(AllocInfo));
	}
#endif
	HK_ASSERT2( 0xf0345456, p <= m_next && p >= m_start, "Inconsistent use of stack memory" );

	m_next = p;
}

void hkStackMemory::shrinkAllocatedStack(void* p, int newSize )
{
#if defined (HK_PLATFORM_SIM)
	{
		AllocInfo& allocInfo = m_allocInfos[m_numAllocInfos-1];
		HK_ASSERT2(0xaf83d35f, allocInfo.m_p == p, "Deallocating invalid memory pointer." );
		allocInfo.m_size = newSize;
	}
#endif
	m_next = hkAddByteOffset(p, newSize);
}



void hkStackMemory::deallocateStack(int numBytes)
{
#if defined (HK_PLATFORM_SIM)
	{
		AllocInfo& allocInfo = m_allocInfos[--m_numAllocInfos];
		HK_ASSERT2(0xaf83d35f, allocInfo.m_size == numBytes, "Deallocating invalid memory size." );
		hkMemUtil::memSet(allocInfo.m_p, 0xcd, allocInfo.m_size);
		hkMemUtil::memSet(&allocInfo, 0x0, sizeof(AllocInfo));
	}
#endif

	m_next = hkAddByteOffset(m_next.val(), -numBytes);
	HK_ASSERT2( 0x34a0e762, m_next >= m_start, "Stack underflow!" );
}




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
