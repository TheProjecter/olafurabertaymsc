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

class hkStandardRttiParser
{
	public:
		typedef hkRttiParser::Type Type;
		typedef hkRttiParser::Node Node;
		typedef hkRttiParser::Allocator Allocator;

			/// Parses a standard C++ type declaration
			/// Does not support function types
			/// Does not support non types as template parameters.
		static Node* HK_CALL parseType(const hkSubString& subString, Allocator& allocator);

	protected:
		enum TokenType
		{
			TOKEN_REFERENCE,
			TOKEN_POINTER,
			TOKEN_IDENTIFIER,
			TOKEN_CLASS,
			TOKEN_ENUM,
			TOKEN_BUILT_IN,			
			TOKEN_UNSIGNED,			
			TOKEN_LT,				
			TOKEN_GT,				
			TOKEN_COMMA,			
			TOKEN_TERMINATOR,		
			TOKEN_OPEN_SQUARE,		
			TOKEN_CLOSE_SQUARE,		
			TOKEN_UINT_VALUE,		
		};

		struct Token
		{
			TokenType m_token;				
			Type m_builtInType;			
			hkSubString m_string;		
		};
		struct TokenRange
		{
			const Token* m_start;
			const Token* m_end;
		};
		struct TokenContainer
		{
			TokenContainer():
				m_numTokens(0)
			{
			}
			void addToken(TokenType tokenType, const hkSubString& string = hkSubString(), Type builtInType = hkRttiParser::TYPE_UNKNOWN)
			{
				HK_ASSERT(0x2423423, m_numTokens < MAX_TOKENS);
				Token& token = m_tokens[m_numTokens++];
				token.m_token = tokenType;
				token.m_string = string;
				token.m_builtInType = builtInType;
			}

			enum { MAX_TOKENS = 64 };

			int m_numTokens;
			Token m_tokens[MAX_TOKENS];
		};

		/// Parses the type to produce a token stream. Returns true on success.
		static hkBool HK_CALL parseTokens(const hkSubString& str, TokenContainer& container);
		HK_FORCE_INLINE static hkBool HK_CALL isLetter(char c)
		{
			return (c >= 'a' && c <='z') || (c >= 'A' && c <= 'Z');
		}
		HK_FORCE_INLINE static hkBool HK_CALL isDigit(char c)
		{
			return (c >= '0' && c <= '9');
		}
		/// Returns the basic type or TYPE_UNKNOWN if not a basic type
		static Type HK_CALL calcBuiltInType(const hkSubString& subString);


		static Node* HK_CALL _parseType(TokenRange& range, Allocator& allocator);
		static Node* HK_CALL _parseTerminalType(TokenRange& range, Allocator& allocator);
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
