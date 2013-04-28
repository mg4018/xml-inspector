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

#ifndef XML_CHARACTERS_READER_HPP__f66b9cdaf20734ef11086d0851a9c563
#define XML_CHARACTERS_READER_HPP__f66b9cdaf20734ef11086d0851a9c563

#include <istream>

/**
	@file CharactersReader.hpp
*/

namespace Xml
{
	/**
		@brief Encoding stuff.
	*/
	namespace Encoding
	{
		/**
			@brief Abstract class for reading characters from XML documents.
		*/
		class CharactersReader
		{
		public:
			/**
				@brief Destructor.
			*/
			virtual ~CharactersReader();

			/**
				@brief Checks if character is allowed in XML document.

				@param codePoint Unicode character.
				@return True if character is allowed in XML document.
				@sa http://www.w3.org/TR/2008/REC-xml-20081126/#NT-Char.
			*/
			static bool IsChar(char32_t codePoint);

			/**
				@brief Checks if character is a white space.

				@param codePoint Unicode character.
				@return True if character is either space, carriage return, line feed or tab.
			*/
			static bool IsWhiteSpace(char32_t codePoint);

			/**
				@brief Checks if character is allowed to be the first character of the XML name.

				@param codePoint Unicode character.
				@return True if character is allowed to be the first character of the XML name.
				@sa http://www.w3.org/TR/2008/REC-xml-20081126/#NT-NameStartChar.
			*/
			static bool IsNameStartChar(char32_t codePoint);

			/**
				@brief Checks if character is allowed to be the one of
					the XML name characters except first.

				@param codePoint Unicode character.
				@return True if character is allowed to be the one of
					the XML name characters except first.
				@sa http://www.w3.org/TR/2008/REC-xml-20081126/#NT-NameChar.
			*/
			static bool IsNameChar(char32_t codePoint);

			/**
				@brief Checks if character is allowed to be the first character of
					the XML encoding declaration name.

				@param codePoint Unicode character.
				@return True if character is allowed to be the first character of
					the XML encoding declaration name.
				@sa http://www.w3.org/TR/2008/REC-xml-20081126/#NT-EncName.
			*/
			static bool IsEncNameStartChar(char32_t codePoint);

			/**
				@brief Checks if character is allowed to be the one of
					the XML encoding declaration name characters except first.

				@param codePoint Unicode character.
				@return True if character is allowed to be the one of
					the XML encoding declaration name characters except first.
				@sa http://www.w3.org/TR/2008/REC-xml-20081126/#NT-EncName.
			*/
			static bool IsEncNameChar(char32_t codePoint);

			/**
				@brief Gets the value represented by hexadecimal character.

				@param codePoint Unicode character.
				@return Value represented by hexadecimal character,
					or -1 if this is not the hexadecimal character.
			*/
			static int GetHexDigitValue(char32_t codePoint);

			/**
				@brief Reads one character from the specifically encoded source
					and translates it to the Unicode character.

				The source could be a stream, buffer or anything
				depending on derived class.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result) = 0;
		};

		/**
			@brief Abstract UTF-8 characters reader base class.
		*/
		class Utf8ReaderBase : public CharactersReader
		{
		protected:
			/**
				@brief Gets the bytes count of UTF-8 encoded character.

				@param leadingByte First byte of character sequence.
				@return Bytes count of UTF-8 encoded character or 0 if
					character is invalid.
			*/
			static unsigned int GetBytesCount(unsigned char leadingByte);
		public:
			/**
				@brief Destructor.
			*/
			virtual ~Utf8ReaderBase();
		};

		/**
			@brief UTF-8 characters reader from input stream.
		*/
		class Utf8StreamReader : public Utf8ReaderBase
		{
		private:
			std::istream* in;
		public:
			/**
				@brief Constructor.

				@param[in,out] inputStream Input stream from which
					a Unicode characters will be extracted.
			*/
			Utf8StreamReader(std::istream* inputStream);

			/**
				@brief Destructor.
			*/
			virtual ~Utf8StreamReader();

