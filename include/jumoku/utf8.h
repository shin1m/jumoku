#ifndef JUMOKU__UTF8_H
#define JUMOKU__UTF8_H

#include "tree.h"

namespace jumoku
{

template<typename T>
struct t_utf8_index
{
	T v_i0;
	T v_i1;

	template<typename U>
	operator t_utf8_index<U>() const
	{
		return {U(v_i0), U(v_i1)};
	}
	t_utf8_index<int> operator-() const
	{
		return {-int(v_i0), -int(v_i1)};
	}
	bool operator==(const t_utf8_index& a_x) const
	{
		return v_i0 == a_x.v_i0 && v_i1 == a_x.v_i1;
	}
	template<typename U>
	t_utf8_index& operator+=(const t_utf8_index<U>& a_x)
	{
		v_i0 += a_x.v_i0;
		v_i1 += a_x.v_i1;
		return *this;
	}
	template<typename U>
	t_utf8_index& operator-=(const t_utf8_index<U>& a_x)
	{
		v_i0 -= a_x.v_i0;
		v_i1 -= a_x.v_i1;
		return *this;
	}
	template<typename U>
	t_utf8_index operator+(const t_utf8_index<U>& a_x) const
	{
		return {v_i0 + T(a_x.v_i0), v_i1 + T(a_x.v_i1)};
	}
	template<typename U>
	t_utf8_index operator-(const t_utf8_index<U>& a_x) const
	{
		return {v_i0 - T(a_x.v_i0), v_i1 - T(a_x.v_i1)};
	}
};

struct t_utf8_traits
{
	using t_index = t_utf8_index<size_t>;
	using t_delta = t_utf8_index<int>;
	struct t_in_bytes
	{
		constexpr size_t operator()(const t_index& a_index) const
		{
			return a_index.v_i0;
		}
	};
	struct t_in_characters
	{
		constexpr size_t operator()(const t_index& a_index) const
		{
			return a_index.v_i1;
		}
	};

	static bool f_continuation(uint8_t a_c)
	{
		return (a_c & 0xc0) == 0x80;
	}
};

template<size_t A_leaf, size_t A_branch>
class t_utf8 : public t_tree<t_utf8_traits, A_branch>
{
	using typename t_tree<t_utf8_traits, A_branch>::t_branch;
	using typename t_tree<t_utf8_traits, A_branch>::t_via;
	using typename t_tree<t_utf8_traits, A_branch>::t_at;
	using t_tree<t_utf8_traits, A_branch>::f_at;
	using t_index = t_utf8_traits::t_index;
	using t_delta = t_utf8_traits::t_delta;

	struct t_leaf;
	struct t_link
	{
		t_leaf* v_previous;
		t_leaf* v_next;
	};
	struct t_leaf : t_link
	{
		size_t v_size;
		uint8_t v_data[A_leaf];

		t_delta f_delta(size_t a_i, size_t a_n)
		{
			assert(a_n > 0);
			size_t n = 0;
			auto m = a_n;
			for (auto p = v_data + a_i;; ++p) {
				if (!t_utf8_traits::f_continuation(*p)) ++n;
				if (--m <= 0) break;
			}
			return t_index{a_n, n};
		}

