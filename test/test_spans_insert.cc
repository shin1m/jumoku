#include "test_spans.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert_range(array, vector, 0, f_spans(0, 2));
		f_test_insert_range(array, vector, 2, f_spans(2, 3));
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 8));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17}
		});
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 3, f_spans(8, 1));
		// |=   N    =| |=  N  =|
		// |=n0+i=|*|.| |=n-i=|.|
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{8, 18},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17}
		});
		assert(f_dump(array) == R"(
(4, 54)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 8));
		f_test_erase(array, vector, 0, 1);
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 3, f_spans(8, 2));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |=n-i=|.|
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{8, 18},
			t_span{9, 19},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17}
		});
		assert(f_dump(array) == R"(
(5, 73)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 7));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16}
		});
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 4, f_spans(7, 2));
		// |=  N   =| |=   N   =|
		// |=n0+i=|*| |*|=n-i=|.|
		//               <<<<<
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{7, 17},
			t_span{8, 18},
			t_span{5, 15},
			t_span{6, 16}
		});
		assert(f_dump(array) == R"(
(5, 67)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 3, f_spans(6, 5));
		// |=  N   =| |=  N  =|
		// |=n0+i=|*| |*|=n-i=|
		//               >>>>>
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(5, 69)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 7));
		f_test_erase(array, vector, 0, 1);
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 5, f_spans(7, 2));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          <<<<<
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{7, 17},
			t_span{8, 18},
			t_span{6, 16}
		});
		assert(f_dump(array) == R"(
(5, 65)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 8));
		f_test_erase(array, vector, 0, 1);
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_insert_range(array, vector, 6, f_spans(8, 1));
		// |=N=| |=     N      =|
		// |=N=| |=i'=|*|=n-i=|.|
		//               <<<<<
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{8, 18},
			t_span{7, 17}
		});
		assert(f_dump(array) == R"(
(5, 65)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_insert_range(array, vector, 5, f_spans(6, 3));
		// |=N=| |=   N   =|
		// |=N=| |*|=n-i=|.|
		//          >>>>>
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(5, 60)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 10));
		f_test_erase(array, vector, 0, 1);
		f_test_erase(array, vector, 8, 1);
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18}
		});
		assert(f_dump(array) == R"(
(4, 50)
)");
		f_test_insert_range(array, vector, 6, f_spans(10, 2));
		// |=N=| |=    N     =|
		// |=N=| |=i'=|*|=n-i=|
		//               >>>>>
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{10, 20},
			t_span{11, 21},
			t_span{7, 17},
			t_span{8, 18}
		});
		assert(f_dump(array) == R"(
(5, 65)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		f_test_insert(array, vector, 0, f_spans(5, 2));
		f_assert_equals(array, {
			t_span{5, 15},
			t_span{6, 16},
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(4, 52)
)");
		f_test_insert_range(array, vector, 2, f_spans(7, 2));
		// |=  N  =| |=  N   =|
		// |=i=|*|=n-i=|=n1=|.|
		f_assert_equals(array, {
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(5, 76)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 11));
		f_test_erase(array, vector, 0, 1);
		f_test_insert(array, vector, 6, f_spans(11, 1));
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{11, 21},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20}
		});
		assert(f_dump(array) == R"(
(4, 50)
(8, 119)
)");
		f_test_insert_range(array, vector, 6, f_spans(12, 4));
		// |=N=| |= N  =| |=   N    =|
		// |=N=| |=i'=|*| |=n-i=|=n1=|
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{11, 21},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20}
		});
		assert(f_dump(array) == R"(
(5, 65)
(10, 175)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		f_test_insert(array, vector, 0, f_spans(5, 2));
		f_test_erase(array, vector, 6, 1);
		f_assert_equals(array, {
			t_span{5, 15},
			t_span{6, 16},
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
(4, 52)
)");
		f_test_insert_range(array, vector, 3, f_spans(7, 4));
		// |= N =| |=    N     =|
		// |=i=|*****|=n-i=|=n1=|
		f_assert_equals(array, {
			t_span{5, 15},
			t_span{6, 16},
			t_span{0, 10},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
(5, 76)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 11));
		f_test_erase(array, vector, 0, 1);
		f_test_erase(array, vector, 9, 1);
		f_test_insert(array, vector, 6, f_spans(11, 1));
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{11, 21},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19}
		});
		assert(f_dump(array) == R"(
(4, 50)
(8, 119)
)");
		f_test_insert_range(array, vector, 7, f_spans(12, 4));
		// |=N=| |= N  =| |=     N      =|
		// |=N=| |=i'=|*****|=n-i=|=n1=|.|
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19}
		});
		assert(f_dump(array) == R"(
(5, 65)
(10, 171)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 1, f_spans(5, 2));
		// |=  N  =| |=N=|
		// |=i=|*|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{5, 15},
			t_span{6, 16},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(4, 52)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 10));
		assert(f_dump(array) == R"(
(5, 60)
)");
		f_test_insert_range(array, vector, 6, f_spans(10, 1));
		// |=N=| |=N=| |=N=|
		// |=N=| |*|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{10, 20},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19}
		});
		assert(f_dump(array) == R"(
(5, 60)
(8, 111)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 4));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 2, f_spans(4, 4));
		// |= N =| |=   N   =|
		// |=i=|*****|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{2, 12},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
