#include "test_utf8.h"
#include <jumoku/utf32.h>
#include <regex>

int main(int argc, char* argv[])
{
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, {
			0x24,
			0xc2, 0xa3,
			0xe0, 0xa4, 0xb9,
			0xe2, 0x82, 0xac,
			0xf0, 0x90, 0x8d, 0x88,
			0xf0, 0x90, 0x90, 0xb7
		});
		auto i = jumoku::f_utf16(utf8.f_begin());
		auto j = jumoku::f_utf16(utf8.f_end());
		assert(i != j);
		assert(*i == L'$');
		assert(++i != j);
		assert(*i == L'\u00a3');
		assert(++i != j);
		assert(*i == L'\u0939');
		assert(++i != j);
		assert(*i == L'\u20ac');
		assert(++i != j);
		assert(*i == 0xd800);
		assert(++i != j);
		assert(*i == 0xdf48);
		assert(++i != j);
		assert(*i == 0xd801);
		assert(++i != j);
		assert(*i == 0xdc37);
		assert(++i == j);
		i = jumoku::f_utf16(utf8.f_begin());
		assert(j != i);
		assert(*--j == 0xdc37);
		assert(j != i);
		assert(*--j == 0xd801);
		assert(j != i);
		assert(*--j == 0xdf48);
		assert(j != i);
		assert(*--j == 0xd800);
		assert(j != i);
		assert(*--j == L'\u20ac');
		assert(j != i);
		assert(*--j == L'\u0939');
		assert(j != i);
		assert(*--j == L'\u00a3');
		assert(j != i);
		assert(*--j == L'$');
		assert(j == i);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, {
			0x24,
			0xc2, 0xa3,
			0xe0, 0xa4, 0xb9,
			0xe2, 0x82, 0xac,
			0x31,
			0x30,
			0xf0, 0x90, 0x8d, 0x88
		});
		std::wregex pattern(L"\u20ac\\d+");
		auto i = jumoku::f_utf16(utf8.f_begin());
		auto j = jumoku::f_utf16(utf8.f_end());
		std::match_results<decltype(i)> match;
		assert(std::regex_search(i, j, match, pattern));
		assert(match.size() == 1);
		assert(match[0].first == utf8.f_at(6));
		assert(match[0].second == utf8.f_at(11));
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		{
			auto i = jumoku::f_utf8_inserter<5, 5, 4>(utf8, utf8.f_end());
			std::copy_n(L"$\u00a3\u0939\u20ac\xd800\xdf48\xd801\xdc37", 8, jumoku::f_utf16_output(i));
		}
		f_assert_equals(utf8, {
			0x24,
			0xc2, 0xa3,
			0xe0, 0xa4, 0xb9,
			0xe2, 0x82, 0xac,
			0xf0, 0x90, 0x8d, 0x88,
			0xf0, 0x90, 0x90, 0xb7
		});
	}
	return 0;
}