		t_leaf(t_leaf* a_p, size_t a_n) : v_size(a_n)
		{
			this->v_previous = a_p;
			this->v_next = a_p->v_next;
			a_p->v_next = this->v_next->v_previous = this;
		}
		t_leaf(size_t a_n, t_leaf* a_p) : v_size(a_n)
		{
			this->v_next = a_p;
			this->v_previous = a_p->v_previous;
			a_p->v_previous = this->v_previous->v_next = this;
		}
		template<typename T>
		t_leaf(t_leaf* a_p, T a_first, size_t a_n) : t_leaf(a_p, a_n)
		{
			std::uninitialized_copy_n(a_first, a_n, v_data);
		}
		void f_unlink()
		{
			this->v_previous->v_next = this->v_next;
			this->v_next->v_previous = this->v_previous;
			delete this;
		}
		template<typename T>
		t_delta f_insert(size_t a_i, T a_first, size_t a_n)
		{
			assert(a_n > 0);
			std::uninitialized_copy_n(a_first, a_n, f_shift(v_data + a_i, v_data + v_size, a_n));
			v_size += a_n;
			return f_delta(a_i, a_n);
		}
		void f_slide(size_t a_i, size_t a_j)
		{
			if (a_i < a_j) {
				f_shift(v_data + a_i, v_data + v_size, a_j - a_i);
			} else if (a_i > a_j) {
				auto p = v_data + a_j;
				f_unshift(p, p + v_size - a_i, a_i - a_j);
			}
		}
		t_delta f_erase(size_t a_i, size_t a_n)
		{
			assert(a_n > 0);
			v_size -= a_n;
			auto delta = f_delta(a_i, a_n);
			f_unshift(v_data + a_i, v_data + v_size, a_n);
			return delta;
		}
		t_delta f_erase(t_leaf* a_p, size_t a_shift, size_t a_i, size_t a_n)
		{
			assert(a_shift > 0);
			assert(a_n > 0);
			v_size -= a_shift;
			assert(v_size > 0);
			a_p->v_size = A_leaf / 2;
			auto delta = a_p->f_delta(a_i, a_n);
			auto p = a_p->v_data;
			auto q = p + a_i;
			auto r = v_data + v_size;
			f_move(r, r + a_shift, f_shift(p, q, a_shift));
			if (a_n > a_shift) f_unshift(q + a_shift, p + A_leaf / 2, a_n - a_shift);
			return delta;
		}
		t_delta f_erase(size_t a_i, size_t a_n, size_t a_shift, t_leaf* a_p)
		{
			assert(a_shift > 0);
			assert(a_n > 0);
			auto delta = f_delta(a_i, a_n);
			auto r = a_p->v_data;
			f_move(r, r + a_shift, f_unshift(v_data + a_i, v_data + v_size - a_n, a_n));
			v_size = A_leaf / 2;
			a_p->v_size -= a_shift;
			f_unshift(r, r + a_p->v_size, a_shift);
			return delta;
		}
		t_delta f_merge(t_leaf* a_p, size_t a_i, size_t a_n)
		{
			assert(v_size > 0);
			assert(a_n > 0);
			auto delta = a_p->f_delta(a_i, a_n);
			auto p = a_p->v_data;
			auto q = p + a_i;
			f_move(q + a_n, p + a_p->v_size, f_move(p, q, v_data + v_size));
			v_size += a_p->v_size - a_n;
			return delta;
		}
		t_delta f_merge(size_t a_i, size_t a_n, t_leaf* a_p)
		{
			assert(a_n > 0);
			auto delta = f_delta(a_i, a_n);
			v_size -= a_n;
			auto r = a_p->v_data;
			f_move(r, r + a_p->v_size, f_unshift(v_data + a_i, v_data + v_size, a_n));
			v_size += a_p->v_size;
			return delta;
		}
	};

	t_link v_link;

	void f_clear(size_t a_level, void* a_node)
	{
		if (++a_level < this->v_depth) {
			auto p = static_cast<t_branch*>(a_node);
			for (size_t i = 0; i <= p->v_size; ++i) f_clear(a_level, p->v_nodes[i]);
			delete p;
		} else {
			delete static_cast<t_leaf*>(a_node);
		}
	}
	template<typename T>
	t_at f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T a_first, size_t a_n);
	t_at f_erase_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, size_t a_n);
	t_at f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i);
	t_at f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j);
	static t_index f_finalize(t_utf8_traits::t_in_bytes&, t_at& i)
	{
		return i.v_index > 0 ? static_cast<t_leaf*>(i.v_node)->f_delta(0, i.v_index) : t_delta{};
	}
	static t_index f_finalize(t_utf8_traits::t_in_characters&, t_at& i)
	{
		auto p = static_cast<t_leaf*>(i.v_node);
		auto j = i.v_index;
		auto q = p->v_data;
		for (;; --i.v_index) {
			while (t_utf8_traits::f_continuation(*q)) ++q;
			if (i.v_index <= 0) break;
			++q;
		}
		i.v_index = q - p->v_data;
		return {i.v_index, j};
	}