(4, 50)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 7));
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_insert_range(array, vector, 6, f_spans(7, 4));
		// |=N=| |= N  =| |=   N   =|
		// |=N=| |=i'=|*****|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{6, 16}
		});
		assert(f_dump(array) == R"(
(5, 60)
(8, 110)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 4));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 3, f_spans(4, 2));
		// |= N =| |=   N   =|
		// |=i=|.| |*|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{4, 14},
			t_span{5, 15},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
(3, 33)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 5, f_spans(5, 1));
		// |=N=| |= N =|
		// |=  i  =|*|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(3, 33)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{18, 28},
			t_span{19, 29}
		});
		assert(f_dump(array) == R"(
(4, 50)
(9, 135)
(14, 245)
)");
		f_test_insert_range(array, vector, 8, f_spans(20, 2));
		// |=N=| |= N  =| |=   N   =| |=N=| |=N=|
		// |=N=| |=i'=|.| |*|=n-i=|.| |=N=| |=N=|
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{20, 30},
			t_span{21, 31},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{18, 28},
			t_span{19, 29}
		});
		assert(f_dump(array) == R"(
(5, 65)
(8, 116)
(11, 196)
(16, 306)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{18, 28},
			t_span{19, 29}
		});
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(15, 255)
)");
		f_test_insert_range(array, vector, 9, f_spans(20, 1));
		// |=N=| |=N=| |=    N    =| |=N=| |=N=|
		// |=N=| |=  i  =|*|=n-i=|.| |=N=| |=N=|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{20, 30},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{18, 28},
			t_span{19, 29}
		});
		assert(f_dump(array) == R"(
(5, 60)
(8, 108)
(11, 175)
(16, 285)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 2, f_spans(5, 6));
		// |= N =| |=N=| |=  N  =|
		// |=i=|*******| |=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(5, 69)
(8, 126)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		assert(f_dump(array) == R"(
)");
		f_test_insert_range(array, vector, 2, f_spans(5, 13));
		// |= N =| |=N=| |=N=| |=   N   =|
		// |=i=|*****************|=n-i=|.|
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(5, 69)
(10, 169)
(14, 267)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert_range(array, vector, 0, f_spans(0, 6));
		assert(f_dump(array) == R"(
(3, 33)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert_range(array, vector, 0, f_spans(0, 30));
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(15, 255)
(20, 390)
(25, 550)
)");
		f_test_insert_range(array, vector, 15, f_spans(30, 6));
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{30, 40},
			t_span{31, 41},
			t_span{32, 42},
			t_span{33, 43},
			t_span{34, 44},
			t_span{35, 45},
			t_span{15, 25},
			t_span{16, 26},
			t_span{17, 27},
			t_span{18, 28},
			t_span{19, 29},
			t_span{20, 30},
			t_span{21, 31},
			t_span{22, 32},
			t_span{23, 33},
			t_span{24, 34},
			t_span{25, 35},
			t_span{26, 36},
			t_span{27, 37},
			t_span{28, 38},
			t_span{29, 39}
		});
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 465)
 (3, 96)
 (6, 180)
 (11, 340)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert_range(array, vector, 0, f_spans(0, 1000));
	}
	return 0;
}