			/**
				@brief Reads one Unicode character.

				Depending on the character there could be from 1 to 4
				bytes needed to read from the UTF-8 encoded stream.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the pointer to the input stream.
			*/
			std::istream* GetInputStream() const;

			/**
				@brief Replaces the pointer to the input stream.
			*/
			void ResetInputStream(std::istream* inputStream);
		};

		/**
			@brief UTF-8 characters reader from iterators.

			Purpose of this class is an ability to parsing XML documents
			from iterators with a fixed encoding (UTF-8) without
			checking BOM and character set in XML declaration.
			If you are not sure how document is encoded, use Xml::Inspector
			constructor or Xml::Inspector::Reset method with specified
			iterators instead of this reader.

			You can also use the reader object without intermediate
			Xml::Inspector class:
			@code{.cpp}
            #include "CharactersReader.hpp"
            #include <iostream>
            #include <cstdlib>
            #include <string>

            int main()
            {
                // We want to convert UTF-8 string to UTF-32 string.
                std::string source =
                    u8"<root>Greek small letter pi: \U000003C0.</root>";
                std::u32string destination;
                std::u32string desired =
                    U"<root>Greek small letter pi: \U000003C0.</root>";

                Xml::Encoding::Utf8IteratorsReader<std::string::iterator> reader(
                    source.begin(), source.end());

                char32_t c;
                while (reader.ReadCharacter(c) == 1)
                    destination.push_back(c);

                if (destination == desired)
                    std::cout << "OK!\n";
                else
                    std::cout << "Not OK :(\n";

                return EXIT_SUCCESS;
            }
			@endcode
			Result:
			@verbatim
			OK!
			@endverbatim
		*/
		template <typename TInputIterator>
		class Utf8IteratorsReader : public Utf8ReaderBase
		{
		private:
			TInputIterator iterBegin;
			TInputIterator iterEnd;
		public:
			/**
				@brief Constructor.

				@param first,last Input iterators to the initial
					and final positions in a sequence of bytes. The range used
					is [first,last), which contains all the bytes
					between first and last, including the element pointed
					by first but not the element pointed by last.
			*/
			Utf8IteratorsReader(TInputIterator first, TInputIterator last);

			/**
				@brief Destructor.
			*/
			virtual ~Utf8IteratorsReader();

			/**
				@brief Reads one Unicode character.

				Depending on the character there could be from 1 to 4
				bytes per one Unicode character.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the first iterator.
			*/
			TInputIterator GetFirstIterator() const;
			
			/**
				@brief Gets the last iterator.
			*/
			TInputIterator GetLastIterator() const;

			/**
				@brief Replaces the iterators.

				@param first,last Input iterators to the initial
					and final positions in a sequence of bytes. The range used
					is [first,last), which contains all the bytes
					between first and last, including the element pointed
					by first but not the element pointed by last.
			*/
			void ResetIterators(TInputIterator first, TInputIterator last);
		};

		/**
			@brief UTF-16 (big endian) characters reader from input stream.
		*/
		class Utf16BEStreamReader : public CharactersReader
		{
		private:
			std::istream* in;
		public:
			/**
				@brief Constructor.

				@param[in,out] inputStream Input stream from which
					a Unicode characters will be extracted.
			*/
			Utf16BEStreamReader(std::istream* inputStream);

			/**
				@brief Destructor.
			*/
			virtual ~Utf16BEStreamReader();

			/**
				@brief Reads one Unicode character.

				Depending on the character there could be a 2 or 4
				bytes needed to read from the UTF-16 (big endian) encoded stream.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the pointer to the input stream.
			*/
			std::istream* GetInputStream() const;

			/**
				@brief Replaces the pointer to the input stream.
			*/
			void ResetInputStream(std::istream* inputStream);
		};

		/**
			@brief UTF-16 (little endian) characters reader from the input stream.
		*/
		class Utf16LEStreamReader : public CharactersReader
		{
		private:
			std::istream* in;
		public:
			/**
				@brief Constructor.

				@param[in,out] inputStream Input stream from which
					a Unicode characters will be extracted.
			*/
			Utf16LEStreamReader(std::istream* inputStream);

