/// @cond TEST
#include "XmlInspector.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <streambuf>
#include <fstream>
#include <cstddef>
#include <algorithm>
#include <list>
#include <cassert>

class MemBuf : public std::streambuf
{
public:
	MemBuf(void* p, std::size_t n)
	{
		setg(static_cast<char*>(p), static_cast<char*>(p),
			static_cast<char*>(p) + n);
	}
};

static bool IsBigEndian()
{
	char32_t value = 0xAABBCCDD;
	unsigned char bytes[4];
	memcpy(bytes, &value, 4);
	return bytes[0] == 0xAA;
}

class Test
{
public:
	void Start()
	{
		std::cout << "--START TEST--\n";

		Utf8StreamReaderTest();
		Utf16BEStreamReaderTest();
		Utf16LEStreamReaderTest();
		Utf32BEStreamReaderTest();
		Utf32LEStreamReaderTest();
		Utf8IteratorsReaderTest();
		Utf8WriterTest();
		Utf16WriterTest();
		Utf32WriterTest();
		InspectorConstructorsTest();
		InspectorResetTest();
		InspectorClearTest();
		BeforeParsingTest();
		NoSourceTest();
		IteratorsToIstreamTest();
		NoBomStreamTest();
		BomUtf8StreamTest();
		BomUtf16BEStreamTest();
		BomUtf16LEStreamTest();
		BomUtf32BEStreamTest();
		BomUtf32LEStreamTest();
		NoBomIteratorsTest();
		BomUtf8IteratorsTest();
		BomUtf16BEIteratorsTest();
		BomUtf16LEIteratorsTest();
		BomUtf32BEIteratorsTest();
		BomUtf32LEIteratorsTest();
		EmptyDocumentTest();
		WhitespaceTest();
		InvalidByteSequenceTest();
		InvalidSyntaxTest();
		IsCharTest();
		IsWhiteSpaceTest();
		IsNameStartCharTest();
		IsNameCharTest();
		IsEncNameStartCharTest();
		IsEncNameCharTest();
		GetHexDigitValueTest();
		UnclosedTokenTest();
		StartElementTest();
		TagNameAfterSpaceTest();
		InvalidTagNameStartCharTest();
		InvalidTagNameCharTest();
		WeirdTagNameStartCharTest();
		ValidTagNameTest();
		XmlElementPrefixTest();
		XmlnsElementPrefixTest();
		UnexpectedEndTagTest();
		StartElementPlusEndElementTest();
		WhitespacePlusElementTest();
		TextPlusElementTest();
		UnclosedTagTest();

		std::cout << "--END TEST--\n";
	}

