/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

void HK_CALL hkString::memCpy4( void* dst, const void* src, int numWords)
{
	const hkUint32* src32 = reinterpret_cast<const hkUint32*>(src);
	hkUint32* dst32       = reinterpret_cast<      hkUint32*>(dst);
	{
		for (int i = 0; i < numWords; i++)
		{
			*(dst32++) = *(src32++);
		}
	}
}

void HK_CALL hkString::memCpy16( void* dst, const void* src, int numQuads)
{
#if defined (HK_PLATFORM_PS3_PPU) || defined (HK_PLATFORM_PS3_SPU)
	const vector signed int* srcQuad = reinterpret_cast<const vector signed int*>(src);
	vector signed int* dstQuad = reinterpret_cast<vector signed int*>(dst);
	{
		for (int i = numQuads-1; i>=0; i--)
		{
			*(dstQuad++) = *(srcQuad++);
		}
	}
#else
	HK_ASSERT2( 0xf021d445, (hkUlong(dst) & 0xf) == 0, "Unaligned address" );
	HK_ASSERT2( 0xf021d446, (hkUlong(src) & 0xf) == 0, "Unaligned address" );
	const hkUint32* src32 = reinterpret_cast<const hkUint32*>(src);
	hkUint32* dst32 = reinterpret_cast<      hkUint32*>(dst);
	{
		for (int i = 0; i < numQuads; i++)
		{
			dst32[0] = src32[0];
			dst32[1] = src32[1];
			dst32[2] = src32[2];
			dst32[3] = src32[3];
			dst32+= 4;
			src32+= 4;
		}
	}
#endif
}

void HK_CALL hkString::memCpy16NonEmpty( void* dst, const void* src, int numQuads)
{
#if defined (HK_PLATFORM_PS3_PPU) || defined (HK_PLATFORM_PS3_SPU)
	const vector signed int* srcQuad = reinterpret_cast<const vector signed int*>(src);
	vector signed int* dstQuad = reinterpret_cast<vector signed int*>(dst);
	{
		do
		{
			*(dstQuad++) = *(srcQuad++);
		}
		while ( --numQuads > 0 );
	}
#else
	HK_ASSERT2( 0xf022d444, numQuads > 0, "Size 0 not allowed" );
	HK_ASSERT2( 0xf022d445, (hkUlong(dst) & 0xf) == 0, "Unaligned address" );
	HK_ASSERT2( 0xf022d446, (hkUlong(src) & 0xf) == 0, "Unaligned address" );
	const hkUint32* src32 = reinterpret_cast<const hkUint32*>(src);
	hkUint32* dst32 = reinterpret_cast<      hkUint32*>(dst);
	{
		do
		{
			dst32[0] = src32[0];
			dst32[1] = src32[1];
			dst32[2] = src32[2];
			dst32[3] = src32[3];
			dst32+= 4;
			src32+= 4;
		}
		while ( --numQuads > 0 );
	}
#endif
}

template<int size>
void hkString::memCpy16(void* dst, const void* src)
{
	HK_ASSERT( 0xf0dedf34, ((size & 0xf) == 0) && (size <= 128) && (size > 0));

#if defined(HK_PLATFORM_PS3_SPU) || defined(HK_PLATFORM_PS3_PPU)
	const vector signed int* srcQuad = reinterpret_cast<const vector signed int*>(src);
	vector signed int*       dstQuad = reinterpret_cast<vector signed int*>(dst);
	vector signed int a,b,c,d;
	if ( size >  0) a = srcQuad[0];
	if ( size > 16) b = srcQuad[1];
	if ( size > 32) c = srcQuad[2];
	if ( size > 48) d = srcQuad[3];
	if ( size >  0) dstQuad[0] = a;
	if ( size > 64) a = srcQuad[4];
	if ( size > 16) dstQuad[1] = b;
	if ( size > 80) b = srcQuad[5];
	if ( size > 32) dstQuad[2] = c;
	if ( size > 96) c = srcQuad[6];
	if ( size > 48) dstQuad[3] = d;
	if ( size > 112) d = srcQuad[7];
	if ( size > 64) dstQuad[4] = a;
	if ( size > 80) dstQuad[5] = b;
	if ( size > 96) dstQuad[6] = c;
	if ( size > 112) dstQuad[7] = d;
#else
	hkString::memCpy16NonEmpty(dst, src, size/16);
#endif
}

