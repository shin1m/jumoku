#include "test_utf8.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 3));
		f_test_erase_range(utf8, vector, 0, 3);
		f_assert_equals(utf8, {});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 4));
		f_test_erase_range(utf8, vector, 1, 2);
		f_assert_equals(utf8, {0, 3});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 4));
		f_test_erase_range(utf8, vector, 2, 2);
		f_assert_equals(utf8, {0, 1});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 2, 2);
		f_assert_equals(utf8, {0, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(utf8) == R"(
(3, 3)
(8, 8)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		// |=====| |****=| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 5, 4);
		// |====|.| -> |==...| ...
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 9, 10, 11, 12, 13, 14});
		assert(f_dump(utf8) == R"(
(4, 4)
(6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		// |=====| |=****| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 6, 4);
		// |====.| -> |==...| ...
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5, 10, 11, 12, 13, 14});
		assert(f_dump(utf8) == R"(
(4, 4)
(6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 10));
		f_test_erase_range(utf8, vector, 0, 3);
		// |==...| |****=|
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_erase_range(utf8, vector, 2, 4);
		// |===..|
		f_assert_equals(utf8, {3, 4, 9});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 10));
		f_test_erase_range(utf8, vector, 0, 2);
		// |===..| |*****|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 3, 5);
		// |===..|
		f_assert_equals(utf8, {2, 3, 4});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 1));
		// |**=..| |====.|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 0, 2);
		// |==...| <- |===..|
		f_assert_equals(utf8, {2, 3, 4, 5, 6});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		// |*****| |=====| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 0, 5);
		// |==...| <- |===..| ...
		f_assert_equals(utf8, {5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(utf8) == R"(
(2, 2)
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_erase_range(utf8, vector, 5, 1);
		// |**=..| |==...|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 0, 2);
		// |===..|
		f_assert_equals(utf8, {2, 3, 4});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		f_test_erase_range(utf8, vector, 5, 3);
		// |****=| |==...| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(7, 7)
)"sv);
		f_test_erase_range(utf8, vector, 1, 4);
		// |===..| ...
		f_assert_equals(utf8, {0, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 15));
		// |*****| |*****| |*====|
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 0, 11);
		// ...
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 35));
		//                    x
		//  x x       x               x x
		// ... |*****| |*****| |****=| ...
		assert(f_dump(utf8) == R"(
 (5, 5)
 (10, 10)
 (15, 15)
(20, 20)
 (5, 5)
 (10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 10, 14);
		//                        x
		//        x       x               x       x
		// |=====| |===..| |==...| |==...| |====.| |=====|
		assert(f_dump(utf8) == R"(
 (5, 5)
 (8, 8)
(10, 10)
 (2, 2)
 (6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 31));
		f_test_erase_range(utf8, vector, 27, 1);
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |****=| |==...| |===..|
		assert(f_dump(utf8) == R"(
 (5, 5)
 (10, 10)
 (15, 15)
(20, 20)
 (5, 5)
 (7, 7)
)"sv);
		f_test_erase_range(utf8, vector, 10, 14);
		//        x       x       x       x
		// |=====| |===..| |==...| |===..| |===..|
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(10, 10)
(13, 13)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 35));
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |*****| |*====| ...
		assert(f_dump(utf8) == R"(
 (5, 5)
 (10, 10)
 (15, 15)
(20, 20)
 (5, 5)
 (10, 10)
)"sv);
		f_test_erase_range(utf8, vector, 10, 16);
		//  x x       x       x
		// ... |==...| |==...| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(12, 12)
(14, 14)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		// |==*..| |*==..|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 2, 2);
		// |==...| |==...|
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 1));
		// |=**..| |*===.|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 1, 3);
		// |==...| <- |==...|
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 2));
		// |***..| |*====|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 0, 4);
		// |==...| <- |==...|
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 8));
		f_test_insert_range(utf8, vector, 8, f_range(8, 1));
		// |===*.| |****=|
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
		f_test_erase_range(utf8, vector, 3, 5);
		// |==...| -> |==...|
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 10));
		// |====*| |****=|
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
		f_test_erase_range(utf8, vector, 4, 5);
		// |===..| -> |==...|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		// |=**..| |*==..|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 1, 3);
		// |===..|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		f_test_erase_range(utf8, vector, 8, 4);
		// ... |=**..| |*==..| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(11, 11)
)"sv);
		f_test_erase_range(utf8, vector, 6, 3);
		// ... |===..| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		// |***..| |**=..|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 0, 5);
		// |=....|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		// |=**..| |***..|
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_erase_range(utf8, vector, 1, 5);
		// |=....|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		f_test_erase_range(utf8, vector, 8, 4);
		// |=====| |***..| |**=..| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(11, 11)
)"sv);
		f_test_erase_range(utf8, vector, 5, 5);
		// |====.| -> |==...| ...
		assert(f_dump(utf8) == R"(
(4, 4)
(6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		f_test_erase_range(utf8, vector, 8, 4);
		// |=====| |=**..| |***..| ...
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(11, 11)
)"sv);
		f_test_erase_range(utf8, vector, 6, 5);
		// |====.| -> |==...| ...
		assert(f_dump(utf8) == R"(
(4, 4)
(6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 11));
		f_test_erase_range(utf8, vector, 0, 3);
		// |==...| |***..| |**=..|
		assert(f_dump(utf8) == R"(
(2, 2)
(5, 5)
)"sv);
		f_test_erase_range(utf8, vector, 2, 5);
		// |====.|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 11));
		f_test_erase_range(utf8, vector, 0, 3);
		// |==...| |=**..| |***..|
		assert(f_dump(utf8) == R"(
(2, 2)
(5, 5)
)"sv);
		f_test_erase_range(utf8, vector, 3, 5);
		// |====.|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		f_test_erase_range(utf8, vector, 3, 4);
		// |***..| |**=..| |=====| ...
		assert(f_dump(utf8) == R"(
(3, 3)
(6, 6)
(11, 11)
)"sv);
		f_test_erase_range(utf8, vector, 0, 5);
		// |==...| <- |====.| ...
		assert(f_dump(utf8) == R"(
(2, 2)
(6, 6)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 11));
		f_test_erase_range(utf8, vector, 3, 2);
		f_test_erase_range(utf8, vector, 8, 1);
		// |***..| |**=..| |==...|
		assert(f_dump(utf8) == R"(
(3, 3)
(6, 6)
)"sv);
		f_test_erase_range(utf8, vector, 0, 5);
		// |====.|
		assert(f_dump(utf8) == R"(
)"sv);
	}
	return 0;
}
