/*
  Copyright (C) 2013 Przemek Mazurkiewicz (Przemek.Mazurkiewicz.2009@gmail.com)

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#ifndef XML_FINAL_ENCODING_HPP__f66b9cdaf20734ef11086d0851a9c563
#define XML_FINAL_ENCODING_HPP__f66b9cdaf20734ef11086d0851a9c563

#include <string>

/**
	@file FinalEncoding.hpp
*/

namespace Xml
{
	/**
		@brief Class responsible for writing strings in UTF-8 encoding.

		TODO: detailed description...

		@sa Utf16FinalEncoding, Utf32FinalEncoding and Inspector.
	*/
	class Utf8FinalEncoding
	{
	public:
		/**
			@brief String type that is able to store UTF-8 string.
		*/
		typedef std::string StringType;

		/**
			@brief Inserts Unicode character into the UTF-8 encoded string.

			TODO: detailed description...

			@param[out] stringDestination String where the character should be inserted.
			@param[in] codePoint Code point of Unicode character to write.
		*/
		static void PutCharacter(StringType& stringDestination, char32_t codePoint);
	};

	/**
		@brief Class responsible for writing strings in UTF-16 encoding.

		TODO: detailed description...

		@sa Utf8FinalEncoding, Utf32FinalEncoding and Inspector.
	*/
	class Utf16FinalEncoding
	{
	public:
		/**
			@brief String type that is able to store UTF-16 string.
		*/
		typedef std::u16string StringType;

		/**
			@brief Inserts Unicode character into the UTF-16 encoded string.

			TODO: detailed description...

			@param[out] stringDestination String where the character should be inserted.
			@param[in] codePoint Code point of Unicode character to write.
		*/
		static void PutCharacter(StringType& stringDestination, char32_t codePoint);
	};

	/**
		@brief Class responsible for writing strings in UTF-32 encoding.

		TODO: detailed description...

		@sa Utf8FinalEncoding, Utf16FinalEncoding and Inspector.
	*/
	class Utf32FinalEncoding
	{
	public:
		/**
			@brief String type that is able to store UTF-32 string.
		*/
		typedef std::u32string StringType;

		/**
			@brief Inserts Unicode character into the UTF-32 encoded string.

			TODO: detailed description...

			@param[out] stringDestination String where the character should be inserted.
			@param[in] codePoint Code point of Unicode character to write.
		*/
		static void PutCharacter(StringType& stringDestination, char32_t codePoint);
	};

	inline void Utf8FinalEncoding::PutCharacter(
		StringType& stringDestination, char32_t codePoint)
	{
		if (codePoint <= 0x7F)
		{
			stringDestination.push_back(static_cast<StringType::value_type>(codePoint));
		}
		else if (codePoint <= 0x7FF)
		{
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint >> 6) | 0xC0));
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint & 0x3F) | 0x80));
		}
		else if (codePoint <= 0xFFFF)
		{
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint >> 12) | 0xE0));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 6) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint & 0x3F) | 0x80));
		}
		else if (codePoint <= 0x1FFFFF)
		{
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint >> 18) | 0xF0));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 12) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 6) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint & 0x3F) | 0x80));
		}
		// Invalid character. Put this anyway.
		else if (codePoint <= 0x3FFFFFF)
		{
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint >> 24) | 0xF8));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 18) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 12) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 6) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint & 0x3F) | 0x80));
		}
		else
		{
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint >> 30) | 0xFC));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 24) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 18) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 12) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				((codePoint >> 6) & 0x3F) | 0x80));
			stringDestination.push_back(static_cast<StringType::value_type>(
				(codePoint & 0x3F) | 0x80));
		}
	}

	inline void Utf16FinalEncoding::PutCharacter(
		StringType& stringDestination, char32_t codePoint)
	{
		if ((codePoint <= 0xD7FF) || (codePoint >= 0xE000 && codePoint <= 0xFFFF))
		{
			stringDestination.push_back(static_cast<StringType::value_type>(codePoint));
		}
		else if (codePoint >= 0x10000 && codePoint <= 0x10FFFF)
		{
			// We have surrogate pair.
			codePoint -= 0x10000;
			char16_t surr = (codePoint >> 10) + 0xD800; // Lead surrogate.
			stringDestination.push_back(static_cast<StringType::value_type>(surr));
			surr = (codePoint & 0x3FF) + 0xDC00; // Trail surrogate.
			stringDestination.push_back(static_cast<StringType::value_type>(surr));
		}
	}

	inline void Utf32FinalEncoding::PutCharacter(
		StringType& stringDestination, char32_t codePoint)
	{
		stringDestination.push_back(static_cast<StringType::value_type>(codePoint));
	}
}

#endif

