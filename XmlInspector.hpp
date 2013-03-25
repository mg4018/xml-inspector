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

#ifndef XML_INSPECTOR_HPP__f66b9cdaf20734ef11086d0851a9c563
#define XML_INSPECTOR_HPP__f66b9cdaf20734ef11086d0851a9c563

#include "CharactersReader.hpp"
#include "FinalEncoding.hpp"
#include <string>
#include <istream>
#include <cstdint>

/**
	@file XmlInspector.hpp
*/

/**
	@brief Main namespace for XML solutions like XML parser and some encoding stuff.
*/
namespace Xml
{
	/**
		@brief Primary XML parser class.

		TODO: detailed description...

		@tparam TFinalEncoding Desired encoding. You don't need to care how XML file is encoded.
			You can choose how you want to read XML strings between Utf8FinalEncoding, Utf16FinalEncoding
			and Utf32FinalEncoding class from FinalEncoding.hpp file. They respectively store the strings in
			@a std::string, @a std::u16string and @a std::u32string. You can also write your own fancy way of
			storing strings. For example you may want to use @a std::wstring and even other than Unicode encoding.
	*/
	template <typename TFinalEncoding>
	class Inspector
	{
	public:
		/**
			@brief Alias of encoding type that is used to write strings.
		*/
		typedef TFinalEncoding FinalEncodingType;

		/**
			@brief String type provided by FinalEncodingType.
		*/
		typedef typename TFinalEncoding::StringType StringType;

		/**
			@brief Unsigned integer type definition for determining location in XML document.

			This type should be enough to store any file size or memory buffer size.
		*/
		typedef uint_least64_t SizeType;
	private:
		SizeType lastLineNumber;
		SizeType lastLinePosition;
		SizeType currentLineNumber;
		SizeType currentLinePosition;

		// Last numbers <= current numbers.
		void SaveNumbers();
	public:
		/**
			@brief Initializes a new instance of the Inspector class.
		*/
		Inspector();

		/**
			@brief Initializes a new instance of the Inspector class
				with the specified file path.
		*/
		Inspector(const char* filePath);

		/**
			@brief Initializes a new instance of the Inspector class
				with the specified file path.
		*/
		Inspector(const std::string& filePath);

		/**
			@brief Initializes a new instance of the Inspector class
				with the specified stream.

			TODO: warning - stream from this pointer must exist while reading nodes.
		*/
		Inspector(std::istream* inputStream);

		/**
			@brief Initializes a new instance of the Inspector class
				with the specified iterators.

			@param[in] first,last Input iterators to the initial
				and final positions in a sequence. The range used
				is [first,last), which contains all the elements
				between first and last, including the element pointed
				by first but not the element pointed by last.
		*/
		template <typename TInputIterator>
		Inspector(TInputIterator first, TInputIterator last);

		/**
			@brief Initializes a new instance of the Inspector class
				with the specified characters reader interface.

			TODO: warning message about BOM.
		*/
		Inspector(CharactersReader* reader);

		/**
			@brief Reads the next node from the stream.

			TODO: detailed description...

			@return True if the next node was read successfully.
				False if there are no more nodes to read.
		*/
		bool ReadNode();

		/**
			@brief Gets the current line number.

			Starting value is 1. For example:
			@verbatim
			<root>
			   <a>aaa</a>
			   <b>
				   bbb
			   </b>
			</root>
			@endverbatim
			Line number of @a bbb is 4.

			@sa GetLinePosition() and GetDepth().
		*/
		SizeType GetLineNumber() const;

		/**
			@brief Gets the current line position.

			Starting value is 1. For example:
			@verbatim
			<root>
			abcdef<mytag />
			</root>
			@endverbatim
			Line position of &lt;@a mytag /&gt; is 7.

			@warning Carriage return characters (U+000D) are ignored.
			@sa GetLineNumber() and GetDepth().
		*/
		SizeType GetLinePosition() const;

		/**
			@brief Gets the depth of the current node in the XML document.

			Example:
			@verbatim
			<root>
			   <a>aaa</a>
			   <b>
				   bbb
			   </b>
			</root>
			@endverbatim
			Depth of @a aaa is 2, the same as depth of @a bbb.
			Depht of &lt;@a a&gt; tag, the same as closing tag &lt;/@a a&gt; is 1.

			@sa GetLineNumber() and GetLinePosition().
		*/
		SizeType GetDepth() const;
	};

	template <typename TFinalEncoding>
	inline Inspector<TFinalEncoding>::Inspector()
	{
	
	}

	template <typename TFinalEncoding>
	inline Inspector<TFinalEncoding>::Inspector(const char* /* filePath */)
	{
	
	}

	template <typename TFinalEncoding>
	inline Inspector<TFinalEncoding>::Inspector(const std::string& /* filePath */)
	{
	
	}

	template <typename TFinalEncoding>
	inline Inspector<TFinalEncoding>::Inspector(std::istream* /* inputStream */)
	{
	
	}

	template <typename TFinalEncoding>
	template <typename TInputIterator>
	inline Inspector<TFinalEncoding>::Inspector(
		TInputIterator /* first */, TInputIterator /* last */)
	{
	
	}

	template <typename TFinalEncoding>
	inline Inspector<TFinalEncoding>::Inspector(CharactersReader* /* reader */)
	{
	
	}

	template <typename TFinalEncoding>
	inline void Inspector<TFinalEncoding>::SaveNumbers()
	{
		SizeType lastLineNumber = currentLineNumber;
		SizeType lastLinePosition = currentLinePosition;
	}

	template <typename TFinalEncoding>
	inline bool Inspector<TFinalEncoding>::ReadNode()
	{
		// FIXME:
		return false;
	}

	template <typename TFinalEncoding>
	inline typename Inspector<TFinalEncoding>::SizeType
		Inspector<TFinalEncoding>::GetLineNumber() const
	{
		return lastLineNumber;
	}

	template <typename TFinalEncoding>
	inline typename Inspector<TFinalEncoding>::SizeType
		Inspector<TFinalEncoding>::GetLinePosition() const
	{
		return lastLinePosition;
	}

	template <typename TFinalEncoding>
	inline typename Inspector<TFinalEncoding>::SizeType
		Inspector<TFinalEncoding>::GetDepth() const
	{
		// FIXME:
		return 0;
	}
}

#endif

