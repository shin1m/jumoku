#include <iostream>
#include <vector>
#include <cassert>

#include "array.h"

template<typename T_value, size_t A_size>
std::string f_dump(const xtree::t_array<T_value, A_size>& a_array)
{
	std::string s = R"(
)";
	a_array.f_dump([&](auto a_level, auto a_index)
	{
		s += std::string(a_level, ' ') + std::to_string(a_index) + R"(
)";
	});
	return s;
}

template<typename T_value, size_t A_size, typename T>
void f_assert_equals(const xtree::t_array<T_value, A_size>& a0, const T& a1)
{
	assert(a0.f_size() == a1.size());
	assert(std::equal(a0.f_begin(), a0.f_end(), a1.begin(), a1.end()));
	auto i = a1.begin();
	for (size_t j = 0; j < a0.f_size(); ++j) assert(*a0.f_at(j) == *i++);
}

template<typename T_value, size_t A_size>
void f_assert_equals(const xtree::t_array<T_value, A_size>& a0, std::initializer_list<T_value> a1)
{
	f_assert_equals<T_value, A_size, std::initializer_list<T_value>>(a0, a1);
}

template<typename T_value, size_t A_size, typename T_iterator>
void f_test_insert(xtree::t_array<T_value, A_size>& a0, std::vector<T_value>& a1, size_t a_i, T_iterator a_first, T_iterator a_last)
{
	a1.insert(a1.begin() + a_i, a_first, a_last);
	for (auto i = a0.f_at(a_i); a_first != a_last; ++a_first) {
		i = a0.f_insert(i, *a_first);
		assert(*i == *a_first);
		++i;
	}
	f_assert_equals(a0, a1);
}

template<typename T_value, size_t A_size, typename T>
void f_test_insert(xtree::t_array<T_value, A_size>& a0, std::vector<T_value>& a1, size_t a_i, const T& a_values)
{
	f_test_insert(a0, a1, a_i, a_values.begin(), a_values.end());
}

template<typename T_value, size_t A_size>
void f_test_insert(xtree::t_array<T_value, A_size>& a0, std::vector<T_value>& a1, size_t a_i, std::initializer_list<T_value> a_values)
{
	f_test_insert(a0, a1, a_i, a_values.begin(), a_values.end());
}

template<typename T_value, size_t A_size>
void f_test_erase(xtree::t_array<T_value, A_size>& a0, std::vector<T_value>& a1, size_t a_i, size_t a_n)
{
	{
		auto i = a1.begin() + a_i;
		a1.erase(i, i + a_n);
	}
	auto i = a0.f_at(a_i);
	for (size_t j = 0; j < a_n; ++j) {
		i = a0.f_erase(i);
		assert(i == a0.f_at(a_i));
	}
	f_assert_equals(a0, a1);
}

std::vector<int> f_range(int a_i, size_t a_n)
{
	std::vector<int> vector;
	for (size_t i = 0; i < a_n; ++i) vector.push_back(a_i++);
	return vector;
}

int main(int argc, char* argv[])
{
	{
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 2, {5});
		f_assert_equals(array, {0, 1, 5, 2, 3, 4});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		xtree::t_array<int, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, f_range(0, 5));
		f_test_insert(array, vector, 3, {5});
		f_assert_equals(array, {0, 1, 2, 5, 3, 4});
		assert(f_dump(array) == R"(
3
)");
	}
	{
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0});
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
	}
	{
		xtree::t_array<int, 5> array;
		std::vector<int> vector;
		f_test_insert(array, vector, 0, {0, 1});
		f_test_erase(array, vector, 1, 1);
		f_assert_equals(array, {0});
		assert(f_dump(array) == R"(
)");
	}
	{
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
		xtree::t_array<int, 5> array;
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
