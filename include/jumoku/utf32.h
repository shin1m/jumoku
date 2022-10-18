#ifndef JUMOKU__UTF32_H
#define JUMOKU__UTF32_H

#include "utf8.h"

namespace jumoku
{

struct t_utf32_traits
{
	static size_t f_delta(uint8_t a_c)
	{
		if (a_c < 0x80) return 1;
		if (a_c < 0xc0) return 0;
		if (a_c < 0xe0) return 2;
		if (a_c < 0xf0) return 3;
		if (a_c < 0xf8) return 4;
		if (a_c < 0xfc) return 5;
		if (a_c < 0xfe) return 6;
		return 0;
	}
	template<typename T>
	static wchar_t f_decode(wchar_t a_c, T a_next)
	{
		auto d = f_delta(a_c);
		assert(d > 0);
		if (d < 2) return a_c;
		a_c &= 0x7f >> d;
		do {
			a_c <<= 6;
			a_c |= a_next() & 0x3f;
		} while (--d > 1);
		return a_c;
	}
	template<typename T_put>
	static void f_encode(wchar_t a_c, T_put a_put)
	{
		if (a_c < L'\u0080') {
			a_put(a_c);
		} else {
			auto d =
				a_c < L'\u0800' ? 1 :
				a_c < L'\U00010000' ? 2 :
				a_c < L'\U00200000' ? 3 :
				a_c < L'\U04000000' ? 4 :
				5;
			auto s = d * 6;
			a_put(a_c >> s | int8_t(0x80) >> d);
			do {
				s -= 6;
				a_put(a_c >> s & 0x3f | 0x80);
			} while (s > 0);
		}
	}
};

template<typename T>
class t_utf32_iterator
{
	T v_i;

public:
	using difference_type = ptrdiff_t;
	using value_type = wchar_t;
	using pointer = const wchar_t*;
	using reference = const wchar_t&;
	using iterator_category = std::bidirectional_iterator_tag;

	t_utf32_iterator() = default;
	t_utf32_iterator(const T& a_i) : v_i(a_i)
	{
	}
	t_utf32_iterator(const t_utf32_iterator& a_x) : v_i(a_x.v_i)
	{
	}
	t_utf32_iterator& operator=(const t_utf32_iterator& a_x)
	{
		v_i = a_x.v_i;
		return *this;
	}
	bool operator==(const t_utf32_iterator& a_x) const
	{
		return v_i == a_x.v_i;
	}
	bool operator!=(const t_utf32_iterator& a_x) const
	{
		return !(*this == a_x);
	}
	wchar_t operator*() const
	{
		return t_utf32_traits::f_decode(*v_i, [i = v_i]() mutable
		{
			return *++i;
		});
	}
	t_utf32_iterator& operator++()
	{
		auto d = t_utf32_traits::f_delta(*v_i);
		assert(d > 0);
		do ++v_i; while (--d > 0);
		return *this;
	}
	t_utf32_iterator operator++(int) const
	{
		auto i = *this;
		++*this;
		return i;
	}
	t_utf32_iterator& operator--()
	{
		do --v_i; while (t_utf8_traits::f_continuation(*v_i));
		return *this;
	}
	t_utf32_iterator operator--(int) const
	{
		auto i = *this;
		--*this;
		return i;
	}
};

template<typename T>
inline t_utf32_iterator<T> f_utf32(const T& a_i)
{
	return a_i;
}

template<typename T>
class t_utf32_output_iterator
{
	T& v_i;

public:
	using difference_type = void;
	using value_type = void;
	using pointer = void;
	using reference = void;
	using iterator_category = std::output_iterator_tag;

	t_utf32_output_iterator(T& a_i) : v_i(a_i)
	{
	}
	t_utf32_output_iterator& operator=(wchar_t a_c)
	{
		t_utf32_traits::f_encode(a_c, [&](uint8_t c)
		{
			*v_i = c;
			++v_i;
		});
		return *this;
	}
	t_utf32_output_iterator& operator*()
	{
		return *this;
	}
	t_utf32_output_iterator& operator++()
	{
		return *this;
	}
	t_utf32_output_iterator& operator++(int)
	{
		return *this;
	}
};

template<typename T>
inline t_utf32_output_iterator<T> f_utf32_output(T& a_i)
{
	return a_i;
}

template<size_t A_leaf, size_t A_branch, size_t A_buffer>
class t_utf8_insert_iterator
{
	t_utf8<A_leaf, A_branch>& v_utf8;
	typename t_utf8<A_leaf, A_branch>::t_iterator v_i;
	uint8_t v_cs[A_buffer];
	uint8_t* v_p;

public:
	using difference_type = void;
	using value_type = void;
	using pointer = void;
	using reference = void;
	using iterator_category = std::output_iterator_tag;