	void Utf8StreamReaderTest()
	{
		std::cout << "UTF-8 stream reader test... ";

		// Memory buffer to istream.
		char source[] = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		MemBuf buf(source, sizeof(source) - 1);
		std::istream is(&buf);

		Xml::Encoding::Utf8StreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf16BEStreamReaderTest()
	{
		std::cout << "UTF-16 (big endian) stream reader test... ";

		// Memory buffer to istream.
		char16_t source[] = u"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		if (!IsBigEndian())
		{
			for (unsigned i = 0; i < (sizeof(source) - sizeof(char16_t)) / 2; ++i)
			{
				char16_t sw = source[i] << 8;
				sw |= (source[i] >> 8);
				source[i] = sw;
			}
		}
		MemBuf buf(source, sizeof(source) - sizeof(char16_t));
		std::istream is(&buf);

		Xml::Encoding::Utf16BEStreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf16LEStreamReaderTest()
	{
		std::cout << "UTF-16 (little endian) stream reader test... ";

		// Memory buffer to istream.
		char16_t source[] = u"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		if (IsBigEndian())
		{
			for (unsigned i = 0; i < (sizeof(source) - sizeof(char16_t)) / 2; ++i)
			{
				char16_t sw = source[i] << 8;
				sw |= (source[i] >> 8);
				source[i] = sw;
			}
		}
		MemBuf buf(source, sizeof(source) - sizeof(char16_t));
		std::istream is(&buf);

		Xml::Encoding::Utf16LEStreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf32BEStreamReaderTest()
	{
		std::cout << "UTF-32 (big endian) stream reader test... ";

		// Memory buffer to istream.
		char32_t source[] = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		if (!IsBigEndian())
		{
			for (unsigned i = 0; i < (sizeof(source) - sizeof(char32_t)) / 4; ++i)
			{
				char32_t sw = source[i] << 24;
				sw |= (source[i] & 0x0000FF00) << 8;
				sw |= (source[i] & 0x00FF0000) >> 8;
				sw |= source[i] >> 24;
				source[i] = sw;
			}
		}
		MemBuf buf(source, sizeof(source) - sizeof(char32_t));
		std::istream is(&buf);

		Xml::Encoding::Utf32BEStreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf32LEStreamReaderTest()
	{
		std::cout << "UTF-32 (little endian) stream reader test... ";

		// Memory buffer to istream.
		char32_t source[] = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		if (IsBigEndian())
		{
			for (unsigned i = 0; i < (sizeof(source) - sizeof(char32_t)) / 4; ++i)
			{
				char32_t sw = source[i] << 24;
				sw |= (source[i] & 0x0000FF00) << 8;
				sw |= (source[i] & 0x00FF0000) >> 8;
				sw |= source[i] >> 24;
				source[i] = sw;
			}
		}
		MemBuf buf(source, sizeof(source) - sizeof(char32_t));
		std::istream is(&buf);

		Xml::Encoding::Utf32LEStreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf8IteratorsReaderTest()
	{
		std::cout << "UTF-8 iterator reader test... ";

		std::string source = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		Xml::Encoding::Utf8IteratorsReader<std::string::const_iterator> reader(source.cbegin(), source.cend());
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);
		
		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf8WriterTest()
	{
		std::cout << "UTF-8 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::string destination;
		std::string pattern = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf8Writer::WriteCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf16WriterTest()
	{
		std::cout << "UTF-16 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u16string destination;
		std::u16string pattern = u"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf16Writer::WriteCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf32WriterTest()
	{
		std::cout << "UTF-32 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u32string destination;
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf32Writer::WriteCharacter(destination, *i);

		assert(destination == source);

		std::cout << "OK\n";
	}

	void InspectorConstructorsTest()
	{
		std::cout << "Inspector constructors test... ";

		// Defaul constructor.
		Xml::Inspector<Xml::Encoding::Utf32Writer> i0;

		// File path from const char*
		Xml::Inspector<Xml::Encoding::Utf8Writer> i1("test.xml");

		// File path from const std::string&
		std::string path = "test.xml";
		Xml::Inspector<Xml::Encoding::Utf16Writer> i2(path);

		// From input stream.
		std::ifstream ifs("test.xml");
		Xml::Inspector<Xml::Encoding::Utf32Writer> i3(&ifs);
	
		// From bytes.
		char bytes[] = u8"<root>bytes test</root>";
		Xml::Inspector<Xml::Encoding::Utf16Writer> i4(bytes, bytes + sizeof(bytes));

		// From reader interface.
		std::string content = u8"<root>abc</root>";
		Xml::Encoding::Utf8IteratorsReader<std::string::const_iterator> reader(content.cbegin(), content.cend());
		Xml::Inspector<Xml::Encoding::Utf32Writer> i5(&reader);

		std::cout << "OK\n";
	}

	void InspectorResetTest()
	{
		std::cout << "Inspector reset test... ";

		Xml::Inspector<Xml::Encoding::Utf32Writer> inspector;

		// File path from const char*
		inspector.Reset("test.xml");

		// Nothing.
		inspector.Reset();

		// File path from const std::string&
		std::string path = "test.xml";
		inspector.Reset(path);

		// From input stream.
		std::ifstream ifs("test.xml");
		inspector.Reset(&ifs);
	
		// From bytes.
		char bytes[] = u8"<root>bytes test</root>";
		inspector.Reset(bytes, bytes + sizeof(bytes));

		// From reader interface.
		std::string content = u8"<root>abc</root>";
		Xml::Encoding::Utf8IteratorsReader<std::string::const_iterator> reader(content.cbegin(), content.cend());
		inspector.Reset(&reader);

		std::cout << "OK\n";
	}

	void InspectorClearTest()
	{
		std::cout << "Inspector clear test... ";

		Xml::Inspector<Xml::Encoding::Utf32Writer> inspector;

		// File path from const char*
		inspector.Reset("test.xml");

		// Nothing.
		inspector.Clear();

		// From input stream.
		std::ifstream ifs("test.xml");
		inspector.Reset(&ifs);

		std::cout << "OK\n";
	}

	void BeforeParsingTest()
	{
		std::cout << "Before parsing test... ";

		Xml::Inspector<Xml::Encoding::Utf16Writer> inspector("test.xml");

		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 0);
		assert(inspector.GetColumn() == 0);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void NoSourceTest()
	{
		std::cout << "No source test... ";

		Xml::Inspector<Xml::Encoding::Utf16Writer> inspector;
		bool foundNode = inspector.ReadNode();

		assert(!foundNode);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::StreamError);
		assert(inspector.GetRow() == 0);
		assert(inspector.GetColumn() == 0);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void IteratorsToIstreamTest()
	{
		std::cout << "Iterators to istream test... ";
	
		char source[] = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::list<char> listSource(source, source + sizeof(source) - 1);
		Xml::Details::BasicIteratorsBuf<std::list<char>::const_iterator, char>
			buf(listSource.cbegin(), listSource.cend());
		std::istream is(&buf);

		Xml::Encoding::Utf8StreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void NoBomStreamTest()
	{
		std::cout << "No BOM in stream test... ";

		unsigned char source[3];
		source[0] = 0xA1; source[1] = 0xA2; source[2] = 0xA3; // random
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::None);
		int next = is.get();
		assert(next == 0xA1);

		MemBuf buf2(source, 0);
		std::istream is2(&buf2);
		bom = Xml::Details::ReadBom(&is2);
		assert(bom == Xml::Details::Bom::None);
	
		std::cout << "OK\n";
	}

	void BomUtf8StreamTest()
	{
		std::cout << "BOM UTF-8 in stream test... ";
	
		unsigned char source[4];
		source[0] = 0xEF; source[1] = 0xBB; source[2] = 0xBF;
		source[3] = 0x04; // <= random number.
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::Utf8);
		int next = is.get();
		assert(next == 0x04);

		std::cout << "OK\n";
	}

	void BomUtf16BEStreamTest()
	{
		std::cout << "BOM UTF-16 (big endian) in stream test... ";
	
		unsigned char source[3];
		source[0] = 0xFE; source[1] = 0xFF;
		source[2] = 0x04; // <= random number.
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::Utf16BE);
		int next = is.get();
		assert(next == 0x04);

		std::cout << "OK\n";
	}

	void BomUtf16LEStreamTest()
	{
		std::cout << "BOM UTF-16 (little endian) in stream test... ";
	
		unsigned char source[3];
		source[0] = 0xFF; source[1] = 0xFE;
		source[2] = 0x04; // <= random number.
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::Utf16LE);
		int next = is.get();
		assert(next == 0x04);

		std::cout << "OK\n";
	}

	void BomUtf32BEStreamTest()
	{
		std::cout << "BOM UTF-32 (big endian) in stream test... ";
	
		unsigned char source[5];
		source[0] = 0x00; source[1] = 0x00; source[2] = 0xFE; source[3] = 0xFF;
		source[4] = 0x04; // <= random number.
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::Utf32BE);
		int next = is.get();
		assert(next == 0x04);

		std::cout << "OK\n";
	}

	void BomUtf32LEStreamTest()
	{
		std::cout << "BOM UTF-32 (little endian) in stream test... ";
	
		unsigned char source[5];
		source[0] = 0xFF; source[1] = 0xFE; source[2] = 0x00; source[3] = 0x00;
		source[4] = 0x04; // <= random number.
		MemBuf buf(source, sizeof(source));
		std::istream is(&buf);

		Xml::Details::Bom bom = Xml::Details::ReadBom(&is);
		assert(bom == Xml::Details::Bom::Utf32LE);
		int next = is.get();
		assert(next == 0x04);

		std::cout << "OK\n";
	}

	void NoBomIteratorsTest()
	{
		std::cout << "No BOM in iterators test... ";

		unsigned char source[3];
		source[0] = 0xA1; source[1] = 0xA2; source[2] = 0xA3; // random
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::None);
		assert(begin == source);
		assert(end == source + sizeof(source));

		end = begin;
		bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::None);
		assert(begin == source);
		assert(end == source);
	
		std::cout << "OK\n";
	}

	void BomUtf8IteratorsTest()
	{
		std::cout << "BOM UTF-8 in iterators test... ";
	
		unsigned char source[4];
		source[0] = 0xEF; source[1] = 0xBB; source[2] = 0xBF;
		source[3] = 0x04; // <= random number.
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::Utf8);
		assert(begin == (source + sizeof(source) - 1));
		assert(end == source + sizeof(source));

		std::cout << "OK\n";
	}