			/**
				@brief Destructor.
			*/
			virtual ~Utf16LEStreamReader();

			/**
				@brief Reads one Unicode character.

				Depending on the character there could be a 2 or 4
				bytes needed to read from the UTF-16 (little endian) encoded stream.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the pointer to the input stream.
			*/
			std::istream* GetInputStream() const;

			/**
				@brief Replaces the pointer to the input stream.
			*/
			void ResetInputStream(std::istream* inputStream);
		};

		/**
			@brief UTF-32 (big endian) characters reader from the input stream.
		*/
		class Utf32BEStreamReader : public CharactersReader
		{
		private:
			std::istream* in;
		public:
			/**
				@brief Constructor.

				@param[in,out] inputStream Input stream from which
					a Unicode characters will be extracted.
			*/
			Utf32BEStreamReader(std::istream* inputStream);

			/**
				@brief Destructor.
			*/
			virtual ~Utf32BEStreamReader();

			/**
				@brief Reads one Unicode character.

				There are 4 bytes needed to read per one character
				from the UTF-32 (big endian) encoded stream.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the pointer to the input stream.
			*/
			std::istream* GetInputStream() const;

			/**
				@brief Replaces the pointer to the input stream.
			*/
			void ResetInputStream(std::istream* inputStream);
		};

		/**
			@brief UTF-32 (little endian) characters reader from the input stream.
		*/
		class Utf32LEStreamReader : public CharactersReader
		{
		private:
			std::istream* in;
		public:
			/**
				@brief Constructor.

				@param[in,out] inputStream Input stream from which
					a Unicode characters will be extracted.
			*/
			Utf32LEStreamReader(std::istream* inputStream);

			/**
				@brief Destructor.
			*/
			virtual ~Utf32LEStreamReader();

			/**
				@brief Reads one Unicode character.

				There are 4 bytes needed to read per one character
				from the UTF-32 (little endian) encoded stream.

				@param[out] result Variable that will receive the Unicode character.
				@return @b 1 if the character was read successfully.
					@b 0 if there are no more characters to read.
					@b -1 if character is not allowed in XML document.
					@b -2 if there was an error while reading character.
			*/
			virtual int ReadCharacter(char32_t& result);

			/**
				@brief Gets the pointer to the input stream.
			*/
			std::istream* GetInputStream() const;

			/**
				@brief Replaces the pointer to the input stream.
			*/
			void ResetInputStream(std::istream* inputStream);
		};

		//
		// CharactersReader implementation.
		//
		
		inline CharactersReader::~CharactersReader()
		{
		
		}

		inline bool CharactersReader::IsChar(char32_t codePoint)
		{
			if (codePoint < 0xD800)
			{
				return (codePoint >= 0x20 ||
					codePoint == 0x09 ||
					codePoint == 0x0A ||
					codePoint == 0x0D);
			}

			return ((codePoint >= 0xE000 && codePoint <= 0xFFFD) ||
				(codePoint >= 0x10000 && codePoint <= 0x10FFFF));
		}

		inline bool CharactersReader::IsWhiteSpace(char32_t codePoint)
		{
			return (codePoint == 0x20 || codePoint == 0x0A ||
				codePoint == 0x09 || codePoint == 0x0D);
		}

		inline bool CharactersReader::IsNameStartChar(char32_t codePoint)
		{
			static const char Allowed1Byte[256] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1
			};
			if (codePoint <= 0xFF)
				return (Allowed1Byte[static_cast<unsigned char>(codePoint)] != 0);

			return (
				(codePoint <= 0x2FF) ||
				(codePoint >= 0x370 && codePoint <= 0x37D) ||
				(codePoint >= 0x37F && codePoint <= 0x1FFF) ||
				(codePoint >= 0x200C && codePoint <= 0x200D) ||
				(codePoint >= 0x2070 && codePoint <= 0x218F) ||
				(codePoint >= 0x2C00 && codePoint <= 0x2FEF) ||
				(codePoint >= 0x3001 && codePoint <= 0xD7FF) ||
				(codePoint >= 0xF900 && codePoint <= 0xFDCF) ||
				(codePoint >= 0xFDF0 && codePoint <= 0xFFFD) ||
				(codePoint >= 0x10000 && codePoint <= 0xEFFFF));
		}