public:
	class t_iterator
	{
		friend class t_utf8;

		uint8_t* v_p;
		t_leaf* v_leaf;
		t_index v_index;

		t_iterator(const t_at& a_p, const t_index& a_index) : v_p(static_cast<t_leaf*>(a_p.v_node)->v_data + a_p.v_index), v_leaf(static_cast<t_leaf*>(a_p.v_node)), v_index(a_index)
		{
		}

	public:
		using difference_type = std::ptrdiff_t;
		using value_type = uint8_t;
		using pointer = const uint8_t*;
		using reference = const uint8_t&;
		using iterator_category = std::bidirectional_iterator_tag;

		t_iterator() = default;
		t_iterator(const t_iterator& a_x) : v_p(a_x.v_p), v_leaf(a_x.v_leaf), v_index(a_x.v_index)
		{
		}
		t_iterator& operator=(const t_iterator& a_x)
		{
			v_p = a_x.v_p;
			v_leaf = a_x.v_leaf;
			v_index = a_x.v_index;
			return *this;
		}
		bool operator==(const t_iterator& a_x) const
		{
			return v_p == a_x.v_p;
		}
		bool operator!=(const t_iterator& a_x) const
		{
			return !(*this == a_x);
		}
		uint8_t operator*() const
		{
			return *v_p;
		}
		const t_index& f_index() const
		{
			return v_index;
		}
		t_index f_delta() const
		{
			return {1, t_utf8_traits::f_continuation(*v_p) ? 0u : 1};
		}
		t_iterator& operator++()
		{
			v_index += f_delta();
			if (++v_p == v_leaf->v_data + v_leaf->v_size) {
				v_leaf = v_leaf->v_next;
				v_p = v_leaf->v_data;
			}
			return *this;
		}
		t_iterator operator++(int) const
		{
			auto i = *this;
			++*this;
			return i;
		}
		t_iterator& operator--()
		{
			if (v_p == v_leaf->v_data) {
				v_leaf = v_leaf->v_previous;
				v_p = v_leaf->v_data + v_leaf->v_size;
			}
			--v_p;
			v_index -= f_delta();
			return *this;
		}
		t_iterator operator--(int) const
		{
			auto i = *this;
			--*this;
			return i;
		}
	};

	t_utf8()
	{
		v_link.v_previous = v_link.v_next = static_cast<t_leaf*>(&v_link);
	}
	~t_utf8()
	{
		f_clear();
	}
	void f_clear()
	{
		if (!this->v_root) return;
		f_clear(0, this->v_root);
		this->v_root = nullptr;
		this->v_depth = 0;
		this->v_size = {};
		v_link.v_previous = v_link.v_next = static_cast<t_leaf*>(&v_link);
	}
	t_iterator f_begin() const
	{
		return {{v_link.v_next, 0}, {}};
	}
	t_iterator f_end() const
	{
		return {{const_cast<t_link*>(&v_link), 0}, this->v_size};
	}
	template<typename T>
	t_iterator f_insert(t_iterator a_i, T a_first, T a_last)
	{
		size_t n = a_last - a_first;
		if (n <= 0) return {{a_i.v_leaf, size_t(a_i.v_p - a_i.v_leaf->v_data)}, a_i.v_index};
		if (this->v_root) {
			t_via path[this->f_depth(n / (A_leaf / 2))];
			auto at = f_at(a_i.v_index, path, t_utf8_traits::t_in_bytes());
			return {f_insert_leaf(path, path + this->v_depth - 1, at, a_first, n), a_i.v_index};
		}
		size_t m = std::min(n, A_leaf);
		this->v_root = new t_leaf(static_cast<t_leaf*>(&v_link), a_first, m);
		++this->v_depth;
		this->v_size += static_cast<t_leaf*>(this->v_root)->f_delta(0, m);
		n -= m;
		if (n > 0) {
			t_via path[this->f_depth(n / (A_leaf / 2))];
			f_insert_leaf(path, path, {this->v_root, m}, a_first + m, n);
		}
		return f_begin();
	}
	t_iterator f_erase(t_iterator a_first, t_iterator a_last)
	{
		t_utf8_traits::t_in_bytes use;
		size_t n = use(a_last.v_index) - use(a_first.v_index);
		if (n <= 0) return {{a_first.v_leaf, size_t(a_first.v_p - a_first.v_leaf->v_data)}, a_first.v_index};
		if (n >= use(this->v_size)) {
			f_clear();
			return f_begin();
		}
		t_via path[this->v_depth - 1];
		t_at at;
		size_t i = a_first.v_p - a_first.v_leaf->v_data;
		if (i + n > a_first.v_leaf->v_size) {
			n = a_first.v_leaf->v_size - i;
			size_t index = a_first.v_index.v_i0 + n;
			at = f_at({index}, path, use);
			auto tail = path + this->v_depth - 2;
			size_t j = a_last.v_p - a_last.v_leaf->v_data;
			if (j <= 0) {
				a_last.v_leaf = a_last.v_leaf->v_previous;
				j = a_last.v_leaf->v_size;
			}
			for (size_t last = use(a_last.v_index) - j; index < last;) {
				auto p = static_cast<t_leaf*>(at.v_node);
				assert(p->v_size > 0);
				auto delta = p->f_delta(0, p->v_size);
				if (tail->v_index > 0) {
					--tail->v_index;
					tail = this->f_erase_branch(path, tail + 1, delta) - 1;
					auto p = tail;
					while (p->v_index >= p->v_node->v_size) --p;
					++p->v_index;
					while (p != tail) {
						auto q = p->v_node->v_nodes[p->v_index];
						*++p = {static_cast<t_branch*>(q), 0};
					}
				} else {
					auto q = tail->v_node;
					std::swap(q->v_nodes[0], q->v_nodes[1]);
					tail = this->f_erase_branch(path, tail + 1, delta) - 1;
				}
				this->v_size -= delta;
				at.v_node = tail->v_node->v_nodes[tail->v_index];
				last -= p->v_size;
				p->f_unlink();
			}
			if (tail->v_index > 0) return {f_merge_leaf(path, tail, a_first.v_leaf, i, a_last.v_leaf, j), a_first.v_index};
			f_erase_leaf(path, path + this->v_depth - 1, at, j);
			tail = path + this->v_depth - 2;
			auto p = tail;
			while (p->v_index <= 0) --p;
			--p->v_index;
			while (p != tail) {
				auto q = p->v_node->v_nodes[p->v_index];
				*++p = {static_cast<t_branch*>(q), static_cast<t_branch*>(q)->v_size};
			}
			at = {a_first.v_leaf, i};
		} else {
			at = f_at(a_first.v_index, path, use);
		}
		return {f_erase_leaf(path, path + this->v_depth - 1, at, n), a_first.v_index};
	}
	template<typename T_use>
	t_iterator f_at(size_t a_index, T_use a_use) const
	{
		if (a_index >= a_use(this->v_size)) return f_end();
		t_at i{this->v_root, a_index};
		t_index index{};
		for (size_t n = this->v_depth; --n > 0;) {
			auto p = i.f_step(a_use);
			if (p.v_index > 0) index += p.v_node->v_indices[p.v_index - 1];
		}
		return {i, index + f_finalize(a_use, i)};
	}
	t_iterator f_at(size_t a_index) const
	{
		return f_at(a_index, t_utf8_traits::t_in_bytes());
	}
};

