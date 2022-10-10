#include <jumoku/utf8.h>

#include <iostream>
#include <numeric>
#include <vector>
#include <cassert>

using namespace std::literals;

namespace std
{

string to_string(const jumoku::t_utf8_traits::t_index& a_x)
{
	return '(' + to_string(a_x.v_i0) + ", " + to_string(a_x.v_i1) + ')';
}

}

template<size_t A_leaf, size_t A_branch>
std::string f_dump(const jumoku::t_utf8<A_leaf, A_branch>& a_utf8)
{
	auto s = R"(
)"s;
	a_utf8.f_dump([&](auto a_level, auto a_index)
	{
		s += std::string(a_level, ' ') + std::to_string(a_index) + R"(
)";
	});
	return s;
}

template<size_t A_leaf, size_t A_branch, typename T>
void f_assert_equals(const jumoku::t_utf8<A_leaf, A_branch>& a0, const T& a1)
{
	assert(a0.f_size() == std::accumulate(a1.begin(), a1.end(), jumoku::t_utf8_traits::t_index{}, [](auto a_x, auto a_y)
	{
		return a_x + jumoku::t_utf8_traits::t_index{1, jumoku::t_utf8_traits::f_continuation(a_y) ? 0u : 1};
	}));
	assert(std::equal(a0.f_begin(), a0.f_end(), a1.begin(), a1.end()));
	auto i = a1.begin();
	for (size_t j = 0; j < a1.size(); ++j) assert(*a0.f_at(j) == *i++);
}

template<size_t A_leaf, size_t A_branch>
void f_assert_equals(const jumoku::t_utf8<A_leaf, A_branch>& a0, std::initializer_list<uint8_t> a1)
{
	f_assert_equals<A_leaf, A_branch, std::initializer_list<uint8_t>>(a0, a1);
}

std::vector<uint8_t> f_range(uint8_t a_i, size_t a_n)
{
	std::vector<uint8_t> vector;
	for (size_t i = 0; i < a_n; ++i) vector.push_back(a_i++);
	return vector;
}

template<size_t A_leaf, size_t A_branch, typename T_iterator>
void f_test_insert_range(jumoku::t_utf8<A_leaf, A_branch>& a0, std::vector<uint8_t>& a1, size_t a_i, T_iterator a_first, T_iterator a_last)
{
	auto i = a0.f_insert(a0.f_at(a_i), a_first, a_last);
	assert(std::equal(a_first, a_last, i));
	a1.insert(a1.begin() + a_i, a_first, a_last);
	f_assert_equals(a0, a1);
}

template<size_t A_leaf, size_t A_branch, typename T>
void f_test_insert_range(jumoku::t_utf8<A_leaf, A_branch>& a0, std::vector<uint8_t>& a1, size_t a_i, const T& a_values)
{
	f_test_insert_range(a0, a1, a_i, a_values.begin(), a_values.end());
}

template<size_t A_leaf, size_t A_branch>
void f_test_insert_range(jumoku::t_utf8<A_leaf, A_branch>& a0, std::vector<uint8_t>& a1, size_t a_i, std::initializer_list<uint8_t> a_values)
{
	f_test_insert_range(a0, a1, a_i, a_values.begin(), a_values.end());
}

template<size_t A_leaf, size_t A_branch>
void f_test_erase_range(jumoku::t_utf8<A_leaf, A_branch>& a0, std::vector<uint8_t>& a1, size_t a_i, size_t a_n)
{
	a1.erase(a1.begin() + a_i, a1.begin() + a_i + a_n);
	auto i = a0.f_erase(a0.f_at(a_i), a0.f_at(a_i + a_n));
	assert(i == a0.f_at(a_i));
	f_assert_equals(a0, a1);
}