		inline bool CharactersReader::IsNameChar(char32_t codePoint)
		{
			static const char Allowed1Byte[256] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1
			};
			if (codePoint <= 0xFF)
				return (Allowed1Byte[static_cast<unsigned char>(codePoint)] != 0);

			return (
				(codePoint <= 0x2FF) ||
				(codePoint >= 0x300 && codePoint <= 0x37D) ||
				(codePoint >= 0x37F && codePoint <= 0x1FFF) ||
				(codePoint >= 0x200C && codePoint <= 0x200D) ||
				(codePoint >= 0x203F && codePoint <= 0x2040) ||
				(codePoint >= 0x2070 && codePoint <= 0x218F) ||
				(codePoint >= 0x2C00 && codePoint <= 0x2FEF) ||
				(codePoint >= 0x3001 && codePoint <= 0xD7FF) ||
				(codePoint >= 0xF900 && codePoint <= 0xFDCF) ||
				(codePoint >= 0xFDF0 && codePoint <= 0xFFFD) ||
				(codePoint >= 0x10000 && codePoint <= 0xEFFFF));
		}

		inline bool CharactersReader::IsEncNameStartChar(char32_t codePoint)
		{
			static const char Allowed[256] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			return (codePoint <= 0xFF &&
				Allowed[static_cast<unsigned char>(codePoint)] != 0);
		}

		inline bool CharactersReader::IsEncNameChar(char32_t codePoint)
		{
			static const char Allowed[256] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
				0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			return (codePoint <= 0xFF &&
				Allowed[static_cast<unsigned char>(codePoint)] != 0);
		}

		inline int CharactersReader::GetHexDigitValue(char32_t codePoint)
		{
			static const signed char HexValue[256] =
			{
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
				-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 
			};
			if (codePoint <= 0xFF)
				return static_cast<int>(HexValue[static_cast<unsigned char>(codePoint)]);
			return -1;
		}

		//
		// Utf8ReaderBase implementation.
		//
		
		inline unsigned int Utf8ReaderBase::GetBytesCount(unsigned char leadingByte)
		{
			// 0 means invalid leading byte.
			static const unsigned char BytesCount[256] =
			{
				0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
				3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
				4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
			};
			return BytesCount[leadingByte];
		}

		inline Utf8ReaderBase::~Utf8ReaderBase()
		{
		
		}

		//
		// Utf8StreamReader implementation.
		//

		inline Utf8StreamReader::Utf8StreamReader(std::istream* inputStream)
			: in(inputStream)
		{
		
		}

		inline Utf8StreamReader::~Utf8StreamReader()
		{
		
		}