	void BomUtf16BEIteratorsTest()
	{
		std::cout << "BOM UTF-16 (big endian) in iterators test... ";
	
		unsigned char source[3];
		source[0] = 0xFE; source[1] = 0xFF;
		source[2] = 0x04; // <= random number.
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::Utf16BE);
		assert(begin == (source + sizeof(source) - 1));
		assert(end == source + sizeof(source));

		std::cout << "OK\n";
	}

	void BomUtf16LEIteratorsTest()
	{
		std::cout << "BOM UTF-16 (little endian) in iterators test... ";
	
		unsigned char source[3];
		source[0] = 0xFF; source[1] = 0xFE;
		source[2] = 0x04; // <= random number.
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::Utf16LE);
		assert(begin == (source + sizeof(source) - 1));
		assert(end == source + sizeof(source));

		std::cout << "OK\n";
	}

	void BomUtf32BEIteratorsTest()
	{
		std::cout << "BOM UTF-32 (big endian) in iterators test... ";
	
		unsigned char source[5];
		source[0] = 0x00; source[1] = 0x00; source[2] = 0xFE; source[3] = 0xFF;
		source[4] = 0x04; // <= random number.
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::Utf32BE);
		assert(begin == (source + sizeof(source) - 1));
		assert(end == source + sizeof(source));

		std::cout << "OK\n";
	}

	void BomUtf32LEIteratorsTest()
	{
		std::cout << "BOM UTF-32 (little endian) in iterators test... ";
	
		unsigned char source[5];
		source[0] = 0xFF; source[1] = 0xFE; source[2] = 0x00; source[3] = 0x00;
		source[4] = 0x04; // <= random number.
		unsigned char* begin = source;
		unsigned char* end = source + sizeof(source);

		Xml::Details::Bom bom = Xml::Details::ReadBom(begin, end);
		assert(bom == Xml::Details::Bom::Utf32LE);
		assert(begin == (source + sizeof(source) - 1));
		assert(end == source + sizeof(source));

		std::cout << "OK\n";
	}

	void EmptyDocumentTest()
	{
		std::cout << "Empty document test... ";

		std::string docString = "";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::NoElement);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void WhitespaceTest()
	{
		std::cout << "Whitespace test... ";

		unsigned char source[] = { 0x0D, 0x09, 0x0D, 0x0A, 0x20, 0x0A, 0x0D, 0x20, 0x0D };
		const char* pattern = u8"\n\t\n \n\n \n"; // Check http://www.w3.org/TR/REC-xml/#sec-line-ends.
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			source, source + sizeof(source));

		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::Whitespace);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue() == pattern);
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::NoElement);
		assert(inspector.GetRow() == 6);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void InvalidByteSequenceTest()
	{
		std::cout << "Invalid byte sequence test... ";

		unsigned char source[] = { 0x0D, 0x09, 0x0D, 0x0A, 0x09, 0x04, 0x0D, 0x20, 0x0D };
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			source, source + sizeof(source));

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidByteSequence);
		assert(inspector.GetRow() == 3);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void InvalidSyntaxTest()
	{
		std::cout << "Invalid syntax test... ";

		std::string source = u8"  \t \n  bad<root />";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			source.begin(), source.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidSyntax);
		assert(inspector.GetRow() == 2);
		assert(inspector.GetColumn() == 3);
		assert(inspector.GetDepth() == 0);

		source = u8"<a></a>\n  bad \n";
		inspector.Reset(source.begin(), source.end());

		// <a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// </a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 0);

		// bad
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidSyntax);
		assert(inspector.GetRow() == 2);
		assert(inspector.GetColumn() == 3);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void IsCharTest()
	{
		std::cout << "Is char test... ";

		char32_t allowed[] = {
			0x9, 0xA, 0xD, 0x20, 0xABC, 0xD7FF, 0xE000,
			0xEFAB, 0xFFFD, 0x10000, 0xABCDE, 0x10FFFF };

		char32_t notAllowed[] = {
			0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
			0x8, 0xB, 0xC, 0xE, 0xF, 0x10, 0x15, 0x19,
			0xD800, 0xDFFF, 0xFFFE, 0xFFFF, 0x110000,
			0xFFFFFF, ~((char32_t)0) };

		for (std::size_t i = 0; i < sizeof(allowed) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsChar(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
			assert(!Xml::Encoding::CharactersReader::IsChar(notAllowed[i]));

		std::cout << "OK\n";
	}

	void IsWhiteSpaceTest()
	{
		std::cout << "Is white space test... ";

		char32_t allowed[] = { 0x20, 0x9, 0xD, 0xA };

		char32_t notAllowed[] = {
			0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xB, 0xC, 0xE,
			0xF, 0xFF, 0xFFF, 0xFFFF, 0xFFFFF, ~((char32_t)0) };

		for (std::size_t i = 0; i < sizeof(allowed) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsWhiteSpace(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
			assert(!Xml::Encoding::CharactersReader::IsWhiteSpace(notAllowed[i]));

		std::cout << "OK\n";
	}

	void IsNameStartCharTest()
	{
		std::cout << "Is name start char test... ";

		char32_t allowed[] = {
			0x3A, 0x41, 0x4B, 0x5A, 0x5F, 0x61, 0x6B, 0x7A,
			0xC0, 0xCB, 0xD6, 0xD8, 0xE1, 0xF6, 0xF8, 0x100,
			0x2FF, 0x370, 0x377, 0x37D, 0x37F, 0x1000, 0x1FFF, 0x200C,
			0x200D, 0x2070, 0x2111, 0x218F, 0x2C00, 0x2DDD, 0x2FEF, 0x3001,
			0xAAAA, 0xD7FF, 0xF900, 0xFABC, 0xFDCF, 0xFDF0, 0xFEEE, 0xFFFD,
			0x10000, 0xAAAAA, 0xEFFFF };

		char32_t notAllowed[] = {
			0x0, 0x1, 0x9, 0xA, 0xD, 0x20, 0x30, 0x37, 0x39, 0x5B, 0x60, 0x7B,
			0xBF, 0xD7, 0xF7, 0x300, 0x36F, 0x37E, 0x2000, 0x200B, 0x200E, 0x206F,
			0x2190, 0x2BFF, 0x2FF0, 0x2FFF, 0x3000, 0xD800, 0xF8FF, 0xFDD0, 0xFDEF,
			0xFFFE, 0xFFFF, 0xF0000, 0xFFFFF, ~((char32_t)0) };

		for (std::size_t i = 0; i < sizeof(allowed) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsNameStartChar(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
			assert(!Xml::Encoding::CharactersReader::IsNameStartChar(notAllowed[i]));

		std::cout << "OK\n";
	}

	void IsNameCharTest()
	{
		std::cout << "Is name char test... ";

		char32_t allowed[] = {
			0x3A, 0x41, 0x4B, 0x5A, 0x5F, 0x61, 0x6B, 0x7A,
			0xC0, 0xCB, 0xD6, 0xD8, 0xE1, 0xF6, 0xF8, 0x100,
			0x2FF, 0x370, 0x377, 0x37D, 0x37F, 0x1000, 0x1FFF, 0x200C,
			0x200D, 0x2070, 0x2111, 0x218F, 0x2C00, 0x2DDD, 0x2FEF, 0x3001,
			0xAAAA, 0xD7FF, 0xF900, 0xFABC, 0xFDCF, 0xFDF0, 0xFEEE, 0xFFFD,
			0x10000, 0xAAAAA, 0xEFFFF, 0x2D, 0x2E, 0x30, 0x35, 0x39, 0xB7,
			0x300, 0x333, 0x36F, 0x203F, 0x2040 };

		char32_t notAllowed[] = {
			0x0, 0x1, 0x9, 0xA, 0xD, 0x20, 0x2F, 0x3B, 0x5B, 0x60, 0x7B,
			0xBF, 0xD7, 0xF7, 0x37E, 0x2000, 0x200B, 0x200E, 0x206F,
			0x2190, 0x2BFF, 0x2FF0, 0x2FFF, 0x3000, 0xD800, 0xF8FF, 0xFDD0, 0xFDEF,
			0xFFFE, 0xFFFF, 0xF0000, 0xFFFFF, ~((char32_t)0) };

		for (std::size_t i = 0; i < sizeof(allowed) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsNameChar(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
		{
			if (Xml::Encoding::CharactersReader::IsNameChar(notAllowed[i]))
				std::cout << "dupa:" << i << "\n";
			assert(!Xml::Encoding::CharactersReader::IsNameChar(notAllowed[i]));
		}

		std::cout << "OK\n";
	}

	void IsEncNameStartCharTest()
	{
		std::cout << "Is enc name start char test... ";

		const char32_t* allowed = U"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

		char32_t notAllowed[] = { 0x0, 0x30, 0x35, 0x39, 0x40, 0x5B, 0x60, 0x7B, 0x4100, ~((char32_t)0) };

		for (std::size_t i = 0; i < (sizeof(allowed) - sizeof(char32_t)) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsEncNameStartChar(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
			assert(!Xml::Encoding::CharactersReader::IsEncNameStartChar(notAllowed[i]));

		std::cout << "OK\n";
	}

	void IsEncNameCharTest()
	{
		std::cout << "Is enc name char test... ";

		const char32_t* allowed = U"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-";

		char32_t notAllowed[] = { 0x0, 0x1, 0x40, 0x5B, 0x60, 0x7B, 0x4100, ~((char32_t)0) };

		for (std::size_t i = 0; i < (sizeof(allowed) - sizeof(char32_t)) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::IsEncNameChar(allowed[i]));

		for (std::size_t i = 0; i < sizeof(notAllowed) / sizeof(char32_t); ++i)
			assert(!Xml::Encoding::CharactersReader::IsEncNameChar(notAllowed[i]));

		std::cout << "OK\n";
	}

	void GetHexDigitValueTest()
	{
		std::cout << "Get hex digit value test... ";

		const char32_t* digits = U"0123456789ABCDEF";
		const char32_t* digits2 = U"0123456789abcdef";

		for (std::size_t i = 0; i < (sizeof(digits) - sizeof(char32_t)) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::GetHexDigitValue(digits[i]) == (int)i);

		for (std::size_t i = 0; i < (sizeof(digits2) - sizeof(char32_t)) / sizeof(char32_t); ++i)
			assert(Xml::Encoding::CharactersReader::GetHexDigitValue(digits2[i]) == (int)i);

		assert(Xml::Encoding::CharactersReader::GetHexDigitValue(0x47) == -1);
		assert(Xml::Encoding::CharactersReader::GetHexDigitValue(0x67) == -1);

		std::cout << "OK\n";
	}

	void UnclosedTokenTest()
	{
		std::cout << "Unclosed token test... ";

		std::string docString = u8"<elem";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnclosedToken);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void StartElementTest()
	{
		std::cout << "Start element test... ";

		std::string docString = u8"<element>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"element");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"element");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void TagNameAfterSpaceTest()
	{
		std::cout << "Tag name after space test... ";
	
		std::string docString = u8"< afterspace>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidSyntax);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void InvalidTagNameStartCharTest()
	{
		std::cout << "Invalid tag name start char test... ";
	
		std::string docString = u8"<1name>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidTagName);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		docString = u8"<:name>";
		inspector.Reset(docString.begin(), docString.end());

		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidTagName);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void InvalidTagNameCharTest()
	{
		std::cout << "Invalid tag name char test... ";
	
		std::string docString = u8"<na^me>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidTagName);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		docString = u8"<name:>";
		inspector.Reset(docString.begin(), docString.end());

		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidTagName);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void WeirdTagNameStartCharTest()
	{
		std::cout << "Weird tag name start char test... ";
	
		std::string docString = u8"<^name>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::InvalidSyntax);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void ValidTagNameTest()
	{
		std::cout << "Valid tag name test... ";
	
		std::string docString = u8"<name123_abc>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"name123_abc");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"name123_abc");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void XmlElementPrefixTest()
	{
		std::cout << "xml element prefix test... ";
	
		std::string docString = u8"<xml:local>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"xml:local");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"local");
		assert(inspector.GetPrefix() == u8"xml");
		assert(inspector.GetNamespaceUri() == u8"http://www.w3.org/XML/1998/namespace");
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void XmlnsElementPrefixTest()
	{
		std::cout << "xmlns element prefix test... ";
	
		std::string docString = u8"<xmlns:local>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		// I don't want to create new error code just for this case,
		// I will use existing error code instead.
		assert(inspector.GetErrorCode() == Xml::ErrorCode::PrefixWithoutAssignedNamespace);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 2);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void UnexpectedEndTagTest()
	{
		std::cout << "Unexpected end tag test... ";

		std::string docString = u8"</unexpected>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		bool result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnexpectedEndTag);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		docString = u8"<a></unexpected>";
		inspector.Reset(docString.begin(), docString.end());

		// <a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// </unexpected>
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnexpectedEndTag);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void StartElementPlusEndElementTest()
	{
		std::cout << "Start element plus end element test... ";

		std::string docString = u8"<element></element>";
		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		// <element>
		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"element");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"element");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// </element>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"element");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"element");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 10);
		assert(inspector.GetDepth() == 0);

		// eof
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 20);
		assert(inspector.GetDepth() == 0);
	
		std::cout << "OK\n";
	}

	void WhitespacePlusElementTest()
	{
		std::cout << "Whitespace plus element test... ";

		unsigned char source[] =
		{
			0x0A, 0x0D, 0x20, 0x0D, 0x0A, 0x0D, 0x20, 0x09, // "\n\r \r\n\r \t"
			0x3C, 0x61, 0x3E,                               // "<a>"
			0x0A, 0x0D, 0x20, 0x0D, 0x0A, 0x0D, 0x20, 0x09, // "\n\r \r\n\r \t"
			0x3C, 0x2F, 0x61, 0x3E,                         // "</a>"
			0x0A, 0x0D, 0x20, 0x0D, 0x0A, 0x0D, 0x20, 0x09  // "\n\r \r\n\r \t"
		};
		// XML parsers must convert this content to:
		// "\n\n \n\n \t<a>\n\n \n\n \t</a>\n\n \n\n \t".

		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			source, source + sizeof(source));

		// White spaces.
		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::Whitespace);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue() == u8"\n\n \n\n \t");
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// <a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 5);
		assert(inspector.GetColumn() == 3);
		assert(inspector.GetDepth() == 0);

		// White spaces.
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::Whitespace);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue() == u8"\n\n \n\n \t");
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 5);
		assert(inspector.GetColumn() == 6);
		assert(inspector.GetDepth() == 1);

		// </a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 9);
		assert(inspector.GetColumn() == 3);
		assert(inspector.GetDepth() == 0);

		// White spaces.
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::Whitespace);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue() == u8"\n\n \n\n \t");
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 9);
		assert(inspector.GetColumn() == 7);
		assert(inspector.GetDepth() == 0);

		// eof
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 13);
		assert(inspector.GetColumn() == 3);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void TextPlusElementTest()
	{
		std::cout << "Text plus element test... ";

		std::string docString =
			u8"<element>\n  some !@#*(;:[>  text  \n</element>";

		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		// <element>
		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"element");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"element");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// Text.
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::Text);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue() == u8"\n  some !@#*(;:[>  text  \n");
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 10);
		assert(inspector.GetDepth() == 1);

		// </element>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"element");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"element");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 3);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// eof
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 3);
		assert(inspector.GetColumn() == 11);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}

	void UnclosedTagTest()
	{
		std::cout << "Unclosed tag test... ";

		std::string docString =
			u8"<a><b><c></c>";

		Xml::Inspector<Xml::Encoding::Utf8Writer> inspector(
			docString.begin(), docString.end());

		// <a>
		bool result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// <b>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"b");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"b");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 1);

		// <c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 7);
		assert(inspector.GetDepth() == 2);

		// </c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 10);
		assert(inspector.GetDepth() == 2);

		// UnclosedTag error.
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnclosedTag);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 0);

		docString = u8"<a><b><c></c>   ";

		inspector.Reset(docString.begin(), docString.end());

		// <a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// <b>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"b");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"b");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 1);

		// <c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 7);
		assert(inspector.GetDepth() == 2);

		// </c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 10);
		assert(inspector.GetDepth() == 2);

		// UnclosedTag error.
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnclosedTag);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 0);

		docString = u8"<a><b><c></c> abc  ";

		inspector.Reset(docString.begin(), docString.end());

		// <a>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"a");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"a");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 1);
		assert(inspector.GetDepth() == 0);

		// <b>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"b");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"b");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 1);

		// <c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::StartElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 7);
		assert(inspector.GetDepth() == 2);

		// </c>
		result = inspector.ReadNode();

		assert(result == true);
		assert(inspector.GetNodeType() == Xml::NodeType::EndElement);
		assert(inspector.GetName() == u8"c");
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName() == u8"c");
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 10);
		assert(inspector.GetDepth() == 2);

		// UnclosedTag error.
		result = inspector.ReadNode();

		assert(result == false);
		assert(inspector.GetNodeType() == Xml::NodeType::None);
		assert(inspector.GetName().empty());
		assert(inspector.GetValue().empty());
		assert(inspector.GetLocalName().empty());
		assert(inspector.GetPrefix().empty());
		assert(inspector.GetNamespaceUri().empty());
		assert(inspector.HasAttributes() == false);
		assert(inspector.GetAttributesCount() == 0);
		assert(inspector.GetAttributeBegin() == inspector.GetAttributeEnd());
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::UnclosedTag);
		assert(inspector.GetRow() == 1);
		assert(inspector.GetColumn() == 4);
		assert(inspector.GetDepth() == 0);

		std::cout << "OK\n";
	}
};

int main()
{
	Test test;
	test.Start();
	return EXIT_SUCCESS;
}
/// @endcond

