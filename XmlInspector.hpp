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
#include "CharactersWriter.hpp"
#include <string>
#include <ios>
#include <streambuf>
#include <istream>
#include <fstream>
#include <cstdint>
#include <cassert>
#include <memory>

/**
	@file XmlInspector.hpp
*/

/**
	@brief Main namespace for XML solutions like XML parser and some encoding stuff.
*/
namespace Xml
{
	/**
		@brief Specifies the type of node.
	*/
	enum class NodeType
	{
		/**
			@brief This is returned by the Inspector if a ReadNode method has not been called.
		*/
		None
	};

	/**
		@brief Error code.
	*/
	enum class ErrorCode
	{
		/**
			@brief There is no error.
		*/
		None,

		/**
			@brief Stream error has occurred.
		*/
		StreamError,

		/**
			@brief Invalid byte sequence. For example
				invalid byte order mark (BOM) or alone
				surrogate halve in UTF-16.
		*/
		InvalidByteSequence
	};

	/// @cond DETAILS
	namespace Details
	{
		enum class Bom
		{
			None,
			StreamError,
			Invalid,
			Utf8,
			Utf16BE,
			Utf16LE,
			Utf32BE,
			Utf32LE
		};

		Bom ReadBom(std::istream* inputStream);

		template <typename TInputIterator>
		Bom ReadBom(TInputIterator& first, TInputIterator& last);

		template <
			typename TInputIterator,
			typename TCharacterType,
			typename TTraits = std::char_traits<TCharacterType>>
		class BasicIteratorsBuf
			: public std::basic_streambuf<TCharacterType, TTraits>
		{
		public:
			typedef TInputIterator IteratorType;
			typedef std::basic_streambuf<TCharacterType, TTraits> StreambufType;

			typedef TCharacterType char_type;
			typedef TTraits traits_type;
			typedef typename traits_type::int_type int_type;
			typedef typename traits_type::pos_type pos_type;
			typedef typename traits_type::off_type off_type;
		protected:
			IteratorType curIter;
			IteratorType endIter;

			virtual int_type underflow();

			virtual int_type uflow();

			virtual std::streamsize showmanyc();
		public:
			BasicIteratorsBuf(IteratorType first, IteratorType last)
				: StreambufType(), curIter(first), endIter(last)
			{
			
			}
		};
	}
	/// @endcond

	/**
		@brief Primary XML parser class.

		TODO: detailed description...

		@tparam TCharactersWriter Desired encoding. You don't need to care how XML file is encoded.
			You can choose how you want to read XML strings between Utf8CharactersWriter, Utf16CharactersWriter
			and Utf32CharactersWriter class from CharactersWriter.hpp file. They respectively store the strings in
			@a std::string, @a std::u16string and @a std::u32string. You can also write your own fancy way of
			storing strings. For example you may want to use @a std::wstring and even other than Unicode encoding.
	*/
	template <typename TCharactersWriter>
	class Inspector
	{
	public:
		/**
			@brief Alias of encoding type that is used to write strings.
		*/
		typedef TCharactersWriter CharactersWriterType;

		/**
			@brief String type provided by CharactersWriterType.
		*/
		typedef typename TCharactersWriter::StringType StringType;

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
		NodeType node;
		ErrorCode err;
		const char* errMsg;
		std::string fPath;
		std::ifstream fileStream;
		std::istream* inputStreamPtr;
		CharactersReader* reader;
		bool isExternalStream;
		bool isExternalReader;
		bool afterBom;

		// Last numbers <= current numbers.
		void SaveNumbers();

		void SetError(ErrorCode errorCode);

		void ParseBom();
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
			@brief Destructor.
		*/
		~Inspector();

		/**
			@brief Reads the next node from the stream.

			TODO: detailed description...

			@return True if the next node was read successfully.
				False if there are no more nodes to read.
		*/
		bool ReadNode();

		/**
			@brief Gets the type of the current node.
		*/
		NodeType GetNodeType() const;

		/**
			@brief Gets the last error message.
		*/
		const char* GetErrorMessage() const;

		/**
			@brief Gets the last error code.
		*/
		ErrorCode GetErrorCode() const;

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

		/**
			@brief Removes the association with the source and clears
				the state of Inspector object.
		*/
		void Reset();