void hkString::memCpy256(void* dst, const void* src)
{
#if defined(HK_PLATFORM_PS3_SPU) || defined(HK_PLATFORM_PS3_PPU)
	const vector signed int* HK_RESTRICT srcQuad = (const vector signed int*)src;
	vector signed int*       HK_RESTRICT dstQuad = (vector signed int*)      dst;

	vector signed int a = srcQuad[0];
	vector signed int b = srcQuad[1];
	vector signed int c = srcQuad[2];
	vector signed int d = srcQuad[3];
	dstQuad[0]  = a; a = srcQuad[4];
	dstQuad[1]  = b; b = srcQuad[5];
	dstQuad[2]  = c; c = srcQuad[6];
	dstQuad[3]  = d; d = srcQuad[7];
	dstQuad[4]  = a; a = srcQuad[8];
	dstQuad[5]  = b; b = srcQuad[9];
	dstQuad[6]  = c; c = srcQuad[10];
	dstQuad[7]  = d; d = srcQuad[11];
	dstQuad[8]  = a; a = srcQuad[12];
	dstQuad[9]  = b; b = srcQuad[13];
	dstQuad[10] = c; c = srcQuad[14];
	dstQuad[11] = d; d = srcQuad[15];
	dstQuad[12] = a;
	dstQuad[13] = b;
	dstQuad[14] = c;
	dstQuad[15] = d;
#else
	hkString::memCpy16NonEmpty(dst, src, 16);
#endif
}

void hkString::memSet4(void* dst, const int value, int numWords)
{
	hkUint32* dst32 = reinterpret_cast<      hkUint32*>(dst);
	for (int i = numWords-1; i>=0; i--)
	{
		*dst32 = value;
		dst32++;
	}
}

void HK_CALL hkString::memClear16(void* dst, int numQuads)
{
#if defined (HK_PLATFORM_PS3_PPU) || defined (HK_PLATFORM_PS3_SPU)
	vector signed int zero = (vector signed int){0,0,0,0};
	vector signed int* dest = (vector signed int*)dst;

	for (int i = numQuads-1; i>=0; i--)
	{
		*(dest++) = zero;
	}
#else
	HK_ASSERT2( 0xf021d445, (hkUlong(dst)   & 0xf) == 0, "Unaligned address" );
	hkUint32* dst32 = reinterpret_cast<      hkUint32*>(dst);
	{
		for (int i = 0; i < numQuads; i++)
		{
			dst32[0] = 0;
			dst32[1] = 0;
			dst32[2] = 0;
			dst32[3] = 0;
			dst32+= 4;
		}
	}
#endif
}

// For size up to 512 bytes, on PLAYSTATION(R)3, this will compile down to a sequence of store instructions
// For larger copies or other platforms, it reverts to the looped version.
template<int size>
void hkString::memSet16(void* dst, const void* src)
{
	HK_ASSERT( 0xf0dedf34, ((size & 0xf) == 0) && (size > 0) );

#if defined(HK_PLATFORM_SPU) || defined(HK_PLATFORM_PS3_PPU)

	if (size < 32 * 16)
	{
		const hkQuadReal srcQuad = *reinterpret_cast<const hkQuadReal*>(src);
		hkQuadReal*      dstQuad =  reinterpret_cast<hkQuadReal*>(dst);

#define HK_SET_ELEM( X )  { if ( size > 16 * (X) ) dstQuad[X] = srcQuad; }
#define HK_SET_ELEM4( X )  { HK_SET_ELEM((X)+0); HK_SET_ELEM((X)+1); HK_SET_ELEM((X)+2); HK_SET_ELEM((X)+3);}

		HK_SET_ELEM4(0);
		HK_SET_ELEM4(4);
		HK_SET_ELEM4(8);
		HK_SET_ELEM4(12);

		HK_SET_ELEM4(16);
		HK_SET_ELEM4(20);
		HK_SET_ELEM4(24);
		HK_SET_ELEM4(28);

#undef HK_SET_ELEM4
#undef HK_SET_ELEM

	}
	else
#endif
	{
		hkString::memSet16(dst, src, size/16);
	}
}

void HK_CALL hkString::memSet16(void* dst, const void* value, int numQuads)
{
#if defined (HK_PLATFORM_PS3_PPU) || defined (HK_PLATFORM_PS3_SPU)
	const vector signed int* valueQuad = reinterpret_cast<const vector signed int*>(value);
	vector signed int* dstQuad = reinterpret_cast<vector signed int*>(dst);
	{
		for (int i = numQuads-1; i>=0; i--)
		{
			*(dstQuad++) = *(valueQuad);
		}
	}
#else
	HK_ASSERT2( 0xf021d445, (hkUlong(dst) & 0xf) == 0, "Unaligned address" );
	HK_ASSERT2( 0xf021d446, (hkUlong(value) & 0xf) == 0, "Unaligned address" );
	const hkUint32* value32 = reinterpret_cast<const hkUint32*>(value);
	hkUint32* dst32 = reinterpret_cast<      hkUint32*>(dst);
	{
		for (int i = 0; i < numQuads; i++)
		{
			dst32[0] = value32[0];
			dst32[1] = value32[1];
			dst32[2] = value32[2];
			dst32[3] = value32[3];
			dst32+= 4;
		}
	}
#endif
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