		inline int Utf8StreamReader::ReadCharacter(char32_t& result)
		{
			if (in != nullptr)
			{
				// Try to read the first code unit (leading byte).
				int leadingByte = in->get();

				// If not failbit.
				if (leadingByte != std::char_traits<char>::eof())
				{
					unsigned int bytesCount = GetBytesCount(static_cast<unsigned char>(leadingByte));

					if (bytesCount == 1)
					{
						result = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
						return 1; // The character was read successfully.
					}

					if (bytesCount != 0) // 2, 3 or 4.
					{
						result = 0;
						char32_t continuationByte = 0;
						int codeUnit;
						for (unsigned int i = 1; i < bytesCount; ++i)
						{
							codeUnit = in->get();

							// If failbit...
							if (codeUnit == std::char_traits<char>::eof())
							{
								// Because of the end of the stream.
								if ((in->rdstate() & std::istream::eofbit) != 0)
								{
									// End of the stream, but the leading byte told us
									// there should be the continuation byte, so we treat
									// this like invalid character.
									result = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
									return -1; // Invalid character.
								}
								return -2; // Something terrible with the stream.
							}

							if ((codeUnit & 0xC0) != 0x80)
							{
								// Two most significant bits in continuation byte should be 1 followed by 0.
								result = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
								return -1; // Invalid character.
							}

							continuationByte = static_cast<char32_t>(static_cast<unsigned char>(codeUnit));
							continuationByte &= 0x3F; // Hide 2 most significant bits.
							continuationByte <<= ((bytesCount - i - 1) * 6);
							result |= continuationByte;
						}

						if (bytesCount == 2)
						{
							continuationByte = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
							continuationByte = (continuationByte & 0x1F) << 6;
							result |= continuationByte;
							if (result > 0x7F)
								return 1;
							return -1;
						}

						if (bytesCount == 3)
						{
							continuationByte = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
							continuationByte = (continuationByte & 0x0F) << 12;
							result |= continuationByte;
							if ((result > 0x07FF && result < 0xD800) ||
								(result > 0xDFFF && result <= 0xFFFD))
								return 1;
							return -1;
						}

						// bytesCount == 4.
						continuationByte = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
						continuationByte = (continuationByte & 0x07) << 18;
						result |= continuationByte;
						if (result > 0xFFFF && result <= 0x10FFFF)
							return 1;
						return -1;
					}

					result = static_cast<char32_t>(static_cast<unsigned char>(leadingByte));
					return -1; // Invalid character.
				}
				else if ((in->rdstate() & std::istream::eofbit) != 0)
				{
					return 0; // End of the stream.
				}
			}
			return -2; // Something terrible with the stream.
		}

		inline std::istream* Utf8StreamReader::GetInputStream() const
		{
			return in;
		}

		inline void Utf8StreamReader::ResetInputStream(std::istream* inputStream)
		{
			in = inputStream;
		}

		//
		// Utf8IteratorsReader implementation.
		//
		template <typename TInputIterator>
		inline Utf8IteratorsReader<TInputIterator>::Utf8IteratorsReader(
			TInputIterator first, TInputIterator last)
		{
			iterBegin = first;
			iterEnd = last;
		}

		template <typename TInputIterator>
		inline Utf8IteratorsReader<TInputIterator>::~Utf8IteratorsReader()
		{
		
		}

		template <typename TInputIterator>
		inline int Utf8IteratorsReader<TInputIterator>::ReadCharacter(char32_t& result)
		{
			if (iterBegin != iterEnd)
			{
				unsigned char leadingByte = static_cast<unsigned char>(*iterBegin);
				unsigned int bytesCount = GetBytesCount(leadingByte);

				++iterBegin;
				if (bytesCount == 1)
				{
					result = static_cast<char32_t>(leadingByte);
					return 1; // The character was read successfully.
				}

				if (bytesCount != 0) // 2, 3 or 4.
				{
					result = 0;
					char32_t continuationByte = 0;
					unsigned char codeUnit;
					for (unsigned int i = 1; i < bytesCount; ++i, ++iterBegin)
					{
						if (iterBegin == iterEnd)
						{
							// End of the sequence, but the leading byte told us
							// there should be the continuation byte, so we treat
							// this like invalid character.
							result = static_cast<char32_t>(leadingByte);
							return -1; // Invalid character.
						}
						codeUnit = static_cast<unsigned char>(*iterBegin);

						if ((codeUnit & 0xC0) != 0x80)
						{
							// Two most significant bits in continuation byte should be 1 followed by 0.
							result = static_cast<char32_t>(leadingByte);
							return -1; // Invalid character.
						}

						continuationByte = static_cast<char32_t>(codeUnit);
						continuationByte &= 0x3F; // Hide 2 most significant bits.
						continuationByte <<= ((bytesCount - i - 1) * 6);
						result |= continuationByte;
					}

					if (bytesCount == 2)
					{
						continuationByte = static_cast<char32_t>(leadingByte);
						continuationByte = (continuationByte & 0x1F) << 6;
						result |= continuationByte;
						if (result > 0x7F)
							return 1;
						return -1;
					}

					if (bytesCount == 3)
					{
						continuationByte = static_cast<char32_t>(leadingByte);
						continuationByte = (continuationByte & 0x0F) << 12;
						result |= continuationByte;
						if ((result > 0x07FF && result < 0xD800) ||
							(result > 0xDFFF && result <= 0xFFFD))
							return 1;
						return -1;
					}

					// bytesCount == 4.
					continuationByte = static_cast<char32_t>(leadingByte);
					continuationByte = (continuationByte & 0x07) << 18;
					result |= continuationByte;
					if (result > 0xFFFF && result <= 0x10FFFF)
						return 1;
					return -1;
				}

				result = static_cast<char32_t>(leadingByte);
				return -1; // Invalid character.
			}
			return 0;
		}