		/**
			@brief Clears the state of Inspector object and assign
				the source to the specified file path.
		*/
		void Reset(const char* filePath);

		/**
			@brief Clears the state of Inspector object and assign
				the source to the specified file path.
		*/
		void Reset(const std::string& filePath);

		/**
			@brief Clears the state of Inspector object and assign
				the source to the specified stream.

			TODO: warning - stream from this pointer must exist while reading nodes.
		*/
		void Reset(std::istream* inputStream);

		/**
			@brief Clears the state of Inspector object and assign
				the source to the specified iterators.

			@param[in] first,last Input iterators to the initial
				and final positions in a sequence. The range used
				is [first,last), which contains all the elements
				between first and last, including the element pointed
				by first but not the element pointed by last.
		*/
		template <typename TInputIterator>
		void Reset(TInputIterator first, TInputIterator last);

		/**
			@brief Clears the state of Inspector object and assign
				the source to the specified characters reader interface.

			TODO: warning message about BOM.
		*/
		void Reset(CharactersReader* reader);
	};

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector()
		: lastLineNumber(0),
		lastLinePosition(0),
		currentLineNumber(0),
		currentLinePosition(0),
		node(NodeType::None),
		err(ErrorCode::None),
		errMsg(nullptr),
		fPath(),
		fileStream(),
		inputStreamPtr(nullptr),
		reader(nullptr),
		isExternalStream(false),
		isExternalReader(false),
		afterBom(false)
	{

	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector(const char* filePath)
		: Inspector<TCharactersWriter>()
	{
		Reset(filePath);
	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector(const std::string& filePath)
		: Inspector<TCharactersWriter>()
	{
		Reset(filePath);
	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector(std::istream* inputStream)
		: Inspector<TCharactersWriter>()
	{
		Reset(inputStream);
	}

	template <typename TCharactersWriter>
	template <typename TInputIterator>
	inline Inspector<TCharactersWriter>::Inspector(
		TInputIterator first, TInputIterator last)
			: Inspector<TCharactersWriter>()
	{
		Reset(first, last);
	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector(CharactersReader* reader)
		: Inspector<TCharactersWriter>()
	{
		Reset(reader);
	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::~Inspector()
	{
		Reset();
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::SaveNumbers()
	{
		SizeType lastLineNumber = currentLineNumber;
		SizeType lastLinePosition = currentLinePosition;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::SetError(ErrorCode errorCode)
	{
		err = errorCode;
		if (errorCode != ErrorCode::None)
		{
			switch (errorCode)
			{
				case ErrorCode::StreamError:
					errMsg = "Stream error has occurred.";
					return;
				case ErrorCode::InvalidByteSequence:
					errMsg = "Invalid byte sequence. For example "
						"invalid byte order mark (BOM) "
						"or alone surrogate halve in UTF-16.";
					return;
				default:
					errMsg = nullptr;
					return;
			}
		}
		errMsg = nullptr;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::ParseBom()
	{
		// TODO:
		if (!fPath.empty())
		{
			fileStream.open(fPath.c_str());
			if (!fileStream.is_open())
			{
				fileStream.clear();
				SetError(ErrorCode::StreamError);
				return;
			}

			Details::Bom bom = Details::ReadBom(&fileStream);
			if (bom == Details::Bom::None)
			{
				try
				{
					reader = new Utf8StreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				return;
			}

			if (bom == Details::Bom::StreamError)
			{
				fileStream.close();
				fileStream.clear();
				SetError(ErrorCode::StreamError);
				return;
			}

			if (bom == Details::Bom::Invalid)
			{
				fileStream.close();
				fileStream.clear();
				SetError(ErrorCode::InvalidByteSequence);
				return;
			}

			// We have a BOM.
			// TODO:
			assert(false && "Not implemented yet.");
		}
		else if (inputStreamPtr != nullptr)
		{
			Details::Bom bom = Details::ReadBom(inputStreamPtr);
			if (bom == Details::Bom::None)
			{
				reader = new Utf8StreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				return;
			}

			if (bom == Details::Bom::StreamError)
			{
				SetError(ErrorCode::StreamError);
				return;
			}

			if (bom == Details::Bom::Invalid)
			{
				SetError(ErrorCode::InvalidByteSequence);
				return;
			}

			// We have a BOM.
			// TODO:
			assert(false && "Not implemented yet.");
		}
		else if (isExternalReader)
		{
			err = ErrorCode::None;
			afterBom = true;
		}
		else
		{
			SetError(ErrorCode::StreamError);
		}
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ReadNode()
	{
		if (!afterBom)
			ParseBom();

		if (err != ErrorCode::None)
			return false;

		// TODO:
		assert(false && "Not implemented yet.");
		return true;
	}

	template <typename TCharactersWriter>
	inline NodeType Inspector<TCharactersWriter>::GetNodeType() const
	{
		return node;
	}

	template <typename TCharactersWriter>
	inline const char* Inspector<TCharactersWriter>::GetErrorMessage() const
	{
		return errMsg;
	}

	template <typename TCharactersWriter>
	inline ErrorCode Inspector<TCharactersWriter>::GetErrorCode() const
	{
		return err;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetLineNumber() const
	{
		return lastLineNumber;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetLinePosition() const
	{
		return lastLinePosition;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetDepth() const
	{
		// TODO:
		return 0;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset()
	{
		lastLineNumber = 0;
		lastLinePosition = 0;
		currentLineNumber = 0;
		currentLinePosition = 0;
		node = NodeType::None;
		err = ErrorCode::None;
		errMsg = nullptr;
		afterBom = false;
		if (!fPath.empty())
		{
			fPath.clear();
			if (fileStream.is_open())
				fileStream.close();
			fileStream.clear();
			delete reader;
			reader = nullptr;
		}
		else if (isExternalReader)
		{
			reader = nullptr;
			isExternalReader = false;
		}
		else if (isExternalStream)
		{
			inputStreamPtr = nullptr;
			isExternalStream = false;
			delete reader;
			reader = nullptr;
		}
		else
		{
			if (inputStreamPtr != nullptr)
			{
				delete inputStreamPtr->rdbuf();
				delete inputStreamPtr;
				inputStreamPtr = nullptr;
			}
			delete reader;
			reader = nullptr;
		}
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(const char* filePath)
	{
		Reset();
		fPath = filePath;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(const std::string& filePath)
	{
		Reset();
		fPath = filePath;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(std::istream* inputStream)
	{
		Reset();
		inputStreamPtr = inputStream;
		isExternalStream = true;
	}

	template <typename TCharactersWriter>
	template <typename TInputIterator>
	inline void Inspector<TCharactersWriter>::Reset(
		TInputIterator first, TInputIterator last)
	{
		Reset();
		std::unique_ptr<Details::BasicIteratorsBuf<TInputIterator, char>> buf(
			new Details::BasicIteratorsBuf<TInputIterator, char>(first, last));
		inputStreamPtr = new std::istream(buf.get());
		buf.release();
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(CharactersReader* r)
	{
		Reset();
		reader = r;
		isExternalReader = (r != nullptr);
	}

	/// @cond DETAILS
	namespace Details
	{
		inline Bom ReadBom(std::istream* /* inputStream */)
		{
			// TODO:
			return Bom::None;
		}
		
		template <typename TInputIterator>
		inline Bom ReadBom(TInputIterator& /* first */, TInputIterator& /* last */)
		{
			// TODO:
			return Bom::None;
		}

		template <
			typename TInputIterator,
			typename TCharacterType,
			typename TTraits>
		inline typename BasicIteratorsBuf<TInputIterator, TCharacterType, TTraits>::int_type
			BasicIteratorsBuf<TInputIterator, TCharacterType, TTraits>::underflow()
		{
			if (curIter == endIter)
				return traits_type::eof();

			return traits_type::to_int_type(*curIter);
		}

		template <
			typename TInputIterator,
			typename TCharacterType,
			typename TTraits>
		inline typename BasicIteratorsBuf<TInputIterator, TCharacterType, TTraits>::int_type
			BasicIteratorsBuf<TInputIterator, TCharacterType, TTraits>::uflow()
		{
			if (curIter == endIter)
				return traits_type::eof();

			return traits_type::to_int_type(*curIter++);
		}

		template <
			typename TInputIterator,
			typename TCharacterType,
			typename TTraits>
		inline std::streamsize
			BasicIteratorsBuf<TInputIterator, TCharacterType, TTraits>::showmanyc()
		{
			return (curIter != endIter)
					? 1
					: 0;
		}
	}
	/// @endcond
}

#endif

