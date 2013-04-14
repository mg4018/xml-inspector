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
#include <deque>

/**
	@file XmlInspector.hpp
*/

/**
	@brief Main namespace for XML solutions.
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
			@brief Reserved namespace cannot be declared as a default namespace.
		*/
		ReservedNamespaceAsDefault,

		/**
			@brief Prefix 'xml' is reserved for use by XML and has a fixed
				namespace URI http://www.w3.org/XML/1998/namespace.
		*/
		InvalidXmlPrefixDeclaration
	};

	/**
		@brief Delimiter for attribute value.
	*/
	enum class ValueDelimiter
	{
		/**
			@brief Attribute is delimited by apostrophes (for example <tt>&lt;a name='value'&gt;</tt> ).
		*/
		Apostrophe,

		/**
			@brief Attribute is delimited by double quotes (for example <tt>&lt;a name="value"&gt;</tt> ).
		*/
		DoubleQuote
	};

	/**
		@brief Class for storing attribute data like name and value.
	*/
	template <typename TStringType>
	class Attribute
	{
	public:
		/**
			@brief Alias to string type provided by class template parameter.
		*/
		typedef TStringType StringType;

		/**
			@brief Unsigned integer type definition for determining location in XML document.
				This type should be enough to store any file size or memory buffer size.
		*/
		typedef uint_least64_t SizeType;

		/**
			@brief Qualified name of the attribute.
		*/
		StringType Name;

		/**
			@brief Value of the attribute.
		*/
		StringType Value;

		/**
			@brief Local name of the attribute.
		*/
		StringType LocalName;

		/**
			@brief Namespace prefix of the attribute.
		*/
		StringType Prefix;

		/**
			@brief Namespace URI of the attribute.
		*/
		StringType NamespaceUri;

		/**
			@brief Row number of attribute name.

			Starting value is 1. For example:
			@verbatim
			<root>
			   <a
			       attrName=
				       "value"
			   />
			</root>
			@endverbatim
			Row number of @c attrName is 3.

			@sa Column.
		*/
		SizeType Row;

		/**
			@brief Column number of attribute name.

			Starting value is 1. For example:
			@verbatim
			<root attrName="value"/>
			@endverbatim
			Column number of @c attrName is 7.

			@warning Carriage return characters (U+000D) are ignored.
			@sa Row.
		*/
		SizeType Column;

		/**
			@brief Delimiter for attribute value.
		*/
		ValueDelimiter Delimiter;
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

		template <typename TStringType>
		class NamespaceDeclaration
		{
		public:	
			typedef TStringType StringType;
			typedef uint_least64_t SizeType;

			StringType Prefix;
			StringType Uri;
			SizeType TagIndex; // Counting from 0.
		};

		template <typename TStringType>
		class UnclosedTag
		{
		public:	
			typedef TStringType StringType;
			typedef uint_least64_t SizeType;

			StringType Name;
			StringType LocalName;
			StringType Prefix;
			StringType NamespaceUri;
			SizeType Row;
			SizeType Column;
		};
	}
	/// @endcond

	/**
		@brief Primary XML parser class.

		Example:
		@code{.cpp}
        #include "XmlInspector.hpp"
        #include <iostream>
        #include <cstdlib>
        #include <string>

        int main()
        {
            Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
                "test.xml");

            while (inspector.ReadNode())
            {
                switch (inspector.GetNodeType())
                {
                    case Xml::NodeType::StartElement:
                        std::cout << "[StartElement] name(" << inspector.GetName() <<
                            "), value(" << inspector.GetValue() << ").\n";
                        break;
                    case Xml::NodeType::EndElement:
                        std::cout << "[EndElement] name(" << inspector.GetName() <<
                            "), value(" << inspector.GetValue() << ").\n";
                        break;
                    case Xml::NodeType::Text:
                        std::cout << "[Text] value(" << inspector.GetValue() << ").";
                        break;
                    case Xml::NodeType::Whitespace:
                        // Ignore white spaces between markup.
                        break;
                    default:
                        std::cout << "[...] name(" << inspector.GetName() <<
                            "), value(" << inspector.GetValue() << ").\n";
                        break;
                }
            }

            if (inspector.GetErrorCode() != Xml::ErrorCode::None)
                std::cout << "Error: " << inspector.GetErrorMessage() << "\n";

            return EXIT_SUCCESS;
        }
		@endcode

		@tparam TCharactersWriter Writer with specified encoding. You don't need to care how XML file is encoded.
			You can choose how you want to store the strings between Utf8CharactersWriter, Utf16CharactersWriter
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
			@brief Attribute type.
		*/
		typedef Attribute<StringType> AttributeType;

		/**
			@brief Attribute iterator.
		*/
		typedef typename std::deque<AttributeType>::const_iterator AttributeIterator;

		/**
			@brief Unsigned integer type definition for determining location in XML document.
				This type should be enough to store any file size or memory buffer size.
		*/
		typedef uint_least64_t SizeType;
	private:
		typedef typename StringType::size_type StringSizeType;
		typedef Details::UnclosedTag<StringType> UnclosedTagType;
		typedef Details::NamespaceDeclaration<StringType> NamespaceDeclarationType;
		typedef typename std::deque<AttributeType>::size_type AttributesSizeType;
		typedef typename std::deque<UnclosedTagType>::size_type UnclosedTagsSizeType;
		typedef typename std::deque<NamespaceDeclarationType>::size_type NamespacesSizeType;

		static const char32_t Space = 0x20;                  // ' '
		static const char32_t LineFeed = 0x0A;               // '\n'
		static const char32_t CarriageReturn = 0x0D;         // '\r'
		static const char32_t LessThan = 0x3C;               // '<'
		static const char32_t GreaterThan = 0x3E;            // '>'
		static const char32_t Equals = 0x3D;                 // '='
		static const char32_t Apostrophe = 0x27;             // '\''
		static const char32_t DoubleQuote = 0x22;            // '\"'
		static const char32_t Slash = 0x2F;                  // '/'
		static const char32_t Question = 0x3F;               // '?'
		static const char32_t Exclamation = 0x21;            // '!'
		static const char32_t Minus = 0x2D;                  // '-'
		static const char32_t Ampersand = 0x26;              // '&'
		static const char32_t Hash = 0x23;                   // '#'
		static const char32_t X = 0x78;                      // 'x'
		static const char32_t Colon = 0x3A;                  // ':'
		static const char32_t Semicolon = 0x3B;              // ';'
		static const char32_t LeftSquareBracket = 0x5B;      // '['
		static const char32_t RightSquareBracket = 0x5D;     // ']'
		static const char32_t Dot = 0x2E;                    // '.'
		static constexpr const char32_t* LowerXml = U"xml";
		static constexpr const char32_t* UpperXml = U"XML";
		static constexpr const char32_t* Xmlns = U"xmlns";
		static constexpr const char32_t* XmlUri = U"http://www.w3.org/XML/1998/namespace";
		static constexpr const char32_t* XmlnsUri = U"http://www.w3.org/2000/xmlns/";
		static constexpr const char32_t* XmlDeclarationVersion = U"version";
		static constexpr const char32_t* XmlDeclarationEncoding = U"encoding";
		static constexpr const char32_t* XmlDeclarationStandalone = U"standalone";
		static constexpr const char32_t* Yes = U"yes";
		static constexpr const char32_t* No = U"no";
		static constexpr const char32_t* CDATA = U"CDATA";
		static constexpr const char32_t* DOCTYPE = U"DOCTYPE";
		// Source types.
		static const int SourceNone = 0; // Inspector() constructor.
		static const int SourcePath = 1; // Inspector(const char*) or Inspector(const std::string&) constructor.
		static const int SourceStream = 2; // Inspector(std::istream*) constructor.
		static const int SourceIterators = 3; // Inspector(InputIterator first, InputIterator last) constructor.
		static const int SourceReader = 4; // Inspector(Encoding::CharactersReader*) constructor.
		
		static const StringSizeType NameReserve = 31;
		static const StringSizeType ValueReserve = 63;
		static const StringSizeType LocalNameReserve = 15;
		static const StringSizeType PrefixReserve = 15;
		static const StringSizeType NamespaceUriReserve = 63;

		SizeType row;
		SizeType column;
		SizeType currentRow;
		SizeType currentColumn;
		NodeType node;
		ErrorCode err;
		const char* errMsg;
		std::string fPath;
		std::ifstream fileStream;
		std::istream* inputStreamPtr;
		Encoding::CharactersReader* reader;
		int sourceType;
		bool afterBom;
		Details::Bom bom;
		StringType name;
		StringType value;
		StringType localName;
		StringType prefix;
		StringType namespaceUri;
		char32_t currentCharacter;
		char32_t bufferedCharacter;
		bool foundElement;
		bool eof;
		StringType lowerXmlString;
		StringType xmlnsString;
		StringType xmlUriString;
		StringType xmlnsUriString;
		// Instead of removing objects from collection I decrement size.
		// It's a fake size, but I don't want to
		// allocate strings in objects after each element node and each XML document.
		// To clear these collections you can call Inspector::Clear method.
		std::deque<AttributeType> attributes;
		AttributesSizeType attributesSize;
		std::deque<UnclosedTagType> unclosedTags;
		UnclosedTagsSizeType unclosedTagsSize;
		std::deque<NamespaceDeclarationType> namespaces;
		NamespacesSizeType namespacesSize;

		// We don't need to check carriage return
		// while NextCharBad method removes them for us.
		// That's why I use this method instead of
		// Xml::Encoding::CharactersReader::IsWhiteSpace.
		static bool IsWhiteSpace(char32_t codePoint);

		void SetError(ErrorCode errorCode);

		void SavePosition();

		// Extracts the next character and sets the error flag
		// if eof (only if insideTag flag), invalid character or stream error.
		// Returns true if error or eof (insideTag == false) happened.
		bool NextCharBad(bool insideTag);

		void ParseBom();

		bool ParseElement();

		bool ParseEndElement();

		bool ParseText();

		bool ParseQuestion();

		bool ParseExclamation();

		void PrepareNode();

		bool NamespacesStuff();

		AttributeType& NewAttribute();

		UnclosedTagType& NewUnclosedTag();

		NamespaceDeclarationType& NewNamespace();
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
		Inspector(Encoding::CharactersReader* reader);

		/**
			@brief Copy constructor is inaccessible for this class.
		*/
		Inspector(const Inspector&) = delete;

		/**
			@brief Assignment operator is inaccessible for this class.
		*/
		Inspector& operator=(Inspector&) = delete;

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
			@brief Gets a value indicating whether the current node has any attributes.
		*/
		bool HasAttributes() const;

		/**
			@brief Gets the number of attributes on the current node.
		*/
		SizeType GetAttributesCount() const;

		/**
			@brief Returns a const iterator referring to the first
				attribute on the current node.
		*/
		AttributeIterator GetAttributeBegin() const;

		/**
			@brief Returns a const iterator referring to the <em>past-the-end</em>
				attribute on the current node.
		*/
		AttributeIterator GetAttributeEnd() const;

		/**
			@brief Gets the last error message.
		*/
		const char* GetErrorMessage() const;

		/**
			@brief Gets the last error code.
		*/
		ErrorCode GetErrorCode() const;

		/**
			@brief Gets the current row number.

			Starting value is 1. For example:
			@verbatim
			<root>
			   <a>aaa</a>
			   <b>
				   bbb
			   </b>
			</root>
			@endverbatim
			Row number of @c bbb is 4.

			@sa GetColumn() and GetDepth().
		*/
		SizeType GetRow() const;

		/**
			@brief Gets the current column number.

			Starting value is 1. For example:
			@verbatim
			<root>
			abcdef<mytag />
			</root>
			@endverbatim
			Column number of <tt>&lt;mytag /&gt;</tt> is 7.

			@warning Carriage return characters (U+000D) are ignored.
			@sa GetRow() and GetDepth().
		*/
		SizeType GetColumn() const;

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

			@sa GetRow() and GetColumn().
		*/
		SizeType GetDepth() const;

		/**
			@brief Removes the association with the source and resets
				the state of Inspector object.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you should call the Clear method instead.

			@sa Clear().
		*/
		void Reset();

		/**
			@brief Resets the state of Inspector object and assign
				the source to the specified file path.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you can call the Clear method.

			@sa Clear().
		*/
		void Reset(const char* filePath);

		/**
			@brief Resets the state of Inspector object and assign
				the source to the specified file path.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you can call the Clear method.

			@sa Clear().
		*/
		void Reset(const std::string& filePath);

		/**
			@brief Resets the state of Inspector object and assign
				the source to the specified stream.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you can call the Clear method.
			TODO: warning - stream from this pointer must exist while reading nodes.

			@sa Clear().
		*/
		void Reset(std::istream* inputStream);

		/**
			@brief Resets the state of Inspector object and assign
				the source to the specified iterators.

			@param[in] first,last Input iterators to the initial
				and final positions in a sequence. The range used
				is [first,last), which contains all the elements
				between first and last, including the element pointed
				by first but not the element pointed by last.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you can call the Clear method.

			@sa Clear().
		*/
		template <typename TInputIterator>
		void Reset(TInputIterator first, TInputIterator last);

		/**
			@brief Resets the state of Inspector object and assign
				the source to the specified characters reader interface.

			It doesn't clear the helpful containers to reduce the number of
			string allocations in future reading nodes. To completely
			clear those containers you can call the Clear method.
			TODO: warning message about BOM.

			@sa Clear().
		*/
		void Reset(Encoding::CharactersReader* reader);

		/**
			@brief Removes the association with the source, resets
				the state of Inspector object and clears internal containers.

			This method clears all the helpful containers in addition to
			just reset the state. If you want to use Inspector object
			to parse more XML documents you should consider Reset methods
			instead. It would reduce the number of string allocations.

			@sa Reset().
		*/
		void Clear();
	};

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::IsWhiteSpace(char32_t codePoint)
	{
		return (codePoint == 0x20 || codePoint == 0x0A || codePoint == 0x09);
	}

	template <typename TCharactersWriter>
	inline Inspector<TCharactersWriter>::Inspector()
		: row(0),
		column(0),
		currentRow(0),
		currentColumn(0),
		node(NodeType::None),
		err(ErrorCode::None),
		errMsg(nullptr),
		fPath(),
		fileStream(),
		inputStreamPtr(nullptr),
		reader(nullptr),
		sourceType(SourceNone),
		afterBom(false),
		bom(Details::Bom::None),
		name(),
		value(),
		localName(),
		prefix(),
		namespaceUri(),
		currentCharacter(0),
		bufferedCharacter(0),
		foundElement(false),
		eof(false),
		lowerXmlString(),
		xmlnsString(),
		xmlUriString(),
		xmlnsUriString(),
		attributes(),
		attributesSize(0),
		unclosedTags(),
		unclosedTagsSize(0),
		namespaces(),
		namespacesSize(0)
	{
		name.reserve(NameReserve);
		value.reserve(ValueReserve);
		localName.reserve(LocalNameReserve);
		prefix.reserve(PrefixReserve);
		namespaceUri.reserve(NamespaceUriReserve);

		lowerXmlString.reserve(3);
		xmlnsString.reserve(5);
		xmlUriString.reserve(36);
		xmlnsUriString.reserve(29);

		for (std::size_t i = 0; i < 3; ++i)
			CharactersWriterType::WriteCharacter(lowerXmlString, LowerXml[i]);

		for (std::size_t i = 0; i < 5; ++i)
			CharactersWriterType::WriteCharacter(xmlnsString, Xmlns[i]);

		for (std::size_t i = 0; i < 36; ++i)
			CharactersWriterType::WriteCharacter(xmlUriString, XmlUri[i]);

		for (std::size_t i = 0; i < 29; ++i)
			CharactersWriterType::WriteCharacter(xmlnsUriString, XmlnsUri[i]);
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
	inline Inspector<TCharactersWriter>::Inspector(Encoding::CharactersReader* reader)
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
				case ErrorCode::ReservedNamespaceAsDefault:
					errMsg = "Reserved namespace cannot be declared as a default namespace.";
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
		if (sourceType == SourcePath)
		{
			fileStream.open(fPath.c_str());
			if (!fileStream.is_open())
			{
				fileStream.clear();
				SetError(ErrorCode::StreamError);
				return;
			}

			Details::Bom tempBom = Details::ReadBom(&fileStream);
			if (tempBom == Details::Bom::None || tempBom == Details::Bom::Utf8)
			{
				try
				{
					reader = new Encoding::Utf8StreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
				eof = ((fileStream.rdstate() & std::istream::eofbit) != 0);
				return;
			}

			if (tempBom == Details::Bom::StreamError)
			{
				fileStream.close();
				fileStream.clear();
				SetError(ErrorCode::StreamError);
				return;
			}

			if (tempBom == Details::Bom::Invalid)
			{
				fileStream.close();
				fileStream.clear();
				SetError(ErrorCode::InvalidByteSequence);
				eof = ((fileStream.rdstate() & std::istream::eofbit) != 0);
				return;
			}

			if (tempBom == Details::Bom::Utf16BE)
			{
				try
				{
					reader = new Encoding::Utf16BEStreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf16LE)
			{
				try
				{
					reader = new Encoding::Utf16LEStreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf32BE)
			{
				try
				{
					reader = new Encoding::Utf32BEStreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf32LE)
			{
				try
				{
					reader = new Encoding::Utf32LEStreamReader(&fileStream);
				}
				catch (...)
				{
					fileStream.close();
					fileStream.clear();
					throw;
				}
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
		}
		else if (sourceType == SourceStream || sourceType == SourceIterators)
		{
			Details::Bom tempBom = Details::ReadBom(inputStreamPtr);
			if (tempBom == Details::Bom::None || tempBom == Details::Bom::Utf8)
			{
				reader = new Encoding::Utf8StreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
				eof = ((inputStreamPtr->rdstate() & std::istream::eofbit) != 0);
				return;
			}

			if (tempBom == Details::Bom::StreamError)
			{
				SetError(ErrorCode::StreamError);
				return;
			}

			if (tempBom == Details::Bom::Invalid)
			{
				SetError(ErrorCode::InvalidByteSequence);
				eof = ((inputStreamPtr->rdstate() & std::istream::eofbit) != 0);
				return;
			}

			if (tempBom == Details::Bom::Utf16BE)
			{
				reader = new Encoding::Utf16BEStreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf16LE)
			{
				reader = new Encoding::Utf16LEStreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf32BE)
			{
				reader = new Encoding::Utf32BEStreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
			else if (tempBom == Details::Bom::Utf32LE)
			{
				reader = new Encoding::Utf32LEStreamReader(inputStreamPtr);
				err = ErrorCode::None;
				afterBom = true;
				bom = tempBom;
			}
		}
		else if (sourceType == SourceReader)
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
	inline bool Inspector<TCharactersWriter>::ParseElement()
	{
		// currentCharacter == name start character.
		// and
		// currentCharacter != Colon.
		SizeType tempRow = currentRow;
		SizeType tempColumn = currentColumn;

		PrepareNode();

		// Element name.
		do
		{
			CharactersWriterType::WriteCharacter(name, currentCharacter);
			CharactersWriterType::WriteCharacter(localName, currentCharacter);

			if (NextCharBad(true))
				return false;

			if (currentCharacter == Colon)
			{
				// Prefixed name.
				prefix = name;
				localName.clear();
				CharactersWriterType::WriteCharacter(name, currentCharacter);

				if (NextCharBad(true))
					return false;

				if (currentCharacter == Colon ||
					!Encoding::CharactersReader::IsNameStartChar(currentCharacter))
				{
					Reset();
					SetError(ErrorCode::InvalidTagName);
					row = tempRow;
					column = tempColumn;
					return false;
				}

				do
				{
					CharactersWriterType::WriteCharacter(name, currentCharacter);
					CharactersWriterType::WriteCharacter(localName, currentCharacter);

					if (NextCharBad(true))
						return false;

					if (currentCharacter == Colon)
					{
						Reset();
						SetError(ErrorCode::InvalidTagName);
						row = tempRow;
						column = tempColumn;
						return false;
					}
				}
				while (Encoding::CharactersReader::IsNameChar(currentCharacter));
				break;
			}
		}
		while (Encoding::CharactersReader::IsNameChar(currentCharacter));

		if (currentCharacter == GreaterThan)
		{
			node = NodeType::StartElement;
			bool noErrors = NamespacesStuff();
			if (noErrors)
			{
				tempRow = row;
				tempColumn = column;
				UnclosedTagType& ref = NewUnclosedTag();
				ref.Name = name;
				ref.LocalName = localName;
				ref.Prefix = prefix;
				ref.NamespaceUri = namespaceUri;
				ref.Row = tempRow;
				ref.Column = tempColumn;
				foundElement = true;
				return true;
			}
			return false;
		}

		if (currentCharacter == Slash)
		{
			// <tagName/
			// TODO:
			assert(false && "Not implemented yet.");
		}

		if (IsWhiteSpace(currentCharacter))
		{
			// Ignore white spaces.
			do
			{
				if (NextCharBad(true))
					return false;
			}
			while (IsWhiteSpace(currentCharacter));

			if (currentCharacter != Colon &&
				Encoding::CharactersReader::IsNameStartChar(currentCharacter))
			{
				// Attributes.
				// TODO:
				assert(false && "Not implemented yet.");
			}

			if (currentCharacter == Slash)
			{
				// <tagName /
				// TODO:
				assert(false && "Not implemented yet.");
			}

			if (currentCharacter == GreaterThan)
			{
				// <tagName >
				node = NodeType::StartElement;
				bool noErrors = NamespacesStuff();
				if (noErrors)
				{
					tempRow = row;
					tempColumn = column;
					UnclosedTagType& ref = NewUnclosedTag();
					ref.Name = name;
					ref.LocalName = localName;
					ref.Prefix = prefix;
					ref.NamespaceUri = namespaceUri;
					ref.Row = tempRow;
					ref.Column = tempColumn;
					foundElement = true;
					return true;
				}
				return false;
			}

			if (Encoding::CharactersReader::IsNameChar(currentCharacter))
			{
				// For example <tagName 123attr="value">
				// 1 is not allowed as a first character name.
				Reset();
				SetError(ErrorCode::InvalidAttributeName);
				row = tempRow;
				column = tempColumn;
				return false;
			}

			// Invalid syntax.
			// For example <tagName !abc...
			// ! is not allowed as a part of the name.
			tempRow = currentRow;
			tempColumn = currentColumn;
			Reset();
			SetError(ErrorCode::InvalidSyntax);
			row = tempRow;
			column = tempColumn;
			return false;
		}

		// Invalid tag name.
		Reset();
		SetError(ErrorCode::InvalidTagName);
		row = tempRow;
		column = tempColumn;
		return false;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ParseEndElement()
	{
		// currentCharacter == Slash.
		if (NextCharBad(true))
			return false;

		SizeType tempRow = currentRow;
		SizeType tempColumn = currentColumn;

		if (currentCharacter == Colon)
		{
			Reset();
			SetError(ErrorCode::InvalidTagName);
			row = tempRow;
			column = tempColumn;
			return false;
		}
		else if (!Encoding::CharactersReader::IsNameStartChar(currentCharacter))
		{
			if (Encoding::CharactersReader::IsNameChar(currentCharacter))
			{
				// Not allowed as start character of the name,
				// but allowed as a part of this name.
				Reset();
				SetError(ErrorCode::InvalidTagName);
			}
			else
			{
				// Some weird character.
				Reset();
				SetError(ErrorCode::InvalidSyntax);
			}
			row = tempRow;
			column = tempColumn;
			return false;
		}

		PrepareNode();

		// End element name.
		do
		{
			CharactersWriterType::WriteCharacter(name, currentCharacter);
			CharactersWriterType::WriteCharacter(localName, currentCharacter);

			if (NextCharBad(true))
				return false;

			if (currentCharacter == Colon)
			{
				// Prefixed name.
				prefix = name;
				localName.clear();
				CharactersWriterType::WriteCharacter(name, currentCharacter);

				if (NextCharBad(true))
					return false;

				if (currentCharacter == Colon ||
					!Encoding::CharactersReader::IsNameStartChar(currentCharacter))
				{
					Reset();
					SetError(ErrorCode::InvalidTagName);
					row = tempRow;
					column = tempColumn;
					return false;
				}

				do
				{
					CharactersWriterType::WriteCharacter(name, currentCharacter);
					CharactersWriterType::WriteCharacter(localName, currentCharacter);

					if (NextCharBad(true))
						return false;

					if (currentCharacter == Colon)
					{
						Reset();
						SetError(ErrorCode::InvalidTagName);
						row = tempRow;
						column = tempColumn;
						return false;
					}
				}
				while (Encoding::CharactersReader::IsNameChar(currentCharacter));
				break;
			}
		}
		while (Encoding::CharactersReader::IsNameChar(currentCharacter));

		if (IsWhiteSpace(currentCharacter))
		{
			// Ignore white spaces.
			do
			{
				if (NextCharBad(true))
					return false;
			}
			while (IsWhiteSpace(currentCharacter));

			if (currentCharacter != GreaterThan)
			{
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				SetError(ErrorCode::InvalidSyntax);
				row = tempRow;
				column = tempColumn;
				return false;
			}
		}
		else if (currentCharacter != GreaterThan)
		{
			Reset();
			SetError(ErrorCode::InvalidTagName);
			row = tempRow;
			column = tempColumn;
			return false;
		}

		if (unclosedTagsSize == 0 ||
			unclosedTags[unclosedTagsSize - 1].Name != name)
		{
			tempRow = row;
			tempColumn = column;
			Reset();
			SetError(ErrorCode::UnexpectedEndTag);
			row = tempRow;
			column = tempColumn;
			return false;
		}
		namespaceUri = unclosedTags[unclosedTagsSize - 1].NamespaceUri;

		// Tag is closed.
		--unclosedTagsSize;

		// Namespaces associated with this tag are no longer needed.
		SizeType indicesToRemove = static_cast<SizeType>(unclosedTagsSize);
		NamespacesSizeType newNamespacesSize = 0;
		while (newNamespacesSize < namespacesSize)
		{
			if (namespaces[newNamespacesSize].TagIndex == indicesToRemove)
				break;
			++newNamespacesSize;
		}
		namespacesSize = newNamespacesSize;
		node = NodeType::EndElement;
		return true;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ParseText()
	{
		// currentCharacter == first character of text.

		SizeType tempRow;
		SizeType tempColumn;

		PrepareNode();

		if (IsWhiteSpace(currentCharacter))
		{
			do
			{
				CharactersWriterType::WriteCharacter(value, currentCharacter);
				
				if (NextCharBad(false))
				{
					if (eof)
					{
						if (unclosedTagsSize != 0)
						{
							UnclosedTagType& ref = unclosedTags[unclosedTagsSize - 1];
							Reset();
							SetError(ErrorCode::UnclosedTag);
							row = ref.Row;
							column = ref.Column;
							eof = true;
							return false;
						}

						node = NodeType::Whitespace;
						return true;
					}
					return false;
				}
			}
			while (IsWhiteSpace(currentCharacter));

			if (currentCharacter == LessThan)
			{
				node = NodeType::Whitespace;
				return true;
			}
		}

		if (unclosedTagsSize == 0)
		{
			tempRow = currentRow;
			tempColumn = currentColumn;
			Reset();
			SetError(ErrorCode::InvalidSyntax);
			row = tempRow;
			column = tempColumn;
			return false;
		}

		do
		{
			if (currentCharacter == Ampersand)
			{
				// TODO:
				assert(false && "Not implemented yet.");
			}

			if (currentCharacter == RightSquareBracket)
			{
				// TODO:
				assert(false && "Not implemented yet.");
			}

			CharactersWriterType::WriteCharacter(value, currentCharacter);
			if (NextCharBad(false))
			{
				if (eof)
				{
					UnclosedTagType& ref = unclosedTags[unclosedTagsSize - 1];
					Reset();
					SetError(ErrorCode::UnclosedTag);
					row = ref.Row;
					column = ref.Column;
					eof = true;
				}
				return false;
			}
		}
		while (currentCharacter != LessThan);

		node = NodeType::Text;
		return true;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ParseQuestion()
	{
		// currentCharacter == Question.
		// XmlDeclaration or ProcessingInstruction.
		// TODO:
		assert(false && "Not implemented yet.");
		return false;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ParseExclamation()
	{
		// currentCharacter == Exclamation.
		// Comment or DocumentType.
		// TODO:
		assert(false && "Not implemented yet.");
		return false;
	}
	
	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::PrepareNode()
	{
		name.clear();
		value.clear();
		localName.clear();
		prefix.clear();
		namespaceUri.clear();
		attributesSize = 0;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::NamespacesStuff()
	{
		// Collect namespaces from attributes.
		typedef typename std::deque<AttributeType>::iterator AttrIter;
		AttrIter attrEnd = attributes.begin() + attributesSize;
		for (AttrIter attr = attributes.begin(); attr != attrEnd; ++attr)
		{
			if (attr->Prefix.empty())
			{
				if (attr->LocalName == xmlnsString)
				{
					// Default namespace.
					// <mytag xmlns=...
					if (attr->Value == xmlUriString ||
						attr->Value == xmlnsUriString)
					{
						// <mytag xmlns="http://www.w3.org/XML/1998/namespace"...
						// or
						// <mytag xmlns="http://www.w3.org/2000/xmlns/"...
						Reset();
						SetError(ErrorCode::ReservedNamespaceAsDefault);
						row = attr->Row;
						column = attr->Column;
						return false;
					}

					NamespaceDeclarationType& ref = NewNamespace();
					ref.Uri = attr->Value;
					ref.TagIndex = static_cast<SizeType>(unclosedTagsSize);
				}
			}
			else if (attr->Prefix == xmlnsString)
			{
				if (attr->LocalName == xmlnsString)
				{
					// <mytag xmlns:xmlns=...
					Reset();
					SetError(ErrorCode::XmlnsDeclared);
					row = attr->Row;
					column = attr->Column;
					return false;
				}
				else if (attr->LocalName == lowerXmlString)
				{
					// <mytag xmlns:xml=...

					if (attr->Value != xmlUriString)
					{
						Reset();
						SetError(ErrorCode::InvalidXmlPrefixDeclaration);
						row = attr->Row;
						column = attr->Column;
						return false;
					}
				}
				else if (attr->Value == xmlUriString ||
					attr->Value == xmlnsUriString)
				{
					// <mytag xmlns:newprefix="http://www.w3.org/XML/1998/namespace"...
					// or
					// <mytag xmlns:newprefix="http://www.w3.org/2000/xmlns/"...
					Reset();
					SetError(ErrorCode::PrefixBoundToReservedNamespace);
					row = attr->Row;
					column = attr->Column;
					return false;
				}
				else if (attr->Value.empty())
				{
					// <mytag xmlns:newprefix=""...
					Reset();
					SetError(ErrorCode::PrefixWithEmptyNamespace);
					row = attr->Row;
					column = attr->Column;
					return false;
				}
				else
				{
					NamespaceDeclarationType& ref = NewNamespace();
					ref.Prefix = attr->LocalName;
					ref.Uri = attr->Value;
					ref.TagIndex = static_cast<SizeType>(unclosedTagsSize);
				}
			}
		}

		// Assign URIs to attributes.
		typedef typename std::deque<NamespaceDeclarationType>::const_iterator NamespaceIter;
		NamespaceIter namespaceLast = namespaces.begin();
		if (namespacesSize != 0)
			namespaceLast += (namespacesSize - 1);
		for (AttrIter attr = attributes.begin(); attr != attrEnd; ++attr)
		{
			if (!attr->Prefix.empty())
			{
				if (attr->Prefix == xmlnsString)
				{
					attr->NamespaceUri = xmlnsUriString;
				}
				else if (attr->Prefix == lowerXmlString)
				{
					attr->NamespaceUri = xmlUriString;
				}
				else
				{
					bool found = false;
					NamespaceIter n = namespaceLast;
					for ( ; n != namespaces.begin(); --n)
					{
						if (attr->Prefix == n->Prefix)
						{
							found = true;
							break;
						}
					}
					if (!found && (namespacesSize == 0 || attr->Prefix != n->Prefix))
					{
						Reset();
						SetError(ErrorCode::PrefixWithoutAssignedNamespace);
						row = attr->Row;
						column = attr->Column;
						return false;
					}
					else
					{
						attr->NamespaceUri = n->Uri;
					}
				}
			}
		}

		// Assign URI to element.
		if (!prefix.empty())
		{
			if (prefix == xmlnsString)
			{
				// row and column => '<'
				SizeType tempRow = row;
				SizeType tempColumn = column + 1;
				Reset();
				SetError(ErrorCode::PrefixWithoutAssignedNamespace);
				row = tempRow;
				column = tempColumn;
				return false;
			}
			else if (prefix == lowerXmlString)
			{
				namespaceUri = xmlUriString;
			}
			else
			{
				bool found = false;
				NamespaceIter n = namespaceLast;
				for ( ; n != namespaces.begin(); --n)
				{
					if (prefix == n->Prefix)
					{
						found = true;
						break;
					}
				}
				if (!found && (namespacesSize == 0 || prefix != n->Prefix))
				{
					// row and column => '<'
					SizeType tempRow = row;
					SizeType tempColumn = column + 1;
					Reset();
					SetError(ErrorCode::PrefixWithoutAssignedNamespace);
					row = tempRow;
					column = tempColumn;
					return false;
				}
				else
				{
					namespaceUri = n->Uri;
				}
			}
		}
		else // prefix.empty() == true.
		{
			// Find default namespace.
			bool found = false;
			NamespaceIter n = namespaceLast;
			for ( ; n != namespaces.begin(); --n)
			{
				if (n->Prefix.empty())
				{
					found = true;
					break;
				}
			}
			if (found || (namespacesSize != 0 && n->Prefix.empty()))
				namespaceUri = n->Uri;
		}

		// Ensure no double attribute name like:
		// <a x:local="first" y:local="second">
		// where x and y prefixes are bound to the same namespace URI.
		if (attributesSize > 1)
		{
			AttrIter attrEndMinus1 = attributes.begin() + (attributesSize - 1);
			for (AttrIter attr = attributes.begin(); attr != attrEndMinus1; ++attr)
			{
				if (!attr->Prefix.empty())
				{
					for (AttrIter next = attr + 1; next != attrEnd; ++next)
					{
						if (attr->LocalName == next->LocalName &&
							attr->NamespaceUri == next->NamespaceUri)
						{
							Reset();
							SetError(ErrorCode::DoubleAttributeName);
							row = next->Row;
							column = next->Column;
							return false;
						}
					}
				}
			}
		}

		return true;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::AttributeType&
		Inspector<TCharactersWriter>::NewAttribute()
	{
		AttributesSizeType fakeSize = static_cast<AttributesSizeType>(attributesSize);
		if (fakeSize < attributes.size())
		{
			++attributesSize;
			AttributeType& ref = attributes[fakeSize];
			ref.Name.clear();
			ref.Value.clear();
			ref.LocalName.clear();
			ref.Prefix.clear();
			ref.NamespaceUri.clear();
			return ref;
		}

		// fakeSize >= attributes.size().
		attributes.push_back(AttributeType());
		++attributesSize;
		AttributeType& ref = attributes.back();
		ref.Name.reserve(NameReserve);
		ref.Value.reserve(ValueReserve);
		ref.LocalName.reserve(LocalNameReserve);
		ref.Prefix.reserve(PrefixReserve);
		ref.NamespaceUri.reserve(NamespaceUriReserve);
		return ref;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::UnclosedTagType&
		Inspector<TCharactersWriter>::NewUnclosedTag()
	{
		UnclosedTagsSizeType fakeSize = static_cast<UnclosedTagsSizeType>(unclosedTagsSize);
		if (fakeSize < unclosedTags.size())
		{
			++unclosedTagsSize;
			UnclosedTagType& ref = unclosedTags[fakeSize];
			ref.Name.clear();
			ref.LocalName.clear();
			ref.Prefix.clear();
			ref.NamespaceUri.clear();
			return ref;
		}

		// fakeSize >= unclosedTags.size().
		unclosedTags.push_back(UnclosedTagType());
		++unclosedTagsSize;
		UnclosedTagType& ref = unclosedTags.back();
		ref.Name.reserve(NameReserve);
		ref.LocalName.reserve(LocalNameReserve);
		ref.Prefix.reserve(PrefixReserve);
		ref.NamespaceUri.reserve(NamespaceUriReserve);
		return ref;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::NamespaceDeclarationType&
		Inspector<TCharactersWriter>::NewNamespace()
	{
		NamespacesSizeType fakeSize = static_cast<NamespacesSizeType>(namespacesSize);
		if (fakeSize < namespaces.size())
		{
			++namespacesSize;
			NamespaceDeclarationType& ref = namespaces[fakeSize];
			ref.Prefix.clear();
			ref.Uri.clear();
			ref.TagIndex = 0;
			return ref;
		}

		// fakeSize >= namespaces.size().
		namespaces.push_back(NamespaceDeclarationType());
		++namespacesSize;
		NamespaceDeclarationType& ref = namespaces.back();
		ref.Prefix.reserve(PrefixReserve);
		ref.Uri.reserve(NamespaceUriReserve);
		return ref;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::SavePosition()
	{
		row = currentRow;
		column = currentColumn;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::NextCharBad(bool insideTag)
	{
		// x, CR, LF, y => x, LF, y
		// x, CR, y => x, LF, y
		// x, LF, CR, y => x, LF, LF, y
		// Check http://www.w3.org/TR/REC-xml/#sec-line-ends.

		SizeType tempRow;
		SizeType tempColumn;

		if (currentCharacter == LineFeed)
		{
			++currentRow;
			currentColumn = 1;
		}
		else
		{
			++currentColumn;
		}

		if (bufferedCharacter != 0)
		{
			if (bufferedCharacter > 3) // Allowed character.
			{
				currentCharacter = bufferedCharacter;
				bufferedCharacter = 0;
				return false;
			}

			if (bufferedCharacter == 1) // No more characters to read.
			{
				if (insideTag)
				{
					// Start token position.
					tempRow = row;
					tempColumn = column;
					Reset();
					row = tempRow;
					column = tempColumn;
					SetError(ErrorCode::UnclosedToken);
					eof = true;
				}
				else
				{
					eof = true;
					bufferedCharacter = 0;
				}
			}
			else if (bufferedCharacter == 2) // Character is not allowed in XML document.
			{
				// Invalid character position.
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				row = tempRow;
				column = tempColumn;
				SetError(ErrorCode::InvalidByteSequence);
			}
			else // bufferedCharacter == 3 // Stream error.
			{
				// Character at stream error position.
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				row = tempRow;
				column = tempColumn;
				SetError(ErrorCode::StreamError);
			}
			afterBom = true;
			return true;
		}

		int result = reader->ReadCharacter(currentCharacter);

		if (result == 1) // Character was read successfully.
		{
			if (currentCharacter == CarriageReturn) // We don't like CR.
			{
				result = reader->ReadCharacter(currentCharacter);
				if (result == 1) // Second character was read successfully.
				{
					if (currentCharacter != LineFeed)
					{
						// CR, x => LF, x
						bufferedCharacter = currentCharacter;
						currentCharacter = LineFeed;
					}
					// else CR, LF => LF
				}
				else if (result == 0) // No more characters to read.
				{
					// CR, [end of document] => LF, [end of document]
					bufferedCharacter = 1;
					currentCharacter = LineFeed;
				}
				else if (result == -1) // Character is not allowed in XML document.
				{
					// CR, [not allowed character] => LF, [not allowed character]
					bufferedCharacter = 2;
					currentCharacter = LineFeed;
				}
				else // result == -2 // Stream error.
				{
					// CR, [stream error] => LF, [stream error]
					bufferedCharacter = 3;
					currentCharacter = LineFeed;
				}
			}
			return false;
		}

		if (result == 0) // No more characters to read.
		{
			if (insideTag)
			{
				// Start token position.
				tempRow = row;
				tempColumn = column;
				Reset();
				row = tempRow;
				column = tempColumn;
				SetError(ErrorCode::UnclosedToken);
				eof = true;
			}
			else
			{
				eof = true;
			}
		}
		else if (result == -1) // Character is not allowed in XML document.
		{
			// Invalid character position.
			tempRow = currentRow;
			tempColumn = currentColumn;
			Reset();
			row = tempRow;
			column = tempColumn;
			SetError(ErrorCode::InvalidByteSequence);
		}
		else // result == -2 // Stream error.
		{
			// Character at stream error position.
			tempRow = currentRow;
			tempColumn = currentColumn;
			Reset();
			row = tempRow;
			column = tempColumn;
			SetError(ErrorCode::StreamError);
		}
		afterBom = true;
		return true;
	}

	template <typename TCharactersWriter>
	inline bool Inspector<TCharactersWriter>::ReadNode()
	{
		SizeType tempRow;
		SizeType tempColumn;
		if (!afterBom && (err == ErrorCode::None || err == ErrorCode::StreamError))
		{
			// First call of ReadNode method or after stream error while BOM parsing.
			ParseBom();
			if (err != ErrorCode::None)
				return false;
			row = 1;
			column = 1;
			if (eof)
			{
				SetError(ErrorCode::NoElement);
				return false;
			}
			currentRow = 1;
			currentColumn = 0; // Don't worry,
			// it will be 1 after first call of NextCharBad method.

			// First character.
			if (NextCharBad(false))
			{
				if (eof)
				{
					SetError(ErrorCode::NoElement);
					return false;
				}
			}

			if (IsWhiteSpace(currentCharacter))
			{
				do
				{
					CharactersWriterType::WriteCharacter(value, currentCharacter);
					if (NextCharBad(false))
					{
						if (eof) // White spaces followed by end of file.
						{
							// In XML document at least one root element is required.
							tempRow = currentRow;
							tempColumn = currentColumn;
							Reset();
							SetError(ErrorCode::NoElement);
							row = tempRow;
							column = tempColumn;
							return false;
						}
						else // White spaces followed by invalid character or stream error.
						{
							// Error is set already.
							return false;
						}
					}
				}
				while (IsWhiteSpace(currentCharacter));

				if (currentCharacter != LessThan)
				{
					tempRow = currentRow;
					tempColumn = currentColumn;
					Reset();
					SetError(ErrorCode::InvalidSyntax);
					row = tempRow;
					column = tempColumn;
					return false;
				}
				
				node = NodeType::Whitespace;
				return true;
			}

			if (currentCharacter != LessThan)
			{
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				SetError(ErrorCode::InvalidSyntax);
				row = tempRow;
				column = tempColumn;
			}
		}

		if (err != ErrorCode::None)
			return false;

		if (currentCharacter == GreaterThan ||
			currentCharacter == Semicolon)
		{
			// End of token or reference.
			if (NextCharBad(false) && !eof)
				return false;
		}

		if (eof)
		{
			if (!foundElement)
			{
				// In XML document at least one root element is required.
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				SetError(ErrorCode::NoElement);
				row = tempRow;
				column = tempColumn;
			}
			else if (unclosedTagsSize != 0)
			{
				UnclosedTagType& ref = unclosedTags[unclosedTagsSize - 1];
				Reset();
				SetError(ErrorCode::UnclosedTag);
				row = ref.Row;
				column = ref.Column;
				foundElement = true;
			}
			else
			{
				// XML document is fully parsed without any error.
				tempRow = currentRow;
				tempColumn = currentColumn;
				Reset();
				row = tempRow;
				column = tempColumn;
				node = NodeType::None;
			}
			eof = true;
			return false;
		}

		if (currentCharacter == LessThan) // New tag to parse.
		{
			SavePosition();

			// Go to the next character after '<'.
			if (NextCharBad(true))
				return false;

			if (currentCharacter == Slash)
			{
				// EndElement.
				return ParseEndElement();
			}

			if (currentCharacter != Colon &&
				Encoding::CharactersReader::IsNameStartChar(currentCharacter))
			{
				// StartElement or EmptyElement.
				return ParseElement();
			}

			if (currentCharacter == Exclamation)
			{
				// Comment or DocumentType.
				return ParseExclamation();
			}

			if (currentCharacter == Question)
			{
				// XmlDeclaration or ProcessingInstruction.
				return ParseQuestion();
			}

			// currentCharacter is not allowed here.
			tempRow = currentRow;
			tempColumn = currentColumn;
			if (Encoding::CharactersReader::IsNameChar(currentCharacter))
			{
				// Not allowed as start character of the name,
				// but allowed as a part of this name.
				Reset();
				SetError(ErrorCode::InvalidTagName);
			}
			else
			{
				// Some weird character.
				Reset();
				SetError(ErrorCode::InvalidSyntax);
			}
			row = tempRow;
			column = tempColumn;
			return false;
		}

		// currentCharacter == first character of text.
		SavePosition();
		return ParseText();
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
	inline bool Inspector<TCharactersWriter>::HasAttributes() const
	{
		return (attributesSize != 0);
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetAttributesCount() const
	{
		return static_cast<SizeType>(attributesSize);
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::AttributeIterator
		Inspector<TCharactersWriter>::GetAttributeBegin() const
	{
		return attributes.cbegin();
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::AttributeIterator
		Inspector<TCharactersWriter>::GetAttributeEnd() const
	{
		return attributes.cbegin() + attributesSize;
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
		Inspector<TCharactersWriter>::GetRow() const
	{
		return row;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetColumn() const
	{
		return column;
	}

	template <typename TCharactersWriter>
	inline typename Inspector<TCharactersWriter>::SizeType
		Inspector<TCharactersWriter>::GetDepth() const
	{
		if (node == NodeType::StartElement ||
			node == NodeType::EmptyElement)
			return static_cast<SizeType>(unclosedTagsSize - 1);
		else
			return static_cast<SizeType>(unclosedTagsSize);
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset()
	{
		row = 0;
		column = 0;
		currentRow = 0;
		currentColumn = 0;
		node = NodeType::None;
		err = ErrorCode::None;
		errMsg = nullptr;
		afterBom = false;
		bom = Details::Bom::None;
		name.clear();
		value.clear();
		localName.clear();
		prefix.clear();
		namespaceUri.clear();
		currentCharacter = 0;
		bufferedCharacter = 0;
		foundElement = false;
		eof = false;
		attributesSize = 0;
		unclosedTagsSize = 0;
		namespacesSize = 0;
		if (sourceType == SourcePath)
		{
			fPath.clear();
			if (fileStream.is_open())
				fileStream.close();
			fileStream.clear();
			delete reader;
			reader = nullptr;
		}
		else if (sourceType == SourceReader)
		{
			reader = nullptr;
		}
		else if (sourceType == SourceStream)
		{
			inputStreamPtr = nullptr;
			delete reader;
			reader = nullptr;
		}
		else if (sourceType == SourceIterators)
		{
			delete inputStreamPtr->rdbuf();
			delete inputStreamPtr;
			inputStreamPtr = nullptr;
			delete reader;
			reader = nullptr;
		}
		sourceType = SourceNone;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(const char* filePath)
	{
		Reset();
		fPath = filePath;
		sourceType = SourcePath;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(const std::string& filePath)
	{
		Reset();
		fPath = filePath;
		sourceType = SourcePath;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(std::istream* inputStream)
	{
		Reset();
		inputStreamPtr = inputStream;
		sourceType = SourceStream;
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
		sourceType = SourceIterators;
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Reset(Encoding::CharactersReader* r)
	{
		Reset();
		if (r != nullptr)
		{
			reader = r;
			sourceType = SourceReader;
		}
	}

	template <typename TCharactersWriter>
	inline void Inspector<TCharactersWriter>::Clear()
	{
		Reset();
		attributes.clear();
		unclosedTags.clear();
		namespaces.clear();
	}

	/// @cond DETAILS
	namespace Details
	{
		inline Bom ReadBom(std::istream* inputStream)
		{
			// UTF-8          EF BB BF
			// UTF-16 (BE)    FE FF
			// UTF-16 (LE)    FF FE
			// UTF-32 (BE)    00 00 FE FF
			// UTF-32 (LE)    FF FE 00 00

			if (inputStream != nullptr)
			{
				// Check first byte.
				int oneByte = inputStream->peek();

				if (oneByte == std::char_traits<char>::eof())
				{
					if ((inputStream->rdstate() & std::istream::eofbit) != 0)
						return Bom::None; // End of the stream.
					return Bom::StreamError;
				}

				if (oneByte == 0xEF) // Should be UTF-8
				{
					// Extract first byte.
					inputStream->get();

					// Extract second byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xBB)
						return Bom::Invalid;

					// Extract third byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xBF)
						return Bom::Invalid;

					return Bom::Utf8;
				}
				else if (oneByte == 0xFE) // Should be UTF-16 (BE)
				{
					// Extract first byte.
					inputStream->get();

					// Extract second byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xFF)
						return Bom::Invalid;

					return Bom::Utf16BE;
				}
				else if (oneByte == 0xFF) // Should be UTF-16 (LE) of UTF-32 (LE)
				{
					// Extract first byte.
					inputStream->get();

					// Extract second byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xFE)
						return Bom::Invalid;

					// Check third byte.
					oneByte = inputStream->peek();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Utf16LE;
						return Bom::StreamError;
					}
					if (oneByte != 0x00)
						return Bom::Utf16LE;

					// Should be UTF-32 (LE)
					// Extract third byte.
					inputStream->get();

					// Extract fourth byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0x00)
						return Bom::Invalid;

					return Bom::Utf32LE;
				}
				else if (oneByte == 0x00) // Should be UTF-32 (BE)
				{
					// Extract first byte.
					inputStream->get();

					// Extract second byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0x00)
						return Bom::Invalid;

					// Extract third byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xFE)
						return Bom::Invalid;

					// Extract fourth byte.
					oneByte = inputStream->get();
					if (oneByte == std::char_traits<char>::eof())
					{
						if ((inputStream->rdstate() & std::istream::eofbit) != 0)
							return Bom::Invalid;
						return Bom::StreamError;
					}
					if (oneByte != 0xFF)
						return Bom::Invalid;

					return Bom::Utf32BE;
				}
				else
				{
					return Bom::None;
				}
			}
			return Bom::StreamError;
		}
		
		template <typename TInputIterator>
		inline Bom ReadBom(TInputIterator& first, TInputIterator& last)
		{
			// UTF-8          EF BB BF
			// UTF-16 (BE)    FE FF
			// UTF-16 (LE)    FF FE
			// UTF-32 (BE)    00 00 FE FF
			// UTF-32 (LE)    FF FE 00 00

			if (first == last)
				return Bom::None;

			if (*first == 0xEF) // Should be UTF-8
			{
				++first;
				if (first == last || *first != 0xBB)
					return Bom::Invalid;

				++first;
				if (first == last || *first != 0xBF)
					return Bom::Invalid;

				++first;
				return Bom::Utf8;
			}
			else if (*first == 0xFE) // Should be UTF-16 (BE)
			{
				++first;
				if (first == last || *first != 0xFF)
					return Bom::Invalid;

				++first;
				return Bom::Utf16BE;
			}
			else if (*first == 0xFF) // Should be UTF-16 (LE) of UTF-32 (LE)
			{
				++first;
				if (first == last || *first != 0xFE)
					return Bom::Invalid;

				++first;
				if (first == last || *first != 0x00)
					return Bom::Utf16LE;

				// Should be UTF-32 (LE)
				++first;
				if (first == last || *first != 0x00)
					return Bom::Invalid;

				++first;
				return Bom::Utf32LE;
			}
			else if (*first == 0x00) // Should be UTF-32 (BE)
			{
				++first;
				if (first == last || *first != 0x00)
					return Bom::Invalid;

				++first;
				if (first == last || *first != 0xFE)
					return Bom::Invalid;

				++first;
				if (first == last || *first != 0xFF)
					return Bom::Invalid;

				++first;
				return Bom::Utf32BE;
			}
			else
			{
				return Bom::None;
			}
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

