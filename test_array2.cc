#include "test_array.h"

int main(int argc, char* argv[])
{
	{
		xtree::t_array<int, 4, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0, 1, 2, 3});
		f_test_insert(array, vector, 2, {4});
		f_assert_equals(array, {0, 1, 4, 2, 3});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		xtree::t_array<int, 4, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0, 1, 2, 3});
		f_test_insert(array, vector, 3, {4});
		f_assert_equals(array, {0, 1, 2, 4, 3});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		xtree::t_array<int, 5, 4> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 25));
		//  i i i i
		// x X x x x
		assert(f_dump(array) == R"(
5
10
15
20
)");
		f_test_insert(array, vector, 5, f_range(25, 5));
		//      i
		//  i i   i i
		// x X X x x x
		assert(f_dump(array) == R"(
 5
 10
15
 5
 10
)");
	}
	{
		xtree::t_array<int, 5, 4> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 25));
		//  i i i i
		// x x X x x
		assert(f_dump(array) == R"(
5
10
15
20
)");
		f_test_insert(array, vector, 10, f_range(25, 3));
		//      i
		//  i i   i i
		// x x X X x x
		assert(f_dump(array) == R"(
 5
 10
15
 3
 8
)");
	}
	{
		xtree::t_array<int, 5, 4> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 25));
		//  i i i i
		// x x x X x
		assert(f_dump(array) == R"(
5
10
15
20
)");
		f_test_insert(array, vector, 15, f_range(25, 5));
		//      i
		//  i i   i i
		// x x x X X x
		assert(f_dump(array) == R"(
 5
 10
15
 5
 10
)");
	}
	return 0;
}