template<size_t A_leaf, size_t A_branch>
template<typename T>
typename t_utf8<A_leaf, A_branch>::t_at t_utf8<A_leaf, A_branch>::f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T a_first, size_t a_n)
{
	assert(a_n > 0);
	// |= N  =| |= N =| |= N  =|
	// |=n0=|.| |=n=|.| |=n1=|.|
	//             ^i|m|
	// if n + m <= N
	//   |=     N     =|
	//   |=i=|*|=n-i=|.|
	//   return
	// l = {node: n, index: i}
	// i' = i
	// m' = m
	// if n0 exists
	//   if n0 + i < N
	//     |=  N   =| ? |=    N    =| ...
	//     |=n0+i=|.| ? |.i.|=n-i=|.| ...
	//     l = {node: n0, index: n0 + i}
	//     if n0 + i + m <= N
	//       |=   N    =| |=  N  =|
	//       |=n0+i=|*|.| |=n-i=|.|
	//       return
	//     |=  N   =| ? |=    N    =| ...
	//     |=n0+i=|*| ? |.i.|=n-i=|.| ...
	//     i' = 0
	//     m' = n0 + i + m - N
	//   else
	//     i' = n0 + i - N
	//     |=N=| |=     N      =| ...
	//     |=N=| |=i'=|.|=n-i=|.| ...
	//     l.index = i'
	//   if i' + m' + n - i <= N
	//     |=     N      =|
	//     |=i'=|*|=n-i=|.|
	//     return
	// m + n > N && n0 >= N / 2
	// m' + n - i (= n0 + m + n - N) > N / 2
	// if l.node == n && i' + m' + n - i > N * 2
	//   |= N  =| ? |=    N    =| ...
	//   |=i'=|*| ? |.i.|=n-i=|.| ...
	//   m' = i' + m' - N
	//   l.node = ?
	//   i' = 0
	//   m' + n - i > N
	// while m' + n - i > N * 2
	//   |=N=| ? |=    N    =| ...
	//   |***| ? |.i.|=n-i=|.| ...
	//   m' = m' - N
	// N < i' + m' + n - i <= N * 2
	// if n1 exists && i' + m' + n - i + n1 <= N * 2
	//   |=  N   =| |=  N   =|
	//   |=i'=|*|=n-i=|=n1=|.|
	// else
	//   |= N =| |= N  =|
	//   |=i'=|*|=n-i=|.|
	auto p = static_cast<t_leaf*>(a_at.v_node);
	size_t i = a_at.v_index;
	// if n + m <= N
	if (p->v_size + a_n <= A_leaf) {
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		auto delta = p->f_insert(i, a_first, a_n);
		this->f_adjust(a_head, a_tail, delta);
		this->v_size += delta;
		return {p, i};
	}
	t_at l{p, i};
	auto v = p->v_data;
	size_t ii = i;
	size_t ni = p->v_size - i;
	if (a_head != a_tail) {
		auto q = a_tail[-1].v_node;
		size_t j = a_tail[-1].v_index;
		// if n0 exists
		if (j > 0) {
			auto r = p->v_previous;
			assert(r->v_size > 0);
			// if n0 + i < N
			if (r->v_size + i < A_leaf) {
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|.| ? |.i.|=n-i=|.| ...
				// l = {node: n0, index: n0 + i}
				auto w = r->v_data + r->v_size;
				if (i > 0) {
					q->v_indices[j - 1] += p->f_delta(0, i);
					f_move(v, v + i, w);
					r->v_size += i;
				}
				l = {r, r->v_size};
				// if n0 + i + m <= N
				if (r->v_size + a_n <= A_leaf) {
					// |=   N    =| |=  N  =|
					// |=n0+i=|*|.| |=n-i=|.|
					std::uninitialized_copy_n(a_first, a_n, w + i);
					auto delta = r->f_delta(r->v_size, a_n);
					r->v_size += a_n;
					q->v_indices[j - 1] += delta;
					if (i > 0) f_unshift(v, v + ni, i);
					p->v_size = ni;
					this->f_adjust(a_head, a_tail, delta);
					this->v_size += delta;
					return l;
				}
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|*| ? |.i.|=n-i=|.| ...
				// m' = i' + m' - N
				// i' = 0
				size_t n = A_leaf - r->v_size;
				std::uninitialized_copy_n(a_first, n, w + i);
				auto delta = r->f_delta(r->v_size, n);
				a_first += n;
				a_n -= n;
				r->v_size = A_leaf;
				q->v_indices[j - 1] += delta;
				this->f_adjust(a_head, a_tail, delta);
				this->v_size += delta;
				ii = 0;
				// m' + n - i > N
			} else if (r->v_size < A_leaf) {
				// i' = n0 + i - N
				// |=N=| |=     N      =| ...
				// |=N=| |=i'=|.|=n-i=|.| ...
				// l.index = i'
				size_t n = A_leaf - r->v_size;
				q->v_indices[j - 1] += p->f_delta(0, n);
				f_move(v, v + n, r->v_data + r->v_size);
				r->v_size = A_leaf;
				ii = l.v_index -= n;
				f_unshift(v, v + ii, n);
			}
		}
		// if i' + m' + n - i <= N
		if (ii + a_n + ni <= A_leaf) {
			// |=     N      =|
			// |=i'=|*|=n-i=|.|
			p->f_slide(i, ii + a_n);
			std::uninitialized_copy_n(a_first, a_n, v + ii);
			auto delta = p->f_delta(ii, a_n);
			p->v_size = ii + a_n + ni;
			this->f_adjust(a_head, a_tail, delta);
			this->v_size += delta;
			return l;
		}
	}
	// m + n > N && n0 >= N / 2
	// m' + n - i (= n0 + m + n - N) > N / 2
	// if l.node == n && i' + m' + n - i > N * 2
	if (l.v_node == p && ii + a_n + ni > A_leaf * 2) {
		// |= N  =| ? |=    N    =| ...
		// |=i'=|*| ? |.i.|=n-i=|.| ...
		// m' = i' + m' - N
		// l.node = ?
		// i' = 0
		size_t n = A_leaf - ii;
		if (ii <= ni) {
			auto r = new t_leaf(A_leaf, p);
			std::uninitialized_copy_n(a_first, n, f_move(v, v + ii, r->v_data));
			auto delta = r->f_delta(ii, n);
			a_tail = this->f_insert_branch(a_head, a_tail, ii > 0 ? r->f_delta(0, ii) + delta : delta, delta, r, false, true);
			this->v_size += delta;
			l.v_node = r;
		} else {
			auto r = new t_leaf(p, p->v_size);
			if (n > 0) {
				f_move(v + i, v + p->v_size, r->v_data + i);
				p->v_size = A_leaf;
				std::uninitialized_copy_n(a_first, n, v + ii);
				auto delta = p->f_delta(ii, n);
				a_tail = this->f_insert_branch(a_head, a_tail, p->f_delta(0, ii) + delta, delta, r, true, true);
				this->v_size += delta;
			} else {
				a_tail = this->f_insert_branch(a_head, a_tail, p->f_delta(0, A_leaf), {}, r, true, true);
			}
			p = r;
			v = r->v_data;
		}
		a_first += n;
		a_n -= n;
		ii = 0;
		// m' + n - i > N
	}
	// while m' + n - i > N * 2
	//   |=N=| ? |=    N    =| ...
	//   |***| ? |.i.|=n-i=|.| ...
	//   m' = m' - N
	while (a_n + ni > A_leaf * 2) {
		assert(l.v_node != p);
		auto r = new t_leaf(A_leaf, p);
		std::uninitialized_copy_n(a_first, A_leaf, r->v_data);
		auto delta = r->f_delta(0, A_leaf);
		a_first += A_leaf;
		a_n -= A_leaf;
		a_tail = this->f_insert_branch(a_head, a_tail, delta, delta, r, false, true);
		this->v_size += delta;
	}
	// N < i' + m' + n - i <= N * 2
	size_t k = ii + a_n + ni;
	// if n1 exists && i' + m' + n - i + n1 <= N * 2
	if (a_head != a_tail) {
		auto q = a_tail[-1].v_node;
		size_t j = a_tail[-1].v_index;
		if (j < q->v_size) {
			auto r = p->v_next;
			if (k + r->v_size <= A_leaf * 2) {
				auto w = r->v_data;
				size_t d = k - A_leaf;
				assert(d > 0);
				f_shift(w, w + r->v_size, d);
				t_delta delta;
				if (ii + a_n <= A_leaf) {
					// |=  N   =| |=  N   =|
					// |=i'=|*|=n-i=|=n1=|.|
					size_t e = p->v_size - d;
					f_move(v + e, v + p->v_size, w);
					f_shift(v + i, v + e, ii + a_n - i);
					auto q = std::uninitialized_copy_n(a_first, a_n, v + ii);
					delta = p->f_delta(ii, a_n);
				} else {
					// |= N  =| |=     N      =|
					// |=i'=|*****|=n-i=|=n1=|.|
					size_t e = A_leaf - ii;
					size_t f = a_n - e;
					f_move(v + i, v + p->v_size, w + f);
					std::uninitialized_copy_n(a_first, e, v + ii);
					std::uninitialized_copy_n(a_first + e, f, w);
					delta = p->f_delta(ii, e) + r->f_delta(0, f);
				}
				p->v_size = A_leaf;
				r->v_size += d;
				q->v_indices[j] -= r->f_delta(0, d);
				this->f_adjust(a_head, a_tail, delta);
				this->v_size += delta;
				return l;
			}
		}
	}
	// |= N =| |= N  =|
	// |=i'=|*|=n-i=|.|
	auto r = new t_leaf(p, k / 2);
	t_delta delta;
	if (ni >= r->v_size) {
		// |=  N   =| |=N=|
		// |=i'=|*|=n-i=|.|
		auto w = v + p->v_size - r->v_size;
		auto q = f_move(w, w + r->v_size, r->v_data);
		f_shift(v + i, w, ii + a_n - i);
		std::uninitialized_copy_n(a_first, a_n, v + ii);
		delta = p->f_delta(ii, a_n);
		p->v_size = k - r->v_size;
	} else if (a_n + ni <= r->v_size) {
		// |=N=| |=    N    =|
		// |= i'  =|*|=n-i=|.|
		auto w = r->v_data + r->v_size - ni;
		f_move(v + i, v + p->v_size, w);
		std::uninitialized_copy_n(a_first, a_n, w - a_n);
		p->v_size = k - r->v_size;
		assert(p->v_size > 0);
		f_move(v + p->v_size, v + ii, r->v_data);
		delta = r->f_delta(ii - p->v_size, a_n);
		l = {r, ii - p->v_size};
	} else {
		// |= N  =| |=   N   =|
		// |=i'=|*****|=n-i=|.|
		auto w = r->v_data;
		size_t e = r->v_size - ni;
		f_move(v + i, v + p->v_size, w + e);
		std::uninitialized_copy_n(a_first, a_n - e, v + ii);
		std::uninitialized_copy_n(a_first + a_n - e, e, w);
		delta = p->f_delta(ii, a_n - e) + r->f_delta(0, e);
		p->v_size = k - r->v_size;
	}
	assert(p->v_size > 0);
	this->f_insert_branch(a_head, a_tail, p->f_delta(0, p->v_size), delta, r, true, false);
	this->v_size += delta;
	p = static_cast<t_leaf*>(l.v_node);
	return p->v_next && l.v_index >= p->v_size ? t_at{p->v_next, 0} : l;
}