		template <typename TInputIterator>
		inline TInputIterator Utf8IteratorsReader<TInputIterator>::GetFirstIterator() const
		{
			return iterBegin;
		}

		template <typename TInputIterator>
		inline TInputIterator Utf8IteratorsReader<TInputIterator>::GetLastIterator() const
		{
			return iterEnd;
		}

		template <typename TInputIterator>
		inline void Utf8IteratorsReader<TInputIterator>::ResetIterators(
			TInputIterator first, TInputIterator last)
		{
			iterBegin = first;
			iterEnd = last;
		}

		//
		// Utf16BEStreamReader implementation.
		//

		inline Utf16BEStreamReader::Utf16BEStreamReader(std::istream* inputStream)
			: in(inputStream)
		{
		
		}

		inline Utf16BEStreamReader::~Utf16BEStreamReader()
		{
		
		}

		inline int Utf16BEStreamReader::ReadCharacter(char32_t& result)
		{
			if (in != nullptr)
			{
				// First byte.
				int oneByte = in->get();

				// If not failbit.
				if (oneByte != std::char_traits<char>::eof())
				{
					result = static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					// Second byte.
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					result <<= 8;
					result |= static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					if (result < 0xD800)
					{
						if (result >= 0x20 ||
							result == 0x09 ||
							result == 0x0A ||
							result == 0x0D)
							return 1;
						return -1;
					}
					if (result > 0xDFFF)
					{
						if (result <= 0xFFFD)
							return 1;
						return -1;
					}

					// We have lead surrogate, so next code unit should be a trail surrogate.

					// Third byte (first of trail surrogate).
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					char32_t trailSurrogate =
						static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					// Fourth byte (second of trail surrogate).
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					trailSurrogate <<= 8;
					trailSurrogate |= static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					if (trailSurrogate >= 0xDC00 && trailSurrogate <= 0xDFFF)
					{
						result -= 0xD800;
						result <<= 10;
						result |= (trailSurrogate - 0xDC00);
						result += 0x10000;
						return 1;
					}
					return -1;
				}
				else if ((in->rdstate() & std::istream::eofbit) != 0)
				{
					return 0; // End of the stream.
				}
			}
			return -2; // Something terrible with the stream.
		}

		inline std::istream* Utf16BEStreamReader::GetInputStream() const
		{
			return in;
		}

		inline void Utf16BEStreamReader::ResetInputStream(std::istream* inputStream)
		{
			in = inputStream;
		}

		//
		// Utf16LEStreamReader implementation.
		//

		inline Utf16LEStreamReader::Utf16LEStreamReader(std::istream* inputStream)
			: in(inputStream)
		{
		
		}

		inline Utf16LEStreamReader::~Utf16LEStreamReader()
		{
		
		}