	t_utf8_insert_iterator(t_utf8<A_leaf, A_branch>& a_utf8, typename t_utf8<A_leaf, A_branch>::t_iterator a_i) : v_utf8(a_utf8), v_i(a_i), v_p(v_cs)
	{
	}
	~t_utf8_insert_iterator()
	{
		v_utf8.f_insert(v_i, v_cs, v_p);
	}
	t_utf8_insert_iterator& operator=(uint8_t a_c)
	{
		*v_p = a_c;
		if (++v_p >= v_cs + A_buffer) {
			v_i = v_utf8.f_at(v_utf8.f_insert(v_i, v_cs, v_p).f_index().v_byte + v_p - v_cs);
			v_p = v_cs;
		}
		return *this;
	}
	t_utf8_insert_iterator& operator*()
	{
		return *this;
	}
	t_utf8_insert_iterator& operator++()
	{
		return *this;
	}
	t_utf8_insert_iterator& operator++(int)
	{
		return *this;
	}
};

template<size_t A_leaf, size_t A_branch, size_t A_buffer = 1024>
inline t_utf8_insert_iterator<A_leaf, A_branch, A_buffer> f_utf8_inserter(t_utf8<A_leaf, A_branch>& a_utf8, typename t_utf8<A_leaf, A_branch>::t_iterator a_i)
{
	return {a_utf8, a_i};
}

template<typename T>
class t_utf16_iterator
{
	T v_i;
	bool v_low = false;

public:
	using difference_type = ptrdiff_t;
	using value_type = wchar_t;
	using pointer = const wchar_t*;
	using reference = const wchar_t&;
	using iterator_category = std::bidirectional_iterator_tag;

	t_utf16_iterator() = default;
	t_utf16_iterator(const T& a_i) : v_i(a_i)
	{
	}
	t_utf16_iterator(const t_utf16_iterator& a_x) : v_i(a_x.v_i), v_low(a_x.v_low)
	{
	}
	t_utf16_iterator& operator=(const t_utf16_iterator& a_x)
	{
		v_i = a_x.v_i;
		v_low = a_x.v_low;
		return *this;
	}
	bool operator==(const t_utf16_iterator& a_x) const
	{
		return v_i == a_x.v_i && v_low == a_x.v_low;
	}
	bool operator!=(const t_utf16_iterator& a_x) const
	{
		return !(*this == a_x);
	}
	wchar_t operator*() const
	{
		auto c = t_utf32_traits::f_decode(*v_i, [i = v_i]() mutable
		{
			return *++i;
		});
		if (c < 0x10000) return c;
		c -= 0x10000;
		return v_low ? c & 0x3ff | 0xdc00 : c >> 10 | 0xd800;
	}
	t_utf16_iterator& operator++()
	{
		auto d = t_utf32_traits::f_delta(*v_i);
		assert(d > 0);
		if (v_low || d < 4) {
			do ++v_i; while (--d > 0);
			v_low = false;
		} else {
			v_low = true;
		}
		return *this;
	}
	t_utf16_iterator operator++(int) const
	{
		auto i = *this;
		++*this;
		return i;
	}
	t_utf16_iterator& operator--()
	{
		if (v_low) {
			v_low = false;
		} else {
			do --v_i; while (t_utf8_traits::f_continuation(*v_i));
			v_low = t_utf32_traits::f_delta(*v_i) >= 4;
		}
		return *this;
	}
	t_utf16_iterator operator--(int) const
	{
		auto i = *this;
		--*this;
		return i;
	}
};

template<typename T>
inline t_utf16_iterator<T> f_utf16(const T& a_i)
{
	return a_i;
}

template<typename T>
class t_utf16_output_iterator
{
	T& v_i;
	wchar_t v_high = L'\0';

public:
	using difference_type = void;
	using value_type = void;
	using pointer = void;
	using reference = void;
	using iterator_category = std::output_iterator_tag;

	t_utf16_output_iterator(T& a_i) : v_i(a_i)
	{
	}
	t_utf16_output_iterator& operator=(wchar_t a_c)
	{
		auto encode = [&](wchar_t c)
		{
			t_utf32_traits::f_encode(c, [&](uint8_t c)
			{
				*v_i = c;
				++v_i;
			});
		};
		if (v_high) {
			auto low = a_c >= 0xdc00 && a_c < 0xe000;
			if (low) v_high = ((v_high & 0x3ff) << 10 | a_c & 0x3ff) + 0x10000;
			encode(v_high);
			v_high = L'\0';
			if (low) return *this;
		}
		if (a_c >= 0xd800 && a_c < 0xdc00)
			v_high = a_c;
		else
			encode(a_c);
		return *this;
	}
	t_utf16_output_iterator& operator*()
	{
		return *this;
	}
	t_utf16_output_iterator& operator++()
	{
		return *this;
	}
	t_utf16_output_iterator& operator++(int)
	{
		return *this;
	}
};

template<typename T>
inline t_utf16_output_iterator<T> f_utf16_output(T& a_i)
{
	return a_i;
}

}

#endif
