/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HK_BINARY_TAGFILE_COMMON_H
#define HK_BINARY_TAGFILE_COMMON_H

	/// Common data used in the binary tagfile reader and writer.
namespace hkBinaryTagfile
{
		/// Magic marker constants used in the file.
	enum Tags
	{
		TAG_NONE = 0,

		TAG_FILE_INFO = 1,
		
			// The following item is an hkGenericClass
		TAG_METADATA = 2,

			// The following item is an hkGenericObject
		TAG_OBJECT = 3,
			// The following item is an hkGenericObject which will be referenced again from TAG_OBJECT_BACKREF
		TAG_OBJECT_REMEMBER = 4,
			// Refer to a previously encountered object.
		TAG_OBJECT_BACKREF = 5,
			//
		TAG_OBJECT_NULL = 6,
	};

	enum
	{
		BINARY_MAGIC_0 = 0xCAB00D1E,
		BINARY_MAGIC_1 = 0xD011FACE,
	};

		/// Reverse the endianness of a hkUint32.
	inline hkUint32 HK_CALL convertEndianU32( hkUint32 n )
	{
		union fmtU32
		{
			hkUint8 b[4];
			hkUint32 v;
		};
		union fmtU32 fDataIn, fDataOut;

		fDataIn.v = n;
		fDataOut.b[0] = fDataIn.b[3];
		fDataOut.b[1] = fDataIn.b[2];
		fDataOut.b[2] = fDataIn.b[1];
		fDataOut.b[3] = fDataIn.b[0];
		return fDataOut.v;
	}

		/// Do the numbers match the binary magic numbers in either endianness?
	inline hkBool HK_CALL isBinaryMagic( hkUint32 number0, hkUint32 number1 )
	{
		return ( ((number0 == hkBinaryTagfile::BINARY_MAGIC_0) && (number1 == hkBinaryTagfile::BINARY_MAGIC_1))
			|| ((convertEndianU32(number0) == hkUint32(hkBinaryTagfile::BINARY_MAGIC_0)) && (convertEndianU32(number1) == hkUint32(hkBinaryTagfile::BINARY_MAGIC_1))) );
	}
}

#endif // HK_BINARY_TAGFILE_COMMON_H

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