		inline int Utf16LEStreamReader::ReadCharacter(char32_t& result)
		{
			if (in != nullptr)
			{
				// First byte.
				int oneByte = in->get();

				// If not failbit.
				if (oneByte != std::char_traits<char>::eof())
				{
					result = static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					// Second byte.
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					result |= (static_cast<char32_t>(static_cast<unsigned char>(oneByte)) << 8);

					if (result < 0xD800)
					{
						if (result >= 0x20 ||
							result == 0x09 ||
							result == 0x0A ||
							result == 0x0D)
							return 1;
						return -1;
					}
					if (result > 0xDFFF)
					{
						if (result <= 0xFFFD)
							return 1;
						return -1;
					}

					// We have lead surrogate, so next code unit should be a trail surrogate.

					// Third byte (second of trail surrogate in little endian).
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					char32_t trailSurrogate =
						static_cast<char32_t>(static_cast<unsigned char>(oneByte));

					// Fourth byte (second of trail surrogate).
					oneByte = in->get();

					if (oneByte == std::char_traits<char>::eof())
					{
						if ((in->rdstate() & std::istream::eofbit) != 0)
							return -1; // Invalid character.
						return -2; // Something terrible with the stream.
					}

					trailSurrogate |= (static_cast<char32_t>(static_cast<unsigned char>(oneByte)) << 8);

					if (trailSurrogate >= 0xDC00 && trailSurrogate <= 0xDFFF)
					{
						result -= 0xD800;
						result <<= 10;
						result |= (trailSurrogate - 0xDC00);
						result += 0x10000;
						return 1;
					}
					return -1;
				}
				else if ((in->rdstate() & std::istream::eofbit) != 0)
				{
					return 0; // End of the stream.
				}
			}
			return -2; // Something terrible with the stream.
		}

		inline std::istream* Utf16LEStreamReader::GetInputStream() const
		{
			return in;
		}

		inline void Utf16LEStreamReader::ResetInputStream(std::istream* inputStream)
		{
			in = inputStream;
		}

		//
		// Utf32BEStreamReader implementation.
		//

		inline Utf32BEStreamReader::Utf32BEStreamReader(std::istream* inputStream)
			: in(inputStream)
		{
		
		}

		inline Utf32BEStreamReader::~Utf32BEStreamReader()
		{
		
		}

		inline int Utf32BEStreamReader::ReadCharacter(char32_t& result)
		{
			if (in != nullptr)
			{
				char32_t codePoint, temp;

				// First byte.
				int oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return 0; // End of the stream.
					return -2;
				}

				codePoint = static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				codePoint <<= 24;

				// Second byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				temp = static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				temp <<= 16;
				codePoint |= temp;

				// Third byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				temp = static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				temp <<= 8;
				codePoint |= temp;

				// Fourth byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				codePoint |= static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				result = codePoint;
				if (IsChar(result))
					return 1;
				return -1;
			}
			return -2; // Something terrible with the stream.
		}

		inline std::istream* Utf32BEStreamReader::GetInputStream() const
		{
			return in;
		}

		inline void Utf32BEStreamReader::ResetInputStream(std::istream* inputStream)
		{
			in = inputStream;
		}

		//
		// Utf32LEStreamReader implementation.
		//

		inline Utf32LEStreamReader::Utf32LEStreamReader(std::istream* inputStream)
			: in(inputStream)
		{
		
		}

		inline Utf32LEStreamReader::~Utf32LEStreamReader()
		{
		
		}

		inline int Utf32LEStreamReader::ReadCharacter(char32_t& result)
		{
			if (in != nullptr)
			{
				char32_t codePoint, temp;

				// First byte.
				int oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return 0; // End of the stream.
					return -2;
				}

				codePoint = static_cast<char32_t>(static_cast<unsigned char>(oneByte));

				// Second byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				temp = static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				temp <<= 8;
				codePoint |= temp;

				// Third byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				temp = static_cast<char32_t>(static_cast<unsigned char>(oneByte));
				temp <<= 16;
				codePoint |= temp;

				// Fourth byte.
				oneByte = in->get();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((in->rdstate() & std::istream::eofbit) != 0)
						return -1;
					return -2;
				}

				codePoint |= (static_cast<char32_t>(static_cast<unsigned char>(oneByte)) << 24);
				result = codePoint;
				if (IsChar(result))
					return 1;
				return -1;
			}
			return -2; // Something terrible with the stream.
		}

		inline std::istream* Utf32LEStreamReader::GetInputStream() const
		{
			return in;
		}

		inline void Utf32LEStreamReader::ResetInputStream(std::istream* inputStream)
		{
			in = inputStream;
		}
	}
}

#endif

