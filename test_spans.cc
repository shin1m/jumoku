#include "test_spans.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		assert(array.f_empty());
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
		f_test_insert(array, vector, 0, f_spans(0, 1));
		assert(!array.f_empty());
		f_test_insert(array, vector, 0, f_spans(1, 2));
		f_test_insert(array, vector, 3, f_spans(3, 1));
		f_test_insert(array, vector, 2, f_spans(4, 1));
		f_test_insert(array, vector, 2, f_spans(5, 1));
		f_assert_equals(array, {
			t_span{1, 11},
			t_span{2, 12},
			t_span{5, 15},
			t_span{4, 14},
			t_span{0, 10},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
(3, 38)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		f_test_insert(array, vector, 1, f_spans(5, 1));
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{5, 15},
			t_span{1, 11},
			t_span{2, 12},
		       	t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(3, 36)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		f_test_insert(array, vector, 4, f_spans(5, 1));
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
		       	t_span{3, 13},
			t_span{5, 15},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(3, 33)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 5));
		// xxxxx
		assert(f_dump(array) == R"(
)");
		f_test_insert(array, vector, 0, f_spans(5, 5));
		// xxxxx xxxxx
		assert(f_dump(array) == R"(
(5, 85)
)");
		f_test_insert(array, vector, 0, f_spans(10, 5));
		// xxxxx xxxxx xxxxx
		assert(f_dump(array) == R"(
(5, 110)
(10, 195)
)");
		f_test_insert(array, vector, 8, f_spans(15, 1));
		// xxxxx xxx xxx xxxxx
		f_assert_equals(array, {
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{15, 25},
			t_span{8, 18},
			t_span{9, 19},
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14}
		});
		assert(f_dump(array) == R"(
(5, 110)
(8, 158)
(11, 220)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 8));
		// xxx xxxxx
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_insert(array, vector, 3, f_spans(8, 1));
		// xxxx xxxxx
		assert(f_dump(array) == R"(
(4, 51)
)");
		f_test_insert(array, vector, 5, f_spans(9, 1));
		// xxxxx xxxxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{8, 18},
			t_span{3, 13},
			t_span{9, 19},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17}
		});
		assert(f_dump(array) == R"(
(5, 64)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 30));
		//  i i i i i
		// x x X x x x
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(15, 255)
(20, 390)
(25, 550)
)");
		f_test_insert(array, vector, 10, f_spans(30, 5));
		//        i
		//  i i i   i i
		// x x X X x x x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 355)
(20, 465)
 (5, 135)
 (10, 295)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 30));
		//  i i i i i
		// x x x X x x
		f_test_insert(array, vector, 15, f_spans(30, 3));
		//        i
		//  i i i   i i
		// x x x X X x x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 429)
 (3, 84)
 (8, 244)
)");
		f_test_insert(array, vector, 18, f_spans(33, 1));
		//          i
		//  i i i i   i i
		// x x x X X x x x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
 (18, 378)
(21, 472)
 (3, 84)
 (8, 244)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 30));
		//  i i i i i
		// x x x x X x
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(15, 255)
(20, 390)
(25, 550)
)");
		f_test_insert(array, vector, 20, f_spans(30, 5));
		//        i
		//  i i i   i i
		// x x x x X X x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 210)
 (10, 370)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 1));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 2));
		f_test_erase(array, vector, 1, 1);
		f_assert_equals(array, f_spans(0, 1));
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// xxx xxx
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase(array, vector, 1, 1);
		// xx xxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(2, 22)
)");
		f_test_erase(array, vector, 1, 1);
		// xx xx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(2, 23)
)");
		f_test_erase(array, vector, 1, 1);
		// xxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// xxx xxx
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase(array, vector, 3, 1);
		// xxx xx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{4, 14},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase(array, vector, 3, 1);
		// xx xx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{5, 15}
		});
		assert(f_dump(array) == R"(
(2, 21)
)");
		f_test_erase(array, vector, 3, 1);
		// xxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		// xxxxx xxxxx xxxxx
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase(array, vector, 3, 8);
		// xx xx xxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(2, 21)
(4, 54)
)");
		f_test_erase(array, vector, 3, 1);
		// xx xx xx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(2, 21)
(4, 55)
)");
		f_test_erase(array, vector, 3, 1);
		// xx xxx
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(2, 21)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 40));
		//        i
		//  i i i   i i i
		// X x x x x x x x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 160)
 (10, 345)
 (15, 555)
)");
		f_test_erase(array, vector, 0, 7);
		//      i
		//  i i   i i i
		// X x x x x x x
		assert(f_dump(array) == R"(
 (3, 54)
 (8, 164)
(13, 299)
 (5, 160)
 (10, 345)
 (15, 555)
)");
		f_test_erase(array, vector, 0, 5);
		//      i
		//  i i   i i
		// X x x x x x
		assert(f_dump(array) == R"(
 (3, 69)
 (8, 204)
(13, 364)
 (5, 185)
 (10, 395)
)");
		f_test_erase(array, vector, 0, 5);
		//  i i i i
		// x x x x x
		assert(f_dump(array) == R"(
(3, 84)
(8, 244)
(13, 429)
(18, 639)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 35));
		//        i
		//  i i i   i i
		// x x x x x X x
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 160)
 (10, 345)
)");
		f_test_erase(array, vector, 23, 9);
		//                        i
		//      i     i     i        i  i
		// xxxxx xxxxx xxxxx xxxxx xx xX xx
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (2, 61)
 (4, 135)
)");
		f_test_erase(array, vector, 23, 1);
		//                  i
		//      i     i           i  i
		// xxxxx xxxxx xxxxx xxxxx xx xXX
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
(15, 255)
 (5, 135)
 (7, 196)
)");
		f_test_erase(array, vector, 23, 2);
		//      i     i     i     i
		// xxxxx xxxxx xxxxx xxxxx xxx
		std::cout << f_dump(array) << std::endl;
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(15, 255)
(20, 390)
)");
	}
	return 0;
}
