#include "test_spans.h"

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits, typename T, typename T_use>
void f_test_at(const jumoku::t_array<T_value, A_leaf, A_branch, T_traits>& a0, const T& a1, T_use a_use)
{
	auto n = a_use(a0.f_size());
	for (auto i = 0; i < n; ++i) {
		typename T_traits::t_index m{};
		auto index = m;
		auto j = std::find_if(a1.begin(), a1.end(), [&](auto a_x)
		{
			index = m;
			m += T_traits::f_index(1, a_x);
			return a_use(m) > i;
		});
		auto k = a0.f_at(i, a_use);
		assert(*k == *j);
		assert(k.f_index() == index);
		assert(k.f_delta() == T_traits::f_index(1, *j));
	}
}

int main(int argc, char* argv[])
{
	jumoku::t_array<t_span, 5, 5, t_traits> array;
	std::vector<t_span> vector;
	f_test_insert(array, vector, 0, f_spans(0, 30));
	f_test_at(array, vector, [](const auto& a_index)
	{
		return a_index.v_i1;
	});
	return 0;
}
