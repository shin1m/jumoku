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

		/*t_index() = default;
		t_index(const t_index&) = default;
		t_index(const t_delta& a_x) : v_i0(a_x.v_i0), v_i1(a_x.v_i1)
		{
		}*/
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

	static constexpr size_t f_default(const t_index& a_index)
	{
		return a_index.v_i0;
	}
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

int main(int argc, char* argv[])
{
	{
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 1));
		f_test_erase(array, vector, 0, 1);
		f_assert_equals(array, {});
		assert(f_dump(array) == R"(
)");
	}
	{
		xtree::t_array<t_span, 5, 5, t_traits> array;
		std::vector<t_span> vector;
		f_test_insert(array, vector, 0, f_spans(0, 2));
		f_test_erase(array, vector, 1, 1);
		f_assert_equals(array, f_spans(0, 1));
		assert(f_dump(array) == R"(
)");
	}
	{
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
		xtree::t_array<t_span, 5, 5, t_traits> array;
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