template<size_t A_leaf, size_t A_branch>
typename t_utf8<A_leaf, A_branch>::t_at t_utf8<A_leaf, A_branch>::f_erase_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, size_t a_n)
{
	assert(a_n > 0);
	auto p = static_cast<t_leaf*>(a_at.v_node);
	size_t i = a_at.v_index;
	if (a_head == a_tail) {
		this->v_size -= p->f_erase(i, a_n);
		if (i < p->v_size) return {p, i};
		if (p->v_size <= 0) {
			this->v_root = nullptr;
			p->f_unlink();
			--this->v_depth;
		}
		return {&v_link, 0};
	} else if (p->v_size - a_n >= A_leaf / 2) {
		auto delta = p->f_erase(i, a_n);
		this->f_adjust(a_head, a_tail, -delta);
		this->v_size -= delta;
		return i < p->v_size ? t_at{p, i} : t_at{p->v_next, 0};
	}
	size_t n = A_leaf / 2 + a_n - p->v_size;
	auto q = a_tail[-1].v_node;
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = p->v_previous;
		if (r->v_size - n >= A_leaf / 2) {
			auto delta = r->f_erase(p, n, i, a_n);
			q->v_indices[j - 1] -= p->f_delta(0, n);
			this->f_adjust(a_head, a_tail, -delta);
			this->v_size -= delta;
			i += n;
			return i < A_leaf / 2 ? t_at{p, i} : t_at{p->v_next, 0};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			n = r->v_size;
			auto delta = r->f_merge(p, i, a_n);
			p->f_unlink();
			this->f_erase_branch(a_head, a_tail, delta);
			this->v_size -= delta;
			i += n;
			return i < r->v_size ? t_at{r, i} : t_at{r->v_next, 0};
		}
	}
	auto r = p->v_next;
	if (r->v_size - n >= A_leaf / 2) {
		auto size = r->f_delta(0, n);
		auto delta = p->f_erase(i, a_n, n, r);
		q->v_indices[j] += size;
		this->f_adjust(a_head, a_tail, -delta);
		this->v_size -= delta;
	} else {
		auto delta = p->f_merge(i, a_n, r);
		r->f_unlink();
		this->f_erase_branch(a_head, a_tail, delta);
		this->v_size -= delta;
	}
	return {p, i};
}

