#include "test_array.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 3));
		f_test_erase_range(array, vector, 0, 3);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 4));
		f_test_erase_range(array, vector, 1, 2);
		f_assert_equals(array, {0, 3});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 4));
		f_test_erase_range(array, vector, 2, 2);
		f_assert_equals(array, {0, 1});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		assert(f_dump(array) == R"(
5
10
)"sv);
		f_test_erase_range(array, vector, 2, 2);
		f_assert_equals(array, {0, 1, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
3
8
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		// |=====| |****=| ...
		assert(f_dump(array) == R"(
5
10
)"sv);
		f_test_erase_range(array, vector, 5, 4);
		// |====|.| -> |==...| ...
		f_assert_equals(array, {0, 1, 2, 3, 4, 9, 10, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
4
6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		// |=====| |=****| ...
		assert(f_dump(array) == R"(
5
10
)"sv);
		f_test_erase_range(array, vector, 6, 4);
		// |====.| -> |==...| ...
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 10, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
4
6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 10));
		f_test_erase(array, vector, 0, 3);
		// |==...| |****=|
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_erase_range(array, vector, 2, 4);
		// |===..|
		f_assert_equals(array, {3, 4, 9});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 10));
		f_test_erase(array, vector, 0, 2);
		// |===..| |*****|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 3, 5);
		// |===..|
		f_assert_equals(array, {2, 3, 4});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 7));
		// |**=..| |====.|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 0, 2);
		// |==...| <- |===..|
		f_assert_equals(array, {2, 3, 4, 5, 6});
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		// |*****| |=====| ...
		assert(f_dump(array) == R"(
5
10
)"sv);
		f_test_erase_range(array, vector, 0, 5);
		// |==...| <- |===..| ...
		f_assert_equals(array, {5, 6, 7, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
2
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		f_test_erase(array, vector, 5, 1);
		// |**=..| |==...|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 0, 2);
		// |===..|
		f_assert_equals(array, {2, 3, 4});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		f_test_erase(array, vector, 5, 3);
		// |****=| |==...| ...
		assert(f_dump(array) == R"(
5
7
)"sv);
		f_test_erase_range(array, vector, 1, 4);
		// |===..| ...
		f_assert_equals(array, {0, 8, 9, 10, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
3
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		// |*****| |*****| |*====|
		assert(f_dump(array) == R"(
5
10
)"sv);
		f_test_erase_range(array, vector, 0, 11);
		// ...
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 35));
		//                    x
		//  x x       x               x x
		// ... |*****| |*****| |****=| ...
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
)"sv);
		f_test_erase_range(array, vector, 10, 14);
		//                        x
		//        x       x               x       x
		// |=====| |===..| |==...| |==...| |====.| |=====|
		assert(f_dump(array) == R"(
 5
 8
10
 2
 6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 31));
		f_test_erase(array, vector, 27, 1);
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |****=| |==...| |===..|
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 7
)"sv);
		f_test_erase_range(array, vector, 10, 14);
		//        x       x       x       x
		// |=====| |===..| |==...| |===..| |===..|
		assert(f_dump(array) == R"(
5
8
10
13
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 35));
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |*****| |*====| ...
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
)"sv);
		f_test_erase_range(array, vector, 10, 16);
		//  x x       x       x
		// ... |==...| |==...| ...
		assert(f_dump(array) == R"(
5
10
12
14
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// |==*..| |*==..|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 2, 2);
		// |==...| |==...|
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 7));
		// |=**..| |*===.|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 1, 3);
		// |==...| <- |==...|
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 8));
		// |***..| |*====|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 0, 4);
		// |==...| <- |==...|
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 9));
		// |===*.| |****=|
		assert(f_dump(array) == R"(
4
)"sv);
		f_test_erase_range(array, vector, 3, 5);
		// |==...| -> |==...|
		assert(f_dump(array) == R"(
2
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 10));
		// |====*| |****=|
		assert(f_dump(array) == R"(
5
)"sv);
		f_test_erase_range(array, vector, 4, 5);
		// |===..| -> |==...|
		assert(f_dump(array) == R"(
3
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// |=**..| |*==..|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 1, 3);
		// |===..|
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		f_test_erase(array, vector, 8, 4);
		// ... |=**..| |*==..| ...
		assert(f_dump(array) == R"(
5
8
11
)"sv);
		f_test_erase_range(array, vector, 6, 3);
		// ... |===..| ...
		assert(f_dump(array) == R"(
5
8
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// |***..| |**=..|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 0, 5);
		// |=....|
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// |=**..| |***..|
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_erase_range(array, vector, 1, 5);
		// |=....|
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		f_test_erase(array, vector, 8, 4);
		// |=====| |***..| |**=..| ...
		assert(f_dump(array) == R"(
5
8
11
)"sv);
		f_test_erase_range(array, vector, 5, 5);
		// |====.| -> |==...| ...
		assert(f_dump(array) == R"(
4
6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		f_test_erase(array, vector, 8, 4);
		// |=====| |=**..| |***..| ...
		assert(f_dump(array) == R"(
5
8
11
)"sv);
		f_test_erase_range(array, vector, 6, 5);
		// |====.| -> |==...| ...
		assert(f_dump(array) == R"(
4
6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 11));
		f_test_erase(array, vector, 0, 3);
		// |==...| |***..| |**=..|
		assert(f_dump(array) == R"(
2
5
)"sv);
		f_test_erase_range(array, vector, 2, 5);
		// |====.|
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 11));
		f_test_erase(array, vector, 0, 3);
		// |==...| |=**..| |***..|
		assert(f_dump(array) == R"(
2
5
)"sv);
		f_test_erase_range(array, vector, 3, 5);
		// |====.|
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		f_test_erase(array, vector, 3, 4);
		// |***..| |**=..| |=====| ...
		assert(f_dump(array) == R"(
3
6
11
)"sv);
		f_test_erase_range(array, vector, 0, 5);
		// |==...| <- |====.| ...
		assert(f_dump(array) == R"(
2
6
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 11));
		f_test_erase(array, vector, 3, 2);
		f_test_erase(array, vector, 8, 1);
		// |***..| |**=..| |==...|
		assert(f_dump(array) == R"(
3
6
)"sv);
		f_test_erase_range(array, vector, 0, 5);
		// |====.|
		assert(f_dump(array) == R"(
)"sv);
	}
	return 0;
}
