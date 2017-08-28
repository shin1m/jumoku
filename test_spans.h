#include <string>

struct t_span
{
	int v_x;
	size_t v_n;

	bool operator==(const t_span& a_x) const
	{
		return v_x == a_x.v_x && v_n == a_x.v_n;
	}
};

struct t_traits
{
	struct t_index;
	struct t_delta
	{
		int v_i0;
		int v_i1;

		operator t_index() const
		{
			return {size_t(v_i0), size_t(v_i1)};
		}
		t_delta operator-() const
		{
			return {-v_i0, -v_i1};
		}
		t_delta& operator+=(const t_delta& a_x)
		{
			v_i0 += a_x.v_i0;
			v_i1 += a_x.v_i1;
			return *this;
		}
		t_delta& operator-=(const t_index& a_x)
		{
			v_i0 -= a_x.v_i0;
			v_i1 -= a_x.v_i1;
			return *this;
		}
		t_delta operator+(const t_delta& a_x) const
		{
			return {v_i0 + a_x.v_i0, v_i1 + a_x.v_i1};
		}
		t_delta operator-(const t_delta& a_x) const
		{
			return {v_i0 - a_x.v_i0, v_i1 - a_x.v_i1};
		}
	};
	struct t_index
	{
		size_t v_i0;
		size_t v_i1;

		operator t_delta() const
		{
			return {int(v_i0), int(v_i1)};
		}
		t_delta operator-() const
		{
			return {-int(v_i0), -int(v_i1)};
		}
		bool operator==(const t_index& a_x) const
		{
			return v_i0 == a_x.v_i0 && v_i1 == a_x.v_i1;
		}
		t_index& operator+=(const t_index& a_x)
		{
			v_i0 += a_x.v_i0;
			v_i1 += a_x.v_i1;
			return *this;
		}
		t_index& operator+=(const t_delta& a_x)
		{
			v_i0 += a_x.v_i0;
			v_i1 += a_x.v_i1;
			return *this;
		}
		t_index& operator-=(const t_index& a_x)
		{
			v_i0 -= a_x.v_i0;
			v_i1 -= a_x.v_i1;
			return *this;
		}
		t_index operator+(const t_index& a_x) const
		{
			return {v_i0 + a_x.v_i0, v_i1 + a_x.v_i1};
		}
		t_index operator+(const t_delta& a_x) const
		{
			return {v_i0 + a_x.v_i0, v_i1 + a_x.v_i1};
		}
		t_index operator-(const t_delta& a_x) const
		{
			return {v_i0 - a_x.v_i0, v_i1 - a_x.v_i1};
		}
	};
	struct t_default
	{
		constexpr size_t operator()(const t_index& a_index) const
		{
			return a_index.v_i0;
		}
	};

	template<typename T>
	static constexpr t_index f_index(size_t a_n, const T& a_value)
	{
		return {a_n, a_value.v_n};
	}
	template<typename T>
	static constexpr void f_add(T& a_value, const t_delta& a_index)
	{
		a_value.v_n += a_index.v_i1;
	}
	template<typename T>
	static constexpr size_t f__delta(T* a_base, T* a_p)
	{
		return a_p > a_base ? a_p->v_n - a_p[-1].v_n : a_p->v_n;
	}
	template<typename T>
	static constexpr T f_get(T* a_base, T* a_p)
	{
		return {a_p->v_x, f__delta(a_base, a_p)};
	}
	template<typename T>
	static constexpr t_index f_delta(T* a_base, T* a_p)
	{
		return {1, f__delta(a_base, a_p)};
	}
};

namespace std
{

string to_string(const t_traits::t_index& a_x)
{
	return '(' + to_string(a_x.v_i0) + ", " + to_string(a_x.v_i1) + ')';
}

}

#include "test_array.h"

std::vector<t_span> f_spans(int a_i, size_t a_n)
{
	std::vector<t_span> vector;
	for (size_t i = 0; i < a_n; ++i, ++a_i) vector.push_back({a_i, size_t(a_i) + 10});
	return vector;
}
