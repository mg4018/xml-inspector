#include "XmlInspector.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <streambuf>
#include <cstddef>
#include <algorithm>
#include <cassert>

// For test.
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
		Utf16StreamReaderTest();
		Utf32StreamReaderTest();
		Utf8IteratorsReaderTest();
		Utf8FinalEncodingTest();
		Utf16FinalEncodingTest();
		Utf32FinalEncodingTest();

		std::cout << "--END TEST--\n";
	}

	void Utf8StreamReaderTest()
	{
		std::cout << "UTF-8 stream reader test... ";

		// Memory buffer to istream.
		char source[] = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		MemBuf buf(source, sizeof(source) - 1);
		std::istream is(&buf);

		Xml::Utf8StreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf16StreamReaderTest()
	{
		std::cout << "UTF-16 stream reader test... ";

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

		Xml::Utf16StreamReader reader(&is);
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);

		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf32StreamReaderTest()
	{
		std::cout << "UTF-32 stream reader test... ";

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

		Xml::Utf32StreamReader reader(&is);
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
		Xml::Utf8IteratorsReader<std::string::const_iterator> reader(source.cbegin(), source.cend());
		char32_t c;
		std::u32string destination;
		int result;
		while ((result = reader.ReadCharacter(c)) == 1)
			destination.push_back(c);
		
		assert(result == 0);
		assert(destination == U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi");

		std::cout << "OK\n";
	}

	void Utf8FinalEncodingTest()
	{
		std::cout << "UTF-8 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::string destination;
		std::string pattern = u8"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Utf8FinalEncoding::PutCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf16FinalEncodingTest()
	{
		std::cout << "UTF-16 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u16string destination;
		std::u16string pattern = u"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Utf16FinalEncoding::PutCharacter(destination, *i);

		assert(destination == pattern);

		std::cout << "OK\n";
	}

	void Utf32FinalEncodingTest()
	{
		std::cout << "UTF-32 final encoding test... ";

		std::u32string source = U"abc def\U00000024\U000000A2\U000020AC\U00024B62ghi";
		std::u32string destination;
		for (auto i = source.cbegin(); i != source.cend(); ++i)
			Xml::Utf32FinalEncoding::PutCharacter(destination, *i);

		assert(destination == source);

		std::cout << "OK\n";
	}
};

int main()
{
	Test test;
	test.Start();
	return EXIT_SUCCESS;
}