template<size_t A_leaf, size_t A_branch>
typename t_utf8<A_leaf, A_branch>::t_at t_utf8<A_leaf, A_branch>::f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i)
{
	size_t n = A_leaf / 2 - a_p->v_size;
	assert(n > 0);
	auto v = a_p->v_data;
	auto q = a_tail[-1].v_node;
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = a_p->v_previous;
		assert(r->v_size > 0);
		if (r->v_size - n >= A_leaf / 2) {
			r->v_size -= n;
			a_p->v_size = A_leaf / 2;
			auto w = r->v_data + r->v_size;
			f_move(w, w + n, f_shift(v, v + a_p->v_size, n));
			q->v_indices[j - 1] -= a_p->f_delta(0, n);
			a_i += n;
			return a_i < A_leaf / 2 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			a_i += r->v_size;
			f_move(v, v + a_p->v_size, r->v_data + r->v_size);
			r->v_size += a_p->v_size;
			a_p->f_unlink();
			this->f_erase_branch(a_head, a_tail, {});
			return a_i < r->v_size ? t_at{r, a_i} : t_at{r->v_next, 0};
		}
	}
	auto r = a_p->v_next;
	auto w = r->v_data;
	auto vv = v + a_p->v_size;
	if (r->v_size - n >= A_leaf / 2) {
		auto delta = r->f_delta(0, n);
		f_move(w, w + n, vv);
		a_p->v_size = A_leaf / 2;
		r->v_size -= n;
		f_unshift(w, w + r->v_size, n);
		q->v_indices[j] += delta;
	} else {
		f_move(w, w + r->v_size, vv);
		a_p->v_size += r->v_size;
		r->f_unlink();
		this->f_erase_branch(a_head, a_tail, {});
	}
	return {a_p, a_i};
}

