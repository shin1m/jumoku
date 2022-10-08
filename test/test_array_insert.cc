#include "test_array.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert_range(array, vector, 0, f_range(0, 2));
		f_test_insert_range(array, vector, 2, f_range(2, 3));
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		f_assert_equals(array, {0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 8));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 3, f_range(8, 1));
		// |=   N    =| |=  N  =|
		// |=n0+i=|*|.| |=n-i=|.|
		f_assert_equals(array, {1, 2, 3, 8, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
4
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 8));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 3, f_range(8, 2));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |=n-i=|.|
		f_assert_equals(array, {1, 2, 3, 8, 9, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 7));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 4, f_range(7, 2));
		// |=  N   =| |=   N   =|
		// |=n0+i=|*| |*|=n-i=|.|
		//               <<<<<
		f_assert_equals(array, {1, 2, 3, 4, 7, 8, 5, 6});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 3, f_range(6, 5));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |*|=n-i=|
		//               >>>>>
		f_assert_equals(array, {1, 2, 3, 6, 7, 8, 9, 10, 4, 5});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 7));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 5, f_range(7, 2));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          <<<<<
		f_assert_equals(array, {1, 2, 3, 4, 5, 7, 8, 6});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 8));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(array) == R"(
2
)"sv);
		f_test_insert_range(array, vector, 6, f_range(8, 1));
		// |=N=| |=     N      =|
		// |=N=| |=i'=|*|=n-i=|.|
		//               <<<<<
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 8, 7});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 6));
		f_assert_equals(array, {0, 1, 2, 3, 4, 5});
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_insert_range(array, vector, 5, f_range(6, 3));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          >>>>>
		f_assert_equals(array, {0, 1, 2, 3, 4, 6, 7, 8, 5});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 10));
		f_test_erase(array, vector, 0, 1);
		f_test_erase(array, vector, 8, 1);
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 7, 8});
		assert(f_dump(array) == R"(
4
)"sv);
		f_test_insert_range(array, vector, 6, f_range(10, 2));
		// |=N=| |=    N     =|
		// |=N=| |=i'=|*|=n-i=|
		//               >>>>>
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 10, 11, 7, 8});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 0, {5, 6});
		f_assert_equals(array, {5, 6, 0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
4
)"sv);
		f_test_insert_range(array, vector, 2, f_range(7, 2));
		// |=  N  =| |=  N   =|
		// |=i=|*|=n-i=|=n1=|.|
		f_assert_equals(array, {5, 6, 7, 8, 0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 11));
		f_test_erase(array, vector, 0, 1);
		f_test_insert(array, vector, 6, {11});
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 11, 7, 8, 9, 10});
		assert(f_dump(array) == R"(
4
8
)"sv);
		f_test_insert_range(array, vector, 6, f_range(12, 4));
		// |=N=| |= N  =| |=   N    =|
		// |=N=| |=i'=|*| |=n-i=|=n1=|
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 12, 13, 14, 15, 11, 7, 8, 9, 10});
		assert(f_dump(array) == R"(
5
10
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 0, {5, 6});
		f_test_erase(array, vector, 6, 1);
		f_assert_equals(array, {5, 6, 0, 1, 2, 3});
		assert(f_dump(array) == R"(
4
)"sv);
		f_test_insert_range(array, vector, 3, f_range(7, 4));
		// |= N =| |=    N     =|
		// |=i=|*****|=n-i=|=n1=|
		f_assert_equals(array, {5, 6, 0, 7, 8, 9, 10, 1, 2, 3});
		assert(f_dump(array) == R"(
5
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 11));
		f_test_erase(array, vector, 0, 1);
		f_test_erase(array, vector, 9, 1);
		f_test_insert(array, vector, 6, {11});
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 11, 7, 8, 9});
		assert(f_dump(array) == R"(
4
8
)"sv);
		f_test_insert_range(array, vector, 7, f_range(12, 4));
		// |=N=| |= N  =| |=     N      =|
		// |=N=| |=i'=|*****|=n-i=|=n1=|.|
		f_assert_equals(array, {1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 7, 8, 9});
		assert(f_dump(array) == R"(
5
10
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_assert_equals(array, {0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 1, f_range(5, 2));
		// |=  N  =| |=N=|
		// |=i=|*|=n-i=|.|
		f_assert_equals(array, {0, 5, 6, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
4
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 10));
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
		assert(f_dump(array) == R"(
5
)"sv);
		f_test_insert_range(array, vector, 6, f_range(10, 1));
		// |=N=| |=N=| |=N=|
		// |=N=| |*|=n-i=|.|
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 10, 6, 7, 8, 9});
		assert(f_dump(array) == R"(
5
8
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 4));
		f_assert_equals(array, {0, 1, 2, 3});
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 2, f_range(4, 4));
		// |= N =| |=   N   =|
		// |=i=|*****|=n-i=|.|
		f_assert_equals(array, {0, 1, 4, 5, 6, 7, 2, 3});
		assert(f_dump(array) == R"(
4
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 7));
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 6});
		assert(f_dump(array) == R"(
3
)"sv);
		f_test_insert_range(array, vector, 6, f_range(7, 4));
		// |=N=| |= N  =| |=   N   =|
		// |=N=| |=i'=|*****|=n-i=|.|
		f_assert_equals(array, {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 6});
		assert(f_dump(array) == R"(
5
8
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 4));
		f_assert_equals(array, {0, 1, 2, 3});
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 3, f_range(4, 2));
		// |= N =| |=   N   =|
		// |=i=|.| |*|=n-i=|.|
		f_assert_equals(array, {0, 1, 2, 4, 5, 3});
		assert(f_dump(array) == R"(
3
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_assert_equals(array, {0, 1, 2, 3, 4});
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 5, f_range(5, 1));
		// |=N=| |= N =|
		// |=  i  =|*|.|
		f_assert_equals(array, {0, 1, 2, 3, 4, 5});
		assert(f_dump(array) == R"(
3
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		f_test_erase(array, vector, 0, 1);
		assert(f_dump(array) == R"(
4
9
14
)"sv);
		f_test_insert_range(array, vector, 8, f_range(20, 2));
		// |=N=| |= N  =| |=   N   =| |=N=| |=N=|
		// |=N=| |=i'=|.| |*|=n-i=|.| |=N=| |=N=|
		assert(f_dump(array) == R"(
5
8
11
16
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 20));
		assert(f_dump(array) == R"(
5
10
15
)"sv);
		f_test_insert_range(array, vector, 9, f_range(20, 1));
		// |=N=| |=N=| |=    N    =| |=N=| |=N=|
		// |=N=| |=  i  =|*|=n-i=|.| |=N=| |=N=|
		assert(f_dump(array) == R"(
5
8
11
16
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 2, f_range(5, 6));
		// |= N =| |=N=| |=  N  =|
		// |=i=|*******| |=n-i=|.|
		assert(f_dump(array) == R"(
5
8
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 2, f_range(5, 13));
		// |= N =| |=N=| |=N=| |=   N   =|
		// |=i=|*****************|=n-i=|.|
		assert(f_dump(array) == R"(
5
10
14
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert_range(array, vector, 0, f_range(0, 6));
		f_assert_equals(array, {0, 1, 2, 3, 4, 5});
		assert(f_dump(array) == R"(
3
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert_range(array, vector, 0, f_range(0, 30));
		assert(f_dump(array) == R"(
5
10
15
20
25
)"sv);
		f_test_insert_range(array, vector, 15, f_range(30, 6));
		assert(f_dump(array) == R"(
 5
 10
 15
20
 3
 6
 11
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert_range(array, vector, 0, f_range(0, 5));
		assert(f_dump(array) == R"(
)"sv);
		f_test_insert_range(array, vector, 5, f_range(5, 15));
		assert(f_dump(array) == R"(
5
10
15
)"sv);
	}
	{
		jumoku::t_array<int, 5, 5> array;
		std::vector<int> vector;
		f_test_insert_range(array, vector, 0, f_range(0, 1000));
	}
	return 0;
}
