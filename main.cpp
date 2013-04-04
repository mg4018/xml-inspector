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
		Utf8CharactersWriterTest();
		Utf16CharactersWriterTest();
		Utf32CharactersWriterTest();
		InspectorConstructorsTest();
		InspectorResetTest();
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

	void Utf8CharactersWriterTest()
	{
		std::cout << "UTF-8 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::string destination;
		std::string pattern = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf8CharactersWriter::PutCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf16CharactersWriterTest()
	{
		std::cout << "UTF-16 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u16string destination;
		std::u16string pattern = u"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf16CharactersWriter::PutCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf32CharactersWriterTest()
	{
		std::cout << "UTF-32 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u32string destination;
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Encoding::Utf32CharactersWriter::PutCharacter(destination, *i);

		assert(destination == source);

		std::cout << "OK\n";
	}

	void InspectorConstructorsTest()
	{
		std::cout << "Inspector constructors test... ";

		// Defaul constructor.
		Xml::Inspector<Xml::Encoding::Utf32CharactersWriter> i0;

		// File path from const char*
		Xml::Inspector<Xml::Encoding::Utf8CharactersWriter> i1("test.xml");

		// File path from const std::string&
		std::string path = "test.xml";
		Xml::Inspector<Xml::Encoding::Utf16CharactersWriter> i2(path);

		// From input stream.
		std::ifstream ifs("test.xml");
		Xml::Inspector<Xml::Encoding::Utf32CharactersWriter> i3(&ifs);
	
		// From bytes.
		char bytes[] = u8"<root>bytes test</root>";
		Xml::Inspector<Xml::Encoding::Utf16CharactersWriter> i4(bytes, bytes + sizeof(bytes));

		// From reader interface.
		std::string content = u8"<root>abc</root>";
		Xml::Encoding::Utf8IteratorsReader<std::string::const_iterator> reader(content.cbegin(), content.cend());
		Xml::Inspector<Xml::Encoding::Utf32CharactersWriter> i5(&reader);

		std::cout << "OK\n";
	}

	void InspectorResetTest()
	{
		std::cout << "Inspector reset test... ";

		Xml::Inspector<Xml::Encoding::Utf32CharactersWriter> inspector;

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

	void BeforeParsingTest()
	{
		std::cout << "Before parsing test... ";

		Xml::Inspector<Xml::Encoding::Utf16CharactersWriter> inspector("test.xml");

		assert(inspector.GetErrorMessage() == nullptr);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::None);
		assert(inspector.GetNodeType() == Xml::NodeType::None);

		std::cout << "OK\n";
	}

	void NoSourceTest()
	{
		std::cout << "No source test... ";

		Xml::Inspector<Xml::Encoding::Utf16CharactersWriter> inspector;
		bool foundNode = inspector.ReadNode();

		assert(!foundNode);
		assert(inspector.GetErrorCode() == Xml::ErrorCode::StreamError);
		assert(inspector.GetErrorMessage() != nullptr);
		assert(inspector.GetLineNumber() == 0);
		assert(inspector.GetLinePosition() == 0);
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
};

int main()
{
	Test test;
	test.Start();
	return EXIT_SUCCESS;
}
/// @endcond

