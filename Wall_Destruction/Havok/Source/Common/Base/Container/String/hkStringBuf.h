/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSTRINGBUF_H
#define HKBASE_HKSTRINGBUF_H

class hkStringPtr;

	/// String and buffer functions normally found in libc.
class hkStringBuf
{
	public:	

		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STRING, hkStringBuf );

			/// Creates an empty string.
		hkStringBuf();

			/// Creates a new string as a copy from a null terminated character array.
		hkStringBuf(const char* s);

			/// Copy from a string ptr.
		explicit hkStringBuf(const hkStringPtr& s);

			/// Construct a string as the concatenation of several strings.
			/// The list is terminated by the first null pointer.
		hkStringBuf(const char* s0, const char* s1, const char* s2=HK_NULL, const char* s3=HK_NULL, const char* s4=HK_NULL, const char* s5=HK_NULL);

			/// Creates a new string as a copy from a buffer of length len.
			/// The copied string will automatically be null terminated.
		hkStringBuf(const char* b, int len);

			/// Creates a new string as a copy of an existing one.
		hkStringBuf(const hkStringBuf& s);

 			/// Copy of an existing string.
		hkStringBuf& operator=(const hkStringBuf& s);

			/// Copy of an existing c string.
		hkStringBuf& operator=(const char* s);

	private:

			/// For internal use only
			/// Noncopying constructor, the buffer pointed by ptr is used.
		hkStringBuf(char* ptr, int size, int capacity);

			/// sets the size as length+1
		void setLength( int length );

	public:

			/// Read only access the internal buffer.
		operator const char*() const;

			/// Read only access the internal buffer.
		const char* cString() const;

			/// Returns the number of characters in this string excluding the trailing NULL
		int getLength() const;

			/// Read only access the i'th character.
		char operator[] (int i) const;


			/// Returns the first index of c given range, or -1 if not found.
			/// The index returned is from the start of the string, not from startIndex.
		int indexOf (char c, int startIndex=0, int endIndex=HK_INT32_MAX) const;
			/// Returns the first index of c given range, or -1 if not found.
			/// The index returned is from the start of the string, not from startIndex.
		int indexOf( const char* s, int startIndex=0, int endIndex=HK_INT32_MAX ) const;
			/// Returns the first index of c given range ignoring case, or -1 if not found.
		int indexOfCase( const char* s ) const;
			/// Returns the last index of c, or -1 if not found.
			/// The index returned is from the start of the string, not from startIndex.
		int lastIndexOf(char c, int startIndex=0, int endIndex=HK_INT32_MAX) const;
			/// Returns the last index of the string s, or -1 if not found.
			/// The index returned is from the start of the string, not from startIndex.
		int lastIndexOf(const char* s, int startIndex=0, int endIndex=HK_INT32_MAX) const;

			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other.
		int compareTo(const char* other) const;
			/// Returns <0,0,>0 if *this is lexicographically less than, equal to or greater than other, ignoring case.
		int compareToIgnoreCase(const char* other) const;

			/// Convenience operator for use in map<>
		hkBool operator< (const char* other) const;

			/// Returns compareTo(s)==0
		hkBool32 operator==(const char* s) const;

			/// Return true if this string starts with s.
		hkBool32 startsWith(const char* s) const;
			/// Return true if this string starts with s, ignoring case.
		hkBool32 startsWithCase(const char* s) const;

			/// Return true if this string ends with s.
		hkBool32 endsWith(const char* s) const;
			/// Return true if this string ends with s, ignoring case.
		hkBool32 endsWithCase(const char* s) const;

			/// Splits this string at the separator and returns the sub strings.
			/// This method destroys the original string as it replaces occurrences
			/// of the separator with null bytes. Returns the number of splits.
		int split( int sep, hkArray<const char*>::Temp& bits );


			/// Set this to the empty string ("")
		void clear();

			/// Overwrite the current value. Arguments are the same as for ::sprintf.
		void printf(const char* format, ...); // member function, not HK_CALL
			/// Append a formatted string. Arguments are the same as for ::sprintf.
		void appendPrintf(const char* format, ...); // member function, not HK_CALL

			/// Sets *this as the concatenation of *this and other.
		hkStringBuf& operator+= (const char* other);

		/// Parameter to hkStringBuf::replace functions
		enum ReplaceType
		{
			REPLACE_ONE,
			REPLACE_ALL
		};

		void appendJoin(const char* s0, const char* s1=HK_NULL, const char* s2=HK_NULL, const char* s3=HK_NULL, const char* s4=HK_NULL, const char* s5=HK_NULL);
		void setJoin(const char* s0, const char* s1=HK_NULL, const char* s2=HK_NULL, const char* s3=HK_NULL, const char* s4=HK_NULL, const char* s5=HK_NULL);

			/// Remove n characters from the start of this string.
		void chompStart(int n);
			/// Remove n characters from the end of this string.
		void chompEnd(int n);
			/// Set the string as a substring of itself.
		void slice(int startOffset, int length);
			/// Set the string as a substring of itself.
		void set(const char* s, int len=-1);
		void append(const char* s, int len=-1);
		void prepend(const char* s, int len=-1);
		void insert(int pos, const char* s, int len=-1);
			/// Extract the filename part after the last '/'. e.g. Path/To/File -> File.
			/// If there is no '/', the result is unchanged.
		void pathBasename();
			/// Extract the value up to the last '/'. e.g. Path/To/File -> Path/To.
			/// If there is no '/', the result is the empty string.
		void pathDirname();
			/// Extract the value after the first '/'. e.g. Path/To/File -> To/File.
			/// If there is no '/', the result is the empty string.
		void pathChildname();

			/// Returns a new string where occurrences of 'from' have been replaced with 'to'.
			/// If ReplaceType==REPLACE_ONE only the first occurrence is replaced.
		hkBool32 replace(char from, char to, ReplaceType=REPLACE_ALL);
		hkBool32 replace(const char* from, const char* to, ReplaceType=REPLACE_ALL);

			/// Make all characters lower case.
		void lowerCase();
			/// Make all characters upper case.
		void upperCase();

			// Advanced usage.
		hkArray<char>::Temp& getArray();

	private:

			/// The string storage.
			/// The storage is always null terminated and the null terminator is included in the array 
			/// size so the length of the string is always m_string.getSize()-1.
		hkInplaceArray<char, 128, hkContainerTempAllocator> m_string;
};

#	define HK_PRINTF_FORMAT_INT64 "%I64i"
#	define HK_PRINTF_FORMAT_UINT64 "%I64u"

#if HK_POINTER_SIZE==4
#	define HK_PRINTF_FORMAT_ULONG "%u"
#else
#	define HK_PRINTF_FORMAT_ULONG HK_PRINTF_FORMAT_UINT64
#endif

#include <Common/Base/Container/String/hkStringBuf.inl>

#endif // HKBASE_HKSTRING_H

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
