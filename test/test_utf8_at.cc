#include "test_utf8.h"

int main(int argc, char* argv[])
{
	auto at = [](auto& utf8, size_t i)
	{
		return utf8.f_at(i, jumoku::t_utf8_traits::t_in_characters()).f_index();
	};
	auto index = [](size_t i0, size_t i1)
	{
		return jumoku::t_utf8_traits::t_index{i0, i1};
	};
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, {
			0xc2, 0xa3,
			0xe0, 0xa4, 0xb9,
			0xe2, 0x82, 0xac,
			0xc2, 0xa3
		});
		assert(f_dump(utf8) == R"(
(5, 2)
)"sv);
		assert(at(utf8, 0) == index(0, 0));
		assert(at(utf8, 1) == index(2, 1));
		assert(at(utf8, 2) == index(5, 2));
		assert(at(utf8, 3) == index(8, 3));
		assert(at(utf8, 4) == index(10, 4));
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, {
			0x24,
			0xc2, 0xa3,
			0xe0, 0xa4, 0xb9,
			0xe2, 0x82, 0xac,
			0x24
		});
		assert(f_dump(utf8) == R"(
(5, 3)
)"sv);
		assert(at(utf8, 0) == index(0, 0));
		assert(at(utf8, 1) == index(1, 1));
		assert(at(utf8, 2) == index(3, 2));
		assert(at(utf8, 3) == index(6, 3));
		assert(at(utf8, 4) == index(9, 4));
		assert(at(utf8, 5) == index(10, 5));
	}
	return 0;
}
