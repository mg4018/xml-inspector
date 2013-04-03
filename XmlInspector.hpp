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
		None,

		/**
			@brief A start element tag (for example <tt>&lt;mytag&gt;</tt> ).
		*/
		StartElement,

		/**
			@brief An end element tag (for example <tt>&lt;/mytag&gt;</tt> ).
		*/
		EndElement,

		/**
			@brief En empty element (for example <tt>&lt;mytag /&gt;</tt> ).
		*/
		EmptyElement,

		/**
			@brief A text content of a node.
		*/
		Text,

		/**
			@brief A CDATA section (for example <tt>&lt;![CDATA[don't touch]]&gt;</tt> ).
		*/
		CDATA,

		/**
			@brief The XML declaration (for example <tt>&lt;?xml version='1.0'?&gt;</tt> ).
		*/
		XmlDeclaration,

		/**
			@brief A comment (for example <tt>&lt;!-- my comment --&gt;</tt> ).
		*/
		Comment,

		/**
			@brief A processing instruction (for example <tt>&lt;?php echo "Hello, world!"; ?&gt;</tt> ).
		*/
		ProcessingInstruction,

		/**
			@brief A reference to an entity (for example <tt>&amp;myref;</tt> ).
		*/
		EntityReference,

		/**
			@brief A document type declaration (for example <tt>&lt;!DOCTYPE...&gt;</tt> ).
		*/
		DocumentType,

		/**
			@brief White space between markup. 
		*/
		Whitespace
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
		InvalidByteSequence,

		/**
			@brief Not allowed characters. For example some characters
				outside the root element, where a white spaces are the
				only characters allowed.
		*/
		InvalidSyntax,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-XMLDecl.
		*/
		InvalidXmlDeclarationLocation,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-XMLDecl.
		*/
		InvalidXmlDeclarationSyntax,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-Comment.
		*/
		InvalidCommentSyntax,

		/**
			@brief CDATA section is outside the root element.
				Check http://www.w3.org/TR/REC-xml/#NT-CDSect.
		*/
		CDataSectionOutside,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-doctypedecl.
		*/
		InvalidDoctypeDeclarationLocation,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-doctypedecl.
		*/
		DoubleDoctypeDeclaration,

		/**
			@brief Check http://www.w3.org/TR/REC-xml/#NT-PI.
		*/
		InvalidProcessingInstructionSyntax,

		/**
			@brief Check http://www.w3.org/TR/xml-names/#NT-QName.
		*/
		InvalidTagName,

		/**
			@brief Check http://www.w3.org/TR/xml-names/#NT-QName.
		*/
		InvalidAttributeName,

		/**
			@brief We should get '=' character now, but we have got other, not allowed character.
		*/
		EqualsSignExpected,

		/**
			@brief For example it's not allowed in XML: <tt>&lt;tagName attrName=value&gt;</tt>,
				but this is OK: <tt>&lt;tagName attrName="value"&gt;</tt>.
		*/
		QuotationMarkExpected,

		/**
			@brief For example: <tt>&lt;tagname attr[end of document]</tt>.
		*/
		UnclosedToken,

		/**
			@brief Invalid syntax of Reference.
				Check http://www.w3.org/TR/REC-xml/#NT-Reference.
		*/
		InvalidReferenceSyntax,

		/**
			@brief Undeclared entity.
				Check http://www.w3.org/TR/REC-xml/#NT-EntityRef.
		*/
		UndeclaredEntity,

		/**
			@brief Code point in character reference doesn't match
				the valid character in ISO/IEC 10646 character set.
				Check http://www.w3.org/TR/REC-xml/#NT-CharRef.
		*/
		InvalidCharacterReference,

		/**
			@brief We should get the greater than sign now, but we have got other,
				not allowed character.
		*/
		GreaterThanSignExpected,

		/**
			@brief For example: <tt>&lt;a&gt;text&lt;/b&gt;</tt>. <tt>&lt;/a&gt;</tt> expected, but <tt>&lt;/b&gt;</tt> found.
				Another example: <tt>&lt;/b&gt;</tt>. Found closing tag, but there is no start tag of @c b.
				Both examples are not allowed in XML files.
		*/
		UnexpectedEndTag,

		/**
			@brief For example: <tt>&lt;a&gt;&lt;b&gt;&lt;c&gt;&lt;/c&gt;&lt;/b&gt;</tt>. Unclosed @c a tag.
		*/
		UnclosedTag,

		/**
			@brief There is no root element in document.
		*/
		NoElement,

		/**
			@brief More than one attribute name in the same start-tag or empty-element tag.
		*/
		DoubleAttributeName,

		/**
			@brief There is some name prefix which is not bound to any namespace URI.
		*/
		PrefixWithoutAssignedNamespace,

		/**
			@brief Namespace declaration with prefix cannot have empty value.
		*/
		PrefixWithEmptyNamespace,

		/**
			@brief Reserved xmlns prefix cannot be declared or set to empty value.
		*/
		XmlnsDeclared,

		/**
			@brief Prefix is bound to reserved namespace.
		*/
		PrefixBoundToReservedNamespace,

		/**
			@brief Prefix 'xml' is reserved for use by XML and has a fixed
				namespace URI http://www.w3.org/XML/1998/namespace.
		*/
		InvalidXmlPrefixDeclaration
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
			@c std::string, @c std::u16string and @c std::u32string. You can also write your own fancy way of
			storing strings. For example you may want to use @c std::wstring and even other than Unicode encoding.
	*/
	template <typename TCharactersWriter>
	class Inspector
	{
	public:
		/**
			@brief Alias of characters writer type that is used to write strings.
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
		SizeType lineNumber;
		SizeType linePosition;
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
		StringType name;
		StringType value;
		StringType localName;
		StringType prefix;
		StringType namespaceUri;
		char32_t currentCharacter;

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
			@brief Gets the qualified name of the node.
		*/
		const StringType& GetName() const;

		/**
			@brief Gets the value of the node.
		*/
		const StringType& GetValue() const;

		/**
			@brief Gets the local name of the node.
		*/
		const StringType& GetLocalName() const;

		/**
			@brief Gets the namespace prefix of the node.
		*/
		const StringType& GetPrefix() const;

		/**
			@brief Gets the namespace URI of the node.
		*/
		const StringType& GetNamespaceUri() const;

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
			Line number of @c bbb is 4.

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
			Line position of <tt>&lt;@c mytag /&gt;</tt> is 7.

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
			Depth of @c aaa is 2, the same as depth of @c bbb.
			Depht of &lt;@c a&gt; tag, the same as closing tag &lt;/@c a&gt; is 1.

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
		: lineNumber(0),
		linePosition(0),
		node(NodeType::None),
		err(ErrorCode::None),
		errMsg(nullptr),
		fPath(),
		fileStream(),
		inputStreamPtr(nullptr),
		reader(nullptr),
		isExternalStream(false),
		isExternalReader(false),
		afterBom(false),
		name(),
		value(),
		localName(),
		prefix(),
		namespaceUri(),
		currentCharacter(0)
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
					errMsg = "Invalid byte sequence.";
					return;
				case ErrorCode::InvalidSyntax:
					errMsg = "Invalid syntax.";
					return;
				case ErrorCode::InvalidXmlDeclarationLocation:
					errMsg = "Invalid location of XML declaration.";
					return;
				case ErrorCode::InvalidXmlDeclarationSyntax:
					errMsg = "Invalid syntax of XML declaration.";
					return;
				case ErrorCode::InvalidCommentSyntax:
					errMsg = "Invalid syntax of comment.";
					return;
				case ErrorCode::CDataSectionOutside:
					errMsg = "CDATA section is outside the root element.";
					return;
				case ErrorCode::InvalidDoctypeDeclarationLocation:
					errMsg = "Invalid location of document type declaration.";
					return;
				case ErrorCode::DoubleDoctypeDeclaration:
					errMsg = "There should be exactly one document type declaration.";
					return;
				case ErrorCode::InvalidProcessingInstructionSyntax:
					errMsg = "Invalid syntax of processing instruction.";
					return;
				case ErrorCode::InvalidTagName:
					errMsg = "Invalid tag name.";
					return;
				case ErrorCode::InvalidAttributeName:
					errMsg = "Invalid attribute name.";
					return;
				case ErrorCode::EqualsSignExpected:
					errMsg = "Equals sign expected.";
					return;
				case ErrorCode::QuotationMarkExpected:
					errMsg = "Quotation mark expected.";
					return;
				case ErrorCode::UnclosedToken:
					errMsg = "Unclosed token.";
					return;
				case ErrorCode::InvalidReferenceSyntax:
					errMsg = "Invalid syntax of reference.";
					return;
				case ErrorCode::UndeclaredEntity:
					errMsg = "Undeclared entity.";
					return;
				case ErrorCode::InvalidCharacterReference:
					errMsg = "Code point in character reference doesn\'t match "
						"the valid character in ISO/IEC 10646 character set.";
					return;
				case ErrorCode::GreaterThanSignExpected:
					errMsg = "Greater than sign expected.";
					return;
				case ErrorCode::UnexpectedEndTag:
					errMsg = "Unexpected end tag.";
					return;
				case ErrorCode::UnclosedTag:
					errMsg = "Unclosed tag.";
					return;
				case ErrorCode::NoElement:
					errMsg = "Cannot find an element.";
					return;
				case ErrorCode::DoubleAttributeName:
					errMsg = "An attribute name must not appear more than "
						"once in the same start-tag or empty-element tag.";
					return;
				case ErrorCode::PrefixWithoutAssignedNamespace:
					errMsg = "Name prefix must bound to the namespace URI.";
					return;
				case ErrorCode::PrefixWithEmptyNamespace:
					errMsg = "Namespace declaration with prefix cannot have empty value.";
					return;
				case ErrorCode::XmlnsDeclared:
					errMsg = "Reserved xmlns prefix cannot be declared or set to empty value.";
					return;
				case ErrorCode::PrefixBoundToReservedNamespace:
					errMsg = "Prefix is bound to reserved namespace.";
					return;
				case ErrorCode::InvalidXmlPrefixDeclaration:
					errMsg = "Prefix \'xml\' is reserved for use by XML and has a fixed "
						"namespace URI http://www.w3.org/XML/1998/namespace.";
					return;
				default:
					errMsg = "XML error has occurred.";
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
	inline const typename Inspector<TCharactersWriter>::StringType&
		Inspector<TCharactersWriter>::GetName() const
	{
		return name;
	}

	template <typename TCharactersWriter>
	inline const typename Inspector<TCharactersWriter>::StringType&
		Inspector<TCharactersWriter>::GetValue() const
	{
		return value;
	}

	template <typename TCharactersWriter>
	inline const typename Inspector<TCharactersWriter>::StringType&
		Inspector<TCharactersWriter>::GetLocalName() const
	{
		return localName;
	}

	template <typename TCharactersWriter>
	inline const typename Inspector<TCharactersWriter>::StringType&
		Inspector<TCharactersWriter>::GetPrefix() const
	{
		return prefix;
	}

	template <typename TCharactersWriter>
	inline const typename Inspector<TCharactersWriter>::StringType&
		Inspector<TCharactersWriter>::GetNamespaceUri() const
	{
		return namespaceUri;
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
		return lineNumber;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetLinePosition() const
	{
		return linePosition;
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
		lineNumber = 0;
		linePosition = 0;
		node = NodeType::None;
		err = ErrorCode::None;
		errMsg = nullptr;
		afterBom = false;
		name.clear();
		value.clear();
		localName.clear();
		prefix.clear();
		namespaceUri.clear();
		currentCharacter = 0;
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

