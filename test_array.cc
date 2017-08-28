#include "test_array.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		assert(array.f_empty());
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
		f_test_insert(array, vector, 0, {0});
		assert(!array.f_empty());
		f_test_insert(array, vector, 0, {1, 2});
		f_test_insert(array, vector, 3, {3});
		f_test_insert(array, vector, 2, {4});
		f_test_insert(array, vector, 2, {5});
		f_assert_equals(array, {1, 2, 5, 4, 0, 3});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 2, {5});
		f_assert_equals(array, {0, 1, 5, 2, 3, 4});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 3, {5});
		f_assert_equals(array, {0, 1, 2, 5, 3, 4});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		// xxxxx
		f_assert_equals(array, {0, 1, 2, 3, 4});
		f_test_insert(array, vector, 0, f_range(5, 5));
		// xxxxx xxxxx
		f_assert_equals(array, {5, 6, 7, 8, 9, 0, 1, 2, 3, 4});
		f_test_insert(array, vector, 0, f_range(10, 5));
		// xxxxx xxxxx xxxxx
		f_assert_equals(array, {10, 11, 12, 13, 14, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4});
		f_test_insert(array, vector, 8, {15});
		// xxxxx xxx xxx xxxxx
		f_assert_equals(array, {10, 11, 12, 13, 14, 5, 6, 7, 15, 8, 9, 0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
5
8
11
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 8));
		// xxx xxxxx
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7});
		f_test_insert(array, vector, 3, {8});
		// xxxx xxxxx
		f_assert_equals(array, {0, 1, 2, 8, 3, 4, 5, 6, 7});
		f_test_insert(array, vector, 5, {9});
		// xxxxx xxxxx
		f_assert_equals(array, {0, 1, 2, 8, 3, 9, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
5
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 30));
		//  i i i i i
		// x x X x x x
		assert(f_dump(array) == R"(
5
10
15
20
25
)");
		f_test_insert(array, vector, 10, f_range(30, 5));
		//        i
		//  i i i   i i
		// x x X X x x x
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 30, 31, 32, 33, 34, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29});
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 30));
		//  i i i i i
		// x x x X x x
		f_test_insert(array, vector, 15, f_range(30, 3));
		//        i
		//  i i i   i i
		// x x x X X x x
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 30, 31, 32, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29});
		assert(f_dump(array) == R"(
 5
 10
 15
20
 3
 8
)");
		f_test_insert(array, vector, 18, {33});
		//          i
		//  i i i i   i i
		// x x x X X x x x
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 30, 31, 32, 33, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29});
		assert(f_dump(array) == R"(
 5
 10
 15
 18
21
 3
 8
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 30));
		//  i i i i i
		// x x x x X x
		assert(f_dump(array) == R"(
5
10
15
20
25
)");
		f_test_insert(array, vector, 20, f_range(30, 5));
		//        i
		//  i i i   i i
		// x x x x X X x
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 30, 31, 32, 33, 34, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29});
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0});
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0, 1});
		f_test_erase(array, vector, 1, 1);
		f_assert_equals(array, {0});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// xxx xxx
		assert(f_dump(array) == R"(
3
)");
		f_test_erase(array, vector, 1, 1);
		// xx xxx
		f_assert_equals(array, {0, 2, 3, 4, 5});
		assert(f_dump(array) == R"(
2
)");
		f_test_erase(array, vector, 1, 1);
		// xx xx
		f_assert_equals(array, {0, 3, 4, 5});
		assert(f_dump(array) == R"(
2
)");
		f_test_erase(array, vector, 1, 1);
		// xxx
		f_assert_equals(array, {0, 4, 5});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		// xxx xxx
		assert(f_dump(array) == R"(
3
)");
		f_test_erase(array, vector, 3, 1);
		// xxx xx
		f_assert_equals(array, {0, 1, 2, 4, 5});
		assert(f_dump(array) == R"(
3
)");
		f_test_erase(array, vector, 3, 1);
		// xx xx
		f_assert_equals(array, {0, 1, 2, 5});
		assert(f_dump(array) == R"(
2
)");
		f_test_erase(array, vector, 3, 1);
		// xxx
		f_assert_equals(array, {0, 1, 2});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 15));
		// xxxxx xxxxx xxxxx
		assert(f_dump(array) == R"(
5
10
)");
		f_test_erase(array, vector, 3, 8);
		// xx xx xxx
		f_assert_equals(array, {0, 1, 2, 11, 12, 13, 14});
		assert(f_dump(array) == R"(
2
4
)");
		f_test_erase(array, vector, 3, 1);
		// xx xx xx
		f_assert_equals(array, {0, 1, 2, 12, 13, 14});
		assert(f_dump(array) == R"(
2
4
)");
		f_test_erase(array, vector, 3, 1);
		// xx xxx
		f_assert_equals(array, {0, 1, 2, 13, 14});
		assert(f_dump(array) == R"(
2
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 40));
		//        i
		//  i i i   i i i
		// X x x x x x x x
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
 15
)");
		f_test_erase(array, vector, 0, 7);
		//      i
		//  i i   i i i
		// X x x x x x x
		assert(f_dump(array) == R"(
 3
 8
13
 5
 10
 15
)");
		f_test_erase(array, vector, 0, 5);
		//      i
		//  i i   i i
		// X x x x x x
		assert(f_dump(array) == R"(
 3
 8
13
 5
 10
)");
		f_test_erase(array, vector, 0, 5);
		//  i i i i
		// x x x x x
		assert(f_dump(array) == R"(
3
8
13
18
)");
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 35));
		//        i
		//  i i i   i i
		// x x x x x X x
		assert(f_dump(array) == R"(
 5
 10
 15
20
 5
 10
)");
		f_test_erase(array, vector, 23, 9);
		//                        i
		//      i     i     i        i  i
		// xxxxx xxxxx xxxxx xxxxx xx xX xx
		assert(f_dump(array) == R"(
 5
 10
 15
20
 2
 4
)");
		f_test_erase(array, vector, 23, 1);
		//                  i
		//      i     i           i  i
		// xxxxx xxxxx xxxxx xxxxx xx xXX
		assert(f_dump(array) == R"(
 5
 10
15
 5
 7
)");
		f_test_erase(array, vector, 23, 2);
		//      i     i     i     i
		// xxxxx xxxxx xxxxx xxxxx xxx
		std::cout << f_dump(array) << std::endl;
		assert(f_dump(array) == R"(
5
10
15
20
)");
	}
	return 0;
}
