#include "test_spans.h"

int main(int argc, char* argv[])
{
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 3));
		f_test_erase_range(array, vector, 0, 3);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 4));
		f_test_erase_range(array, vector, 1, 2);
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{3, 13}
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 4));
		f_test_erase_range(array, vector, 2, 2);
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11}
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase_range(array, vector, 2, 2);
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
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
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(3, 35)
(8, 120)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		// |=====| |****=| ...
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase_range(array, vector, 5, 4);
		// |====|.| -> |==...| ...
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(4, 46)
(6, 79)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		// |=====| |=****| ...
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase_range(array, vector, 6, 4);
		// |====.| -> |==...| ...
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{1, 11},
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(4, 46)
(6, 75)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 10));
		f_test_erase(array, vector, 0, 3);
		// |==...| |****=|
		assert(f_dump(array) == R"(
(2, 27)
)");
		f_test_erase_range(array, vector, 2, 4);
		// |===..|
		f_assert_equals(array, {
			t_span{3, 13},
			t_span{4, 14},
			t_span{9, 19}
		});
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 10));
		f_test_erase(array, vector, 0, 2);
		// |===..| |*****|
		assert(f_dump(array) == R"(
(3, 39)
)");
		f_test_erase_range(array, vector, 3, 5);
		// |===..|
		f_assert_equals(array, {
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
		f_test_insert(array, vector, 0, f_spans(0, 7));
		// |**=..| |====.|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 0, 2);
		// |==...| <- |===..|
		f_assert_equals(array, {
			t_span{2, 12},
			t_span{3, 13},
			t_span{4, 14},
			t_span{5, 15},
			t_span{6, 16}
		});
		assert(f_dump(array) == R"(
(2, 25)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		// |*****| |=====| ...
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase_range(array, vector, 0, 5);
		// |==...| <- |===..| ...
		f_assert_equals(array, {
			t_span{5, 15},
			t_span{6, 16},
			t_span{7, 17},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(2, 31)
(5, 85)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		f_test_erase(array, vector, 5, 1);
		// |**=..| |==...|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 0, 2);
		// |===..|
		f_assert_equals(array, {
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
		f_test_insert(array, vector, 0, f_spans(0, 15));
		f_test_erase(array, vector, 5, 3);
		// |****=| |==...| ...
		assert(f_dump(array) == R"(
(5, 60)
(7, 97)
)");
		f_test_erase_range(array, vector, 1, 4);
		// |===..| ...
		f_assert_equals(array, {
			t_span{0, 10},
			t_span{8, 18},
			t_span{9, 19},
			t_span{10, 20},
			t_span{11, 21},
			t_span{12, 22},
			t_span{13, 23},
			t_span{14, 24}
		});
		assert(f_dump(array) == R"(
(3, 47)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 15));
		// |*****| |*****| |*====|
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
)");
		f_test_erase_range(array, vector, 0, 11);
		// ...
		assert(f_dump(array) == R"(
(2, 43)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 35));
		//                    x
		//  x x       x               x x
		// ... |*****| |*****| |****=| ...
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 160)
 (10, 345)
)");
		f_test_erase_range(array, vector, 10, 14);
		//                        x
		//        x       x               x       x
		// |=====| |===..| |==...| |==...| |====.| |=====|
		assert(f_dump(array) == R"(
 (5, 60)
 (8, 108)
(10, 145)
 (2, 69)
 (6, 219)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 31));
		f_test_erase(array, vector, 27, 1);
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |****=| |==...| |===..|
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 160)
 (7, 231)
)");
		f_test_erase_range(array, vector, 10, 14);
		//        x       x       x       x
		// |=====| |===..| |==...| |===..| |===..|
		assert(f_dump(array) == R"(
(5, 60)
(8, 108)
(10, 145)
(13, 250)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 35));
		//                    x
		//  x x       x               x       x
		// ... |*****| |*****| |*****| |*====| ...
		assert(f_dump(array) == R"(
 (5, 60)
 (10, 145)
 (15, 255)
(20, 390)
 (5, 160)
 (10, 345)
)");
		f_test_erase_range(array, vector, 10, 16);
		//  x x       x       x
		// ... |==...| |==...| ...
		assert(f_dump(array) == R"(
(5, 60)
(10, 145)
(12, 218)
(14, 295)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// |==*..| |*==..|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 2, 2);
		// |==...| |==...|
		assert(f_dump(array) == R"(
(2, 21)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 7));
		// |=**..| |*===.|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 1, 3);
		// |==...| <- |==...|
		assert(f_dump(array) == R"(
(2, 24)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 8));
		// |***..| |*====|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 0, 4);
		// |==...| <- |==...|
		assert(f_dump(array) == R"(
(2, 29)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 9));
		// |===*.| |****=|
		assert(f_dump(array) == R"(
(4, 46)
)");
		f_test_erase_range(array, vector, 3, 5);
		// |==...| -> |==...|
		assert(f_dump(array) == R"(
(2, 21)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 10));
		// |====*| |****=|
		assert(f_dump(array) == R"(
(5, 60)
)");
		f_test_erase_range(array, vector, 4, 5);
		// |===..| -> |==...|
		assert(f_dump(array) == R"(
(3, 33)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// |=**..| |*==..|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 1, 3);
		// |===..|
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_test_erase(array, vector, 8, 4);
		// ... |=**..| |*==..| ...
		assert(f_dump(array) == R"(
(5, 60)
(8, 108)
(11, 177)
)");
		f_test_erase_range(array, vector, 6, 3);
		// ... |===..| ...
		assert(f_dump(array) == R"(
(5, 60)
(8, 122)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// |***..| |**=..|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 0, 5);
		// |=....|
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 6));
		// |=**..| |***..|
		assert(f_dump(array) == R"(
(3, 33)
)");
		f_test_erase_range(array, vector, 1, 5);
		// |=....|
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_test_erase(array, vector, 8, 4);
		// |=====| |***..| |**=..| ...
		assert(f_dump(array) == R"(
(5, 60)
(8, 108)
(11, 177)
)");
		f_test_erase_range(array, vector, 5, 5);
		// |====.| -> |==...| ...
		assert(f_dump(array) == R"(
(4, 46)
(6, 84)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_test_erase(array, vector, 8, 4);
		// |=====| |=**..| |***..| ...
		assert(f_dump(array) == R"(
(5, 60)
(8, 108)
(11, 177)
)");
		f_test_erase_range(array, vector, 6, 5);
		// |====.| -> |==...| ...
		assert(f_dump(array) == R"(
(4, 46)
(6, 75)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 11));
		f_test_erase(array, vector, 0, 3);
		// |==...| |***..| |**=..|
		assert(f_dump(array) == R"(
(2, 27)
(5, 75)
)");
		f_test_erase_range(array, vector, 2, 5);
		// |====.|
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 11));
		f_test_erase(array, vector, 0, 3);
		// |==...| |=**..| |***..|
		assert(f_dump(array) == R"(
(2, 27)
(5, 75)
)");
		f_test_erase_range(array, vector, 3, 5);
		// |====.|
		assert(f_dump(array) == R"(
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 20));
		f_test_erase(array, vector, 3, 4);
		// |***..| |**=..| |=====| ...
		assert(f_dump(array) == R"(
(3, 33)
(6, 87)
(11, 197)
)");
		f_test_erase_range(array, vector, 0, 5);
		// |==...| <- |====.| ...
		assert(f_dump(array) == R"(
(2, 39)
(6, 129)
)");
	}
	{
		jumoku::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 11));
		f_test_erase(array, vector, 3, 2);
		f_test_erase(array, vector, 8, 1);
		// |***..| |**=..| |==...|
		assert(f_dump(array) == R"(
(3, 33)
(6, 81)
)");
		f_test_erase_range(array, vector, 0, 5);
		// |====.|
		assert(f_dump(array) == R"(
)");
	}
	return 0;
}
