#include "test_utf8.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 2));
		f_test_insert_range(utf8, vector, 2, f_range(2, 3));
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		f_assert_equals(utf8, {0, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 2));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(8, 1));
		// |=   N    =| |=  N  =|
		// |=n0+i=|*|.| |=n-i=|.|
		f_assert_equals(utf8, {1, 2, 3, 8, 4, 5, 6, 7});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 2));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(8, 2));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |=n-i=|.|
		f_assert_equals(utf8, {1, 2, 3, 8, 9, 4, 5, 6, 7});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 1));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 4, f_range(7, 2));
		// |=  N   =| |=   N   =|
		// |=n0+i=|*| |*|=n-i=|.|
		//               <<<<<
		f_assert_equals(utf8, {1, 2, 3, 4, 7, 8, 5, 6});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(6, 5));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |*|=n-i=|
		//               >>>>>
		f_assert_equals(utf8, {1, 2, 3, 6, 7, 8, 9, 10, 4, 5});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 1));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 5, f_range(7, 2));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          <<<<<
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 7, 8, 6});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 2));
		f_test_erase_range(utf8, vector, 0, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 7});
		assert(f_dump(utf8) == R"(
(2, 2)
)"sv);
		f_test_insert_range(utf8, vector, 6, f_range(8, 1));
		// |=N=| |=     N      =|
		// |=N=| |=i'=|*|=n-i=|.|
		//               <<<<<
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 8, 7});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_insert_range(utf8, vector, 5, f_range(6, 3));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          >>>>>
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 6, 7, 8, 5});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 10));
		f_test_erase_range(utf8, vector, 0, 1);
		f_test_erase_range(utf8, vector, 8, 1);
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 7, 8});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
		f_test_insert_range(utf8, vector, 6, f_range(10, 2));
		// |=N=| |=    N     =|
		// |=N=| |=i'=|*|=n-i=|
		//               >>>>>
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 10, 11, 7, 8});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		f_test_insert_range(utf8, vector, 0, f_range(5, 2));
		f_assert_equals(utf8, {5, 6, 0, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
		f_test_insert_range(utf8, vector, 2, f_range(7, 2));
		// |=  N  =| |=  N   =|
		// |=i=|*|=n-i=|=n1=|.|
		f_assert_equals(utf8, {5, 6, 7, 8, 0, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 11));
		f_test_erase_range(utf8, vector, 0, 1);
		f_test_insert_range(utf8, vector, 6, f_range(11, 1));
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 11, 7, 8, 9, 10});
		assert(f_dump(utf8) == R"(
(4, 4)
(8, 8)
)"sv);
		f_test_insert_range(utf8, vector, 6, f_range(12, 4));
		// |=N=| |= N  =| |=   N    =|
		// |=N=| |=i'=|*| |=n-i=|=n1=|
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 12, 13, 14, 15, 11, 7, 8, 9, 10});
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		f_test_insert_range(utf8, vector, 0, f_range(5, 2));
		f_test_erase_range(utf8, vector, 6, 1);
		f_assert_equals(utf8, {5, 6, 0, 1, 2, 3});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(7, 4));
		// |= N =| |=    N     =|
		// |=i=|*****|=n-i=|=n1=|
		f_assert_equals(utf8, {5, 6, 0, 7, 8, 9, 10, 1, 2, 3});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 11));
		f_test_erase_range(utf8, vector, 0, 1);
		f_test_erase_range(utf8, vector, 9, 1);
		f_test_insert_range(utf8, vector, 6, f_range(11, 1));
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 11, 7, 8, 9});
		assert(f_dump(utf8) == R"(
(4, 4)
(8, 8)
)"sv);
		f_test_insert_range(utf8, vector, 7, f_range(12, 4));
		// |=N=| |= N  =| |=     N      =|
		// |=N=| |=i'=|*****|=n-i=|=n1=|.|
		f_assert_equals(utf8, {1, 2, 3, 4, 5, 6, 11, 12, 13, 14, 15, 7, 8, 9});
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		f_assert_equals(utf8, {0, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 1, f_range(5, 2));
		// |=  N  =| |=N=|
		// |=i=|*|=n-i=|.|
		f_assert_equals(utf8, {0, 5, 6, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 10));
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});
		assert(f_dump(utf8) == R"(
(5, 5)
)"sv);
		f_test_insert_range(utf8, vector, 6, f_range(10, 1));
		// |=N=| |=N=| |=N=|
		// |=N=| |*|=n-i=|.|
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5, 10, 6, 7, 8, 9});
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 4));
		f_assert_equals(utf8, {0, 1, 2, 3});
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 2, f_range(4, 4));
		// |= N =| |=   N   =|
		// |=i=|*****|=n-i=|.|
		f_assert_equals(utf8, {0, 1, 4, 5, 6, 7, 2, 3});
		assert(f_dump(utf8) == R"(
(4, 4)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_test_insert_range(utf8, vector, 6, f_range(6, 1));
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5, 6});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
		f_test_insert_range(utf8, vector, 6, f_range(7, 4));
		// |=N=| |= N  =| |=   N   =|
		// |=N=| |=i'=|*****|=n-i=|.|
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 6});
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 4));
		f_assert_equals(utf8, {0, 1, 2, 3});
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(4, 2));
		// |= N =| |=   N   =|
		// |=i=|.| |*|=n-i=|.|
		f_assert_equals(utf8, {0, 1, 2, 4, 5, 3});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		f_assert_equals(utf8, {0, 1, 2, 3, 4});
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 5, f_range(5, 1));
		// |=N=| |= N =|
		// |=  i  =|*|.|
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		f_test_erase_range(utf8, vector, 0, 1);
		assert(f_dump(utf8) == R"(
(4, 4)
(9, 9)
(14, 14)
)"sv);
		f_test_insert_range(utf8, vector, 8, f_range(20, 2));
		// |=N=| |= N  =| |=   N   =| |=N=| |=N=|
		// |=N=| |=i'=|.| |*|=n-i=|.| |=N=| |=N=|
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(11, 11)
(16, 16)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 20));
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(15, 15)
)"sv);
		f_test_insert_range(utf8, vector, 9, f_range(20, 1));
		// |=N=| |=N=| |=    N    =| |=N=| |=N=|
		// |=N=| |=  i  =|*|=n-i=|.| |=N=| |=N=|
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
(11, 11)
(16, 16)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 2, f_range(5, 6));
		// |= N =| |=N=| |=  N  =|
		// |=i=|*******| |=n-i=|.|
		assert(f_dump(utf8) == R"(
(5, 5)
(8, 8)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 2, f_range(5, 13));
		// |= N =| |=N=| |=N=| |=   N   =|
		// |=i=|*****************|=n-i=|.|
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(14, 14)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 6));
		f_assert_equals(utf8, {0, 1, 2, 3, 4, 5});
		assert(f_dump(utf8) == R"(
(3, 3)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 30));
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(15, 15)
(20, 20)
(25, 25)
)"sv);
		f_test_insert_range(utf8, vector, 15, f_range(30, 6));
		assert(f_dump(utf8) == R"(
 (5, 5)
 (10, 10)
 (15, 15)
(20, 20)
 (3, 3)
 (6, 6)
 (11, 11)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 5, f_range(5, 15));
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(15, 15)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 5));
		assert(f_dump(utf8) == R"(
)"sv);
		f_test_insert_range(utf8, vector, 3, f_range(5, 15));
		assert(f_dump(utf8) == R"(
(5, 5)
(10, 10)
(15, 15)
)"sv);
	}
	{
		jumoku::t_utf8<5, 5> utf8;
		std::vector<uint8_t> vector;
		f_test_insert_range(utf8, vector, 0, f_range(0, 1000));
	}
	return 0;
}