template<size_t A_leaf, size_t A_branch>
typename t_utf8<A_leaf, A_branch>::t_at t_utf8<A_leaf, A_branch>::f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j)
{
	auto v = a_p->v_data + a_i;
	size_t a = a_p->v_size - a_i;
	assert(a > 0);
	auto delta = a_p->f_delta(a_i, a);
	auto w = a_q->v_data;
	assert(a_j > 0);
	auto q = a_tail->v_node;
	size_t j = a_tail->v_index - 1;
	q->v_indices[j] -= delta;
	delta += a_q->f_delta(0, a_j);
	this->v_size -= delta;
	size_t b = a_q->v_size - a_j;
	size_t m = a_i + b;
	if (m >= A_leaf / 2 * 2) {
		this->f_adjust(a_head, a_tail + 1, -delta);
		if (a_i < A_leaf / 2) {
			a_p->v_size = A_leaf / 2;
			a_q->v_size = m - A_leaf / 2;
			size_t d = A_leaf / 2 - a_i;
			q->v_indices[j] += a_q->f_delta(a_j, d);
			size_t jd = a_j + d;
			f_move(w + a_j, w + jd, v);
			f_unshift(w, w + a_q->v_size, jd);
			return {a_p, a_i};
		} else if (b < A_leaf / 2) {
			a_p->v_size = m - A_leaf / 2;
			assert(a_p->v_size > 0);
			a_q->v_size = A_leaf / 2;
			size_t d = A_leaf / 2 - b;
			auto ww = f_move(a_p->v_data + a_p->v_size, v, w);
			if (a_j > d) f_unshift(ww, w + a_q->v_size, a_j - d);
			q->v_indices[j] -= a_q->f_delta(0, d);
			return b > 0 ? t_at{a_q, d} : t_at{a_q->v_next, 0};
		}
		a_p->v_size = a_i;
		a_q->v_size = b;
		f_unshift(w, w + a_q->v_size, a_j);
		return {a_q, 0};
	}
	a_p->v_size = m;
	f_move(w + a_j, w + a_j + b, v);
	if (b > 0) q->v_indices[j] += a_p->f_delta(a_i, b);
	a_tail->v_index = j;
	a_q->f_unlink();
	a_tail = this->f_erase_branch(a_head, a_tail + 1, delta);
	if (a_tail == a_head || m >= A_leaf / 2) return b > 0 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
	return f_merge_leaf(a_head, a_tail, a_p, a_i);
}

}

#endif
