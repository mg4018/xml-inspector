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
#include <string>

/**
	@file CharactersReader.hpp
*/

namespace Xml
{
	/**
		@brief Abstract class for reading characters.
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

			@param[in] codePoint Code point of Unicode character.
			@return True if character is allowed in XML document.
		*/
		static bool IsAllowed(char32_t codePoint);

		/**
			@brief Reads one character from the specifically encoded source
				and translates it to Unicode character.

			The source could be a stream, buffer or anything
			depending on derived class.

			@param[out] result Unicode character.
			@return @b 1 if the character was read successfully.
				@b 0 if there are no more characters to read.
				@b -1 if character is not allowed in XML document.
				@b -2 if there was an error while reading character.
		*/
		virtual int ReadCharacter(char32_t& result) = 0;
	};

	/**
		@brief UTF-8 characters reader from input stream.
	*/
	class Utf8StreamReader : public CharactersReader
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

			@param[out] result Unicode character.
			@return @b 1 if the character was read successfully.
				@b 0 if there are no more characters to read.
				@b -1 if character is not allowed in XML document.
				@b -2 if there was an error while reading character.
		*/
		virtual int ReadCharacter(char32_t& result);

		/**
			@brief Gets the pointer to input stream.
		*/
		std::istream* GetInputStream() const;

		/**
			@brief Replaces the pointer to input stream.
		*/
		void ResetInputStream(std::istream* inputStream);
	};

	/**
		@brief UTF-16 characters reader from input stream.
	*/
	class Utf16StreamReader : public CharactersReader
	{
	private:
		std::istream* in;
	public:
		/**
			@brief Constructor.

			@param[in,out] inputStream Input stream from which
				a Unicode characters will be extracted.
		*/
		Utf16StreamReader(std::istream* inputStream);

		/**
			@brief Destructor.
		*/
		virtual ~Utf16StreamReader();

		/**
			@brief Reads one Unicode character.

			Depending on the character there could be a 2 or 4
			bytes needed to read from the UTF-16 encoded stream.

			@param[out] result Unicode character.
			@return @b 1 if the character was read successfully.
				@b 0 if there are no more characters to read.
				@b -1 if character is not allowed in XML document.
				@b -2 if there was an error while reading character.
		*/
		virtual int ReadCharacter(char32_t& result);

		/**
			@brief Gets the pointer to input stream.
		*/
		std::istream* GetInputStream() const;

		/**
			@brief Replaces the pointer to input stream.
		*/
		void ResetInputStream(std::istream* inputStream);
	};

	/**
		@brief UTF-32 characters reader from input stream.
	*/
	class Utf32StreamReader : public CharactersReader
	{
	private:
		std::istream* in;
	public:
		/**
			@brief Constructor.

			@param[in,out] inputStream Input stream from which
				a Unicode characters will be extracted.
		*/
		Utf32StreamReader(std::istream* inputStream);

		/**
			@brief Destructor.
		*/
		virtual ~Utf32StreamReader();

		/**
			@brief Reads one Unicode character.

			There are 4 bytes needed to read per one character
			from the UTF-32 encoded stream.

			@param[out] result Unicode character.
			@return @b 1 if the character was read successfully.
				@b 0 if there are no more characters to read.
				@b -1 if character is not allowed in XML document.
				@b -2 if there was an error while reading character.
		*/
		virtual int ReadCharacter(char32_t& result);

		/**
			@brief Gets the pointer to input stream.
		*/
		std::istream* GetInputStream() const;

		/**
			@brief Replaces the pointer to input stream.
		*/
		void ResetInputStream(std::istream* inputStream);
	};

	//
	// CharactersReader implementation.
	//
	
	inline CharactersReader::~CharactersReader()
	{
	
	}

	inline bool CharactersReader::IsAllowed(char32_t codePoint)
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
		
		if (in != nullptr)
		{
			// Try to read the first code unit (leading byte).
			int leadingByte = in->get();

			// If not failbit.
			if (leadingByte != std::char_traits<char>::eof())
			{
				unsigned int bytesCount = BytesCount[static_cast<unsigned int>(leadingByte)];

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
	// Utf16StreamReader implementation.
	//

	inline Utf16StreamReader::Utf16StreamReader(std::istream* inputStream)
		: in(inputStream)
	{
	
	}

	inline Utf16StreamReader::~Utf16StreamReader()
	{
	
	}

	inline int Utf16StreamReader::ReadCharacter(char32_t& result)
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

	inline std::istream* Utf16StreamReader::GetInputStream() const
	{
		return in;
	}

	inline void Utf16StreamReader::ResetInputStream(std::istream* inputStream)
	{
		in = inputStream;
	}

	//
	// Utf32StreamReader implementation.
	//

	inline Utf32StreamReader::Utf32StreamReader(std::istream* inputStream)
		: in(inputStream)
	{
	
	}

	inline Utf32StreamReader::~Utf32StreamReader()
	{
	
	}

	inline int Utf32StreamReader::ReadCharacter(char32_t& result)
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
			if (IsAllowed(result))
				return 1;
			return -1;
		}
		return -2; // Something terrible with the stream.
	}

	inline std::istream* Utf32StreamReader::GetInputStream() const
	{
		return in;
	}

	inline void Utf32StreamReader::ResetInputStream(std::istream* inputStream)
	{
		in = inputStream;
	}
}

#endif

