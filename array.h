#ifndef XTREE__ARRAY_H
#define XTREE__ARRAY_H

#include "tree.h"

namespace xtree
{

template<typename T_value, size_t A_leaf, size_t A_branch>
class t_array : public t_tree<A_branch>
{
	using typename t_tree<A_branch>::t_branch;
	using typename t_tree<A_branch>::t_at;
	using t_tree<A_branch>::f_at;

	struct t_leaf;
	struct t_link
	{
		t_leaf* v_previous;
		t_leaf* v_next;
	};
	struct t_leaf : t_link
	{
		size_t v_size;
		char v_data[sizeof(T_value) * A_leaf];

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
		t_leaf(t_leaf* a_p, T&& a_value) : t_leaf(a_p, size_t(1))
		{
			t_array::f_construct(f_values(), std::forward<T>(a_value));
		}
		template<typename T>
		t_leaf(t_leaf* a_p, T a_first, size_t a_n) : t_leaf(a_p, a_n)
		{
			std::uninitialized_copy_n(a_first, a_n, f_values());
		}
		template<typename T>
		t_leaf(size_t a_i, T&& a_value, t_leaf* a_p) : t_leaf(a_p, (A_leaf + 1) / 2)
		{
			auto p = a_p->f_values() + A_leaf / 2;
			t_array::f_move(p, a_p->f_values() + A_leaf, f_values());
			t_array::f_construct(t_array::f_shift(a_p->f_values() + a_i, p, 1), std::forward<T>(a_value));
			a_p->v_size = A_leaf / 2 + 1;
		}
		template<typename T>
		t_leaf(t_leaf* a_p, size_t a_i, T&& a_value) : t_leaf(a_p, (A_leaf + 1) / 2)
		{
			auto p = a_p->f_values() + a_i;
			auto q = t_array::f_move(a_p->f_values() + A_leaf / 2 + 1, p, f_values());
			*q = std::forward<T>(a_value);
			t_array::f_move(p, a_p->f_values() + A_leaf, ++q);
			a_p->v_size = A_leaf / 2 + 1;
		}
		void f_unlink()
		{
			this->v_previous->v_next = this->v_next;
			this->v_next->v_previous = this->v_previous;
			delete this;
		}
		T_value* f_values()
		{
			return reinterpret_cast<T_value*>(v_data);
		}
		template<typename T>
		void f_insert(size_t a_i, T&& a_value)
		{
			auto p = f_values();
			t_array::f_construct(t_array::f_shift(p + a_i, p + this->v_size, 1), std::forward<T>(a_value));
			++this->v_size;
		}
		template<typename T>
		void f_insert(size_t a_i, T a_first, size_t a_n)
		{
			auto p = f_values();
			std::uninitialized_copy_n(a_first, a_n, t_array::f_shift(p + a_i, p + this->v_size, a_n));
			this->v_size += a_n;
		}
		template<typename T>
		void f_insert(t_leaf* a_p, size_t a_i, T&& a_value)
		{
			auto p = f_values();
			t_array::f_move(a_p->f_values() + a_p->v_size, p);
			++a_p->v_size;
			t_array::f_construct(t_array::f_unshift(p, p + a_i, 1), std::forward<T>(a_value));
		}
		template<typename T>
		void f_insert(size_t a_i, T&& a_value, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = a_p->f_values();
			t_array::f_move(t_array::f_shift(q, q + a_p->v_size, 1), p + A_leaf - 1);
			++a_p->v_size;
			t_array::f_construct(t_array::f_shift(p + a_i, p + A_leaf - 1, 1), std::forward<T>(a_value));
		}
		void f_slide(size_t a_i, size_t a_j)
		{
			if (a_i < a_j) {
				auto p = f_values();
				t_array::f_shift(p + a_i, p + v_size, a_j - a_i);
			} else {
				auto p = f_values() + a_j;
				t_array::f_unshift(p, p + v_size - a_i, a_i - a_j);
			}
		}
		void f_erase(size_t a_i, size_t a_n)
		{
			this->v_size -= a_n;
			auto p = f_values() + a_i;
			t_array::f_destruct(p, p + a_n);
			t_array::f_unshift(p, f_values() + this->v_size, a_n);
		}
		void f_erase(t_leaf* a_p, size_t a_shift, size_t a_i, size_t a_n)
		{
			this->v_size -= a_shift;
			a_p->v_size = A_leaf / 2;
			auto p = a_p->f_values();
			auto q = p + a_i;
			t_array::f_destruct(q, q + a_n);
			auto r = f_values() + this->v_size;
			t_array::f_move(r, r + a_shift, t_array::f_shift(p, q, a_shift));
			t_array::f_unshift(q + a_shift, p + A_leaf / 2, a_n - a_shift);
		}
		void f_erase(size_t a_i, size_t a_n, size_t a_shift, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = p + a_i;
			t_array::f_destruct(q, q + a_n);
			auto r = a_p->f_values();
			t_array::f_move(r, r + a_shift, t_array::f_unshift(q, p + this->v_size - a_n, a_n));
			this->v_size = A_leaf / 2;
			a_p->v_size -= a_shift;
			t_array::f_unshift(r, r + a_p->v_size, a_shift);
		}
		void f_merge(t_leaf* a_p, size_t a_i, size_t a_n)
		{
			auto p = a_p->f_values();
			auto q = p + a_i;
			t_array::f_destruct(q, q + a_n);
			t_array::f_move(q + a_n, p + a_p->v_size, t_array::f_move(p, q, f_values() + this->v_size));
			this->v_size += a_p->v_size - a_n;
		}
		void f_merge(size_t a_i, size_t a_n, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = p + a_i;
			t_array::f_destruct(q, q + a_n);
			this->v_size -= a_n;
			auto r = a_p->f_values();
			t_array::f_move(r, r + a_p->v_size, t_array::f_unshift(q, p + this->v_size, a_n));
			this->v_size += a_p->v_size;
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
			auto p = static_cast<t_leaf*>(a_node);
			this->f_destruct(p->f_values(), p->f_values() + p->v_size);
			delete p;
		}
	}
	template<typename T>
	t_at f_insert_leaf(t_at* a_head, t_at* a_tail, T&& a_value);
	template<typename T>
	t_at f_insert_leaf(t_at* a_head, t_at* a_tail, T a_first, size_t a_n);
	t_at f_erase_leaf(t_at* a_head, t_at* a_tail, size_t a_n);
	t_at f_merge_leaf(t_at* a_head, t_at* a_tail, t_leaf* a_p, size_t a_i);
	t_at f_merge_leaf(t_at* a_head, t_at* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j);

public:
	template<typename T>
	class t_iterator
	{
		friend class t_array;

		std::remove_const_t<T>* v_p;
		t_leaf* v_leaf;
		size_t v_index;

		t_iterator(const t_at& a_p, size_t a_index) : v_p(static_cast<t_leaf*>(a_p.v_node)->f_values() + a_p.v_index), v_leaf(static_cast<t_leaf*>(a_p.v_node)), v_index(a_index)
		{
		}

	public:
		typedef std::ptrdiff_t difference_type;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef std::bidirectional_iterator_tag iterator_category;

		t_iterator() = default;
		t_iterator(const t_iterator<std::remove_const_t<T>>& a_x) : v_p(a_x.v_p), v_leaf(a_x.v_leaf), v_index(a_x.v_index)
		{
		}
		t_iterator& operator=(const t_iterator<std::remove_const_t<T>>& a_x)
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
		T& operator*() const
		{
			return *this->v_p;
		}
		T* operator->() const
		{
			return this->v_p;
		}
		t_iterator& operator++()
		{
			if (++v_p == v_leaf->f_values() + v_leaf->v_size) {
				v_leaf = v_leaf->v_next;
				v_p = v_leaf->f_values();
			}
			++v_index;
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
			if (v_p == v_leaf->f_values()) {
				v_leaf = v_leaf->v_previous;
				v_p = v_leaf->v_values() + v_leaf->v_size;
			}
			--v_p;
			--v_index;
			return *this;
		}
		t_iterator operator--(int) const
		{
			auto i = *this;
			--*this;
			return i;
		}
	};
	typedef t_iterator<T_value> t_mutable_iterator;
	typedef t_iterator<const T_value> t_constant_iterator;

	t_array()
	{
		v_link.v_previous = v_link.v_next = static_cast<t_leaf*>(&v_link);
	}
	~t_array()
	{
		f_clear();
	}
	void f_clear()
	{
		if (!this->v_root) return;
		f_clear(0, this->v_root);
		this->v_root = nullptr;
		this->v_size = this->v_depth = 0;
		v_link.v_previous = v_link.v_next = static_cast<t_leaf*>(&v_link);
	}
	t_mutable_iterator f_begin()
	{
		return {{v_link.v_next, 0}, 0};
	}
	t_constant_iterator f_begin() const
	{
		return {{v_link.v_next, 0}, 0};
	}
	t_mutable_iterator f_end()
	{
		return {{&v_link, 0}, this->v_size};
	}
	t_constant_iterator f_end() const
	{
		return {{const_cast<t_link*>(&v_link), 0}, this->v_size};
	}
	template<typename T>
	t_mutable_iterator f_insert(t_constant_iterator a_i, T&& a_value)
	{
		if (++this->v_size > 1) {
			t_at path[this->v_depth + 1];
			f_at(a_i.v_index, path);
			return {f_insert_leaf(path, path + this->v_depth, std::forward<T>(a_value)), a_i.v_index};
		}
		this->v_root = new t_leaf(static_cast<t_leaf*>(&v_link), std::forward<T>(a_value));
		++this->v_depth;
		return f_begin();
	}
	template<typename T>
	t_mutable_iterator f_insert(t_constant_iterator a_i, T a_first, T a_last)
	{
		size_t n = a_last - a_first;
		this->v_size += n;
		if (this->v_depth > 0) {
			t_at path[this->f_depth(n / (A_leaf / 2))];
			f_at(a_i.v_index, path);
			return {f_insert_leaf(path, path + this->v_depth, a_first, n), a_i.v_index};
		}
		size_t m = std::min(n, A_leaf);
		this->v_root = new t_leaf(static_cast<t_leaf*>(&v_link), a_first, m);
		++this->v_depth;
		n -= m;
		if (n > 0) {
			t_at path[this->f_depth(n / (A_leaf / 2))] = {{this->v_root, m}};
			f_insert_leaf(path, path + 1, a_first + m, n);
		}
		return f_begin();
	}
	t_mutable_iterator f_erase(t_constant_iterator a_i)
	{
		--this->v_size;
		t_at path[this->v_depth];
		f_at(a_i.v_index, path);
		return {f_erase_leaf(path, path + this->v_depth, 1), a_i.v_index};
	}
	t_mutable_iterator f_erase(t_constant_iterator a_first, t_constant_iterator a_last)
	{
		size_t n = a_last.v_index - a_first.v_index;
		if (n >= this->v_size) {
			f_clear();
			return f_begin();
		}
		this->v_size -= n;
		t_at path[this->v_depth];
		size_t i = a_first.v_p - a_first.v_leaf->f_values();
		if (i + n > a_first.v_leaf->v_size) {
			n = a_first.v_leaf->v_size - i;
			size_t index = a_first.v_index + n;
			f_at(index, path);
			auto tail = path + this->v_depth - 2;
			size_t j = a_last.v_p - a_last.v_leaf->f_values();
			if (j <= 0) {
				a_last.v_leaf = a_last.v_leaf->v_previous;
				j = a_last.v_leaf->v_size;
			}
			for (size_t last = a_last.v_index - j; index < last;) {
				auto p = static_cast<t_leaf*>(tail[1].v_node);
				if (tail->v_index > 0) {
					--tail->v_index;
					tail = this->f_erase_branch(path, tail + 1, p->v_size) - 1;
					auto p = tail;
					while (p->v_index >= static_cast<t_branch*>(p->v_node)->v_size) --p;
					++p->v_index;
					while (p != tail) {
						auto q = static_cast<t_branch*>(p->v_node)->v_nodes[p->v_index];
						*++p = {q, 0};
					}
				} else {
					auto q = static_cast<t_branch*>(tail->v_node);
					std::swap(q->v_nodes[0], q->v_nodes[1]);
					tail = this->f_erase_branch(path, tail + 1, p->v_size) - 1;
				}
				tail[1].v_node = static_cast<t_branch*>(tail->v_node)->v_nodes[tail->v_index];
				last -= p->v_size;
				p->f_unlink();
			}
			if (tail->v_index > 0) return {f_merge_leaf(path, tail, a_first.v_leaf, i, a_last.v_leaf, j), a_first.v_index};
			f_erase_leaf(path, path + this->v_depth, j);
			tail = path + this->v_depth - 2;
			auto p = tail;
			while (p->v_index <= 0) --p;
			--p->v_index;
			while (p != tail) {
				auto q = static_cast<t_branch*>(p->v_node)->v_nodes[p->v_index];
				*++p = {q, static_cast<t_branch*>(q)->v_size};
			}
			*++tail = {a_first.v_leaf, i};
		} else {
			f_at(a_first.v_index, path);
		}
		return {f_erase_leaf(path, path + this->v_depth, n), a_first.v_index};
	}
	t_mutable_iterator f_at(size_t a_index)
	{
		if (a_index >= this->v_size) return f_end();
		t_at i{this->v_root, a_index};
		for (size_t n = this->v_depth; --n > 0;) i.f_step();
		return {i, a_index};
	}
	t_constant_iterator f_at(size_t a_index) const
	{
		return const_cast<t_array*>(this)->f_at(a_index);
	}
};

template<typename T_value, size_t A_leaf, size_t A_branch>
template<typename T>
typename t_array<T_value, A_leaf, A_branch>::t_at t_array<T_value, A_leaf, A_branch>::f_insert_leaf(t_at* a_head, t_at* a_tail, T&& a_value)
{
	auto p = static_cast<t_leaf*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (p->v_size < A_leaf) {
		p->f_insert(i, std::forward<T>(a_value));
		this->f_adjust(a_head, a_tail, 1);
		return {p, i};
	}
	if (a_head != a_tail) {
		auto q = static_cast<t_branch*>(a_tail[-1].v_node);
		size_t j = a_tail[-1].v_index;
		if (j > 0) {
			auto r = p->v_previous;
			if (r->v_size < A_leaf) {
				++q->v_indices[j - 1];
				this->f_adjust(a_head, a_tail, 1);
				if (i > 0) {
					p->f_insert(r, --i, std::forward<T>(a_value));
					return {p, i};
				} else {
					r->f_values()[r->v_size] = std::forward<T>(a_value);
					return {r, r->v_size++};
				}
			}
		}
		if (j < q->v_size) {
			auto r = p->v_next;
			if (r->v_size < A_leaf) {
				p->f_insert(i, std::forward<T>(a_value), r);
				--q->v_indices[j];
				this->f_adjust(a_head, a_tail, 1);
				return {p, i};
			}
		}
	}
	if (i < A_leaf / 2 + 1) {
		auto q = new t_leaf(i, std::forward<T>(a_value), p);
		this->f_insert_branch(a_head, a_tail, A_leaf / 2 + 1, 1, q, true, false);
		return {p, i};
	} else {
		auto q = new t_leaf(p, i, std::forward<T>(a_value));
		this->f_insert_branch(a_head, a_tail, A_leaf / 2 + 1, 1, q, true, true);
		return {q, i - (A_leaf / 2 + 1)};
	}
}

template<typename T_value, size_t A_leaf, size_t A_branch>
template<typename T>
typename t_array<T_value, A_leaf, A_branch>::t_at t_array<T_value, A_leaf, A_branch>::f_insert_leaf(t_at* a_head, t_at* a_tail, T a_first, size_t a_n)
{
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
	// if l.node != n
	//   while m' + n - i > N * 2
	//     |=N=| ? |=    N    =| ...
	//     |***| ? |.i.|=n-i=|.| ...
	//     m' = m' - N
	//   N < m' + n - i <= N * 2
	// N < i' + m' + n - i <= N * 2
	// if n1 exists && i' + m' + n - i + n1 <= N * 2
	//   |=  N   =| |=  N   =|
	//   |=i'=|*|=n-i=|=n1=|.|
	// else
	//   |= N =| |= N  =|
	//   |=i'=|*|=n-i=|.|
	auto p = static_cast<t_leaf*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	// if n + m <= N
	if (p->v_size + a_n <= A_leaf) {
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		p->f_insert(i, a_first, a_n);
		this->f_adjust(a_head, a_tail, a_n);
		return {p, i};
	}
	t_at l{p, i};
	auto v = p->f_values();
	size_t ii = i;
	size_t ni = p->v_size - i;
	if (a_head != a_tail) {
		auto q = static_cast<t_branch*>(a_tail[-1].v_node);
		size_t j = a_tail[-1].v_index;
		// if n0 exists
		if (j > 0) {
			auto r = p->v_previous;
			// if n0 + i < N
			if (r->v_size + i < A_leaf) {
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|.| ? |.i.|=n-i=|.| ...
				// l = {node: n0, index: n0 + i}
				this->f_move(v, v + i, r->f_values() + r->v_size);
				r->v_size += i;
				q->v_indices[j - 1] += i;
				l = {r, r->v_size};
				// if n0 + i + m <= N
				if (r->v_size + a_n <= A_leaf) {
					// |=   N    =| |=  N  =|
					// |=n0+i=|*|.| |=n-i=|.|
					std::uninitialized_copy_n(a_first, a_n, r->f_values() + r->v_size);
					r->v_size += a_n;
					q->v_indices[j - 1] += a_n;
					this->f_unshift(v, v + ni, i);
					p->v_size = ni;
					this->f_adjust(a_head, a_tail, a_n);
					return l;
				}
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|*| ? |.i.|=n-i=|.| ...
				// m' = i' + m' - N
				// i' = 0
				size_t n = A_leaf - r->v_size;
				std::uninitialized_copy_n(a_first, n, r->f_values() + r->v_size);
				a_first += n;
				a_n -= n;
				r->v_size = A_leaf;
				q->v_indices[j - 1] += n;
				this->f_adjust(a_head, a_tail, n);
				ii = 0;
				// m' + n - i > N
			} else {
				// i' = n0 + i - N
				// |=N=| |=     N      =| ...
				// |=N=| |=i'=|.|=n-i=|.| ...
				// l.index = i'
				size_t n = A_leaf - r->v_size;
				this->f_move(v, v + n, r->f_values() + r->v_size);
				r->v_size = A_leaf;
				q->v_indices[j - 1] += n;
				ii = l.v_index -= n;
				this->f_unshift(v, v + ii, n);
			}
		}
		// if i' + m' + n - i <= N
		if (ii + a_n + ni <= A_leaf) {
			// |=     N      =|
			// |=i'=|*|=n-i=|.|
			p->f_slide(i, ii + a_n);
			std::uninitialized_copy_n(a_first, a_n, v + ii);
			p->v_size = ii + a_n + ni;
			this->f_adjust(a_head, a_tail, a_n);
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
		auto r = new t_leaf(A_leaf, p);
		size_t n = A_leaf - ii;
		std::uninitialized_copy_n(a_first, n, this->f_move(v, v + ii, r->f_values()));
		a_first += n;
		a_n -= n;
		a_tail = this->f_insert_branch(a_head, a_tail, A_leaf, n, r, false, true);
		l.v_node = r;
		ii = 0;
		// m' + n - i > N
	}
	// if l.node != n
	//   while m' + n - i > N * 2
	//     |=N=| ? |=    N    =| ...
	//     |***| ? |.i.|=n-i=|.| ...
	//     m' = m' - N
	//   N < m' + n - i <= N * 2
	if (l.v_node != p) while (a_n + ni > A_leaf * 2) {
		auto r = new t_leaf(A_leaf, p);
		std::uninitialized_copy_n(a_first, A_leaf, r->f_values());
		a_first += A_leaf;
		a_n -= A_leaf;
		a_tail = this->f_insert_branch(a_head, a_tail, A_leaf, A_leaf, r, false, true);
	}
	// N < i' + m' + n - i <= N * 2
	size_t k = ii + a_n + ni;
	// if n1 exists && i' + m' + n - i + n1 <= N * 2
	if (a_head != a_tail) {
		auto q = static_cast<t_branch*>(a_tail[-1].v_node);
		size_t j = a_tail[-1].v_index;
		if (j < q->v_size) {
			auto r = p->v_next;
			if (k + r->v_size <= A_leaf * 2) {
				auto w = r->f_values();
				size_t d = k - A_leaf;
				this->f_shift(w, w + r->v_size, d);
				if (ii + a_n <= A_leaf) {
					// |=  N   =| |=  N   =|
					// |=i'=|*|=n-i=|=n1=|.|
					this->f_move(v + p->v_size - d, v + p->v_size, w);
					this->f_shift(v + i, v + p->v_size - d, ii + a_n - i);
					std::uninitialized_copy_n(a_first, a_n, v + ii);
				} else {
					// |= N  =| |=     N      =|
					// |=i'=|*****|=n-i=|=n1=|.|
					size_t e = A_leaf - ii;
					this->f_move(v + i, v + p->v_size, w + a_n - e);
					std::uninitialized_copy_n(a_first, e, v + ii);
					std::uninitialized_copy_n(a_first + e, a_n - e, w);
				}
				p->v_size = A_leaf;
				r->v_size += d;
				q->v_indices[j] -= d;
				this->f_adjust(a_head, a_tail, a_n);
				return l;
			}
		}
	}
	// |= N =| |= N  =|
	// |=i'=|*|=n-i=|.|
	auto r = new t_leaf(p, k / 2);
	if (ni > r->v_size) {
		// |=  N   =| |=N=|
		// |=i'=|*|=n-i=|.|
		auto w = v + p->v_size - r->v_size;
		this->f_move(w, w + r->v_size, r->f_values());
		this->f_shift(v + i, w, ii + a_n - i);
		std::uninitialized_copy_n(a_first, a_n, v + ii);
		p->v_size = k - r->v_size;
	} else if (a_n + ni > r->v_size) {
		// |= N  =| |=   N   =|
		// |=i'=|*****|=n-i=|.|
		auto w = r->f_values();
		size_t e = r->v_size - ni;
		this->f_move(v + i, v + p->v_size, w + e);
		std::uninitialized_copy_n(a_first, a_n - e, v + ii);
		std::uninitialized_copy_n(a_first + a_n - e, e, w);
		p->v_size = k - r->v_size;
	} else {
		// |=N=| |=    N    =|
		// |= i'  =|*|=n-i=|.|
		auto w = r->f_values() + r->v_size - ni;
		this->f_move(v + i, v + p->v_size, w);
		std::uninitialized_copy_n(a_first, a_n, w - a_n);
		p->v_size = k - r->v_size;
		this->f_move(v + p->v_size, v + ii, r->f_values());
		l = {r, ii - p->v_size};
	}
	this->f_insert_branch(a_head, a_tail, p->v_size, a_n, r, true, false);
	return l;
}

template<typename T_value, size_t A_leaf, size_t A_branch>
typename t_array<T_value, A_leaf, A_branch>::t_at t_array<T_value, A_leaf, A_branch>::f_erase_leaf(t_at* a_head, t_at* a_tail, size_t a_n)
{
	auto p = static_cast<t_leaf*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (a_head == a_tail) {
		if (p->v_size - a_n > 0) {
			p->f_erase(i, a_n);
			if (i < p->v_size) return {p, i};
		} else {
			this->v_root = nullptr;
			p->f_unlink();
			--this->v_depth;
		}
		return {&v_link, 0};
	} else if (p->v_size - a_n >= A_leaf / 2) {
		p->f_erase(i, a_n);
		this->f_adjust(a_head, a_tail, -a_n);
		return i < p->v_size ? t_at{p, i} : t_at{p->v_next, 0};
	}
	auto q = static_cast<t_branch*>(a_tail[-1].v_node);
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = p->v_previous;
		size_t n = A_leaf / 2 + a_n - p->v_size;
		if (r->v_size - n >= A_leaf / 2) {
			r->f_erase(p, n, i, a_n);
			q->v_indices[j - 1] -= n;
			this->f_adjust(a_head, a_tail, -a_n);
			i += n;
			return i < A_leaf / 2 ? t_at{p, i} : t_at{p->v_next, 0};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			n = r->v_size;
			r->f_merge(p, i, a_n);
			p->f_unlink();
			this->f_erase_branch(a_head, a_tail, a_n);
			i += n;
			return i < r->v_size ? t_at{r, i} : t_at{r->v_next, 0};
		}
	}
	auto r = p->v_next;
	size_t n = A_leaf / 2 + a_n - p->v_size;
	if (r->v_size - n >= A_leaf / 2) {
		p->f_erase(i, a_n, n, r);
		q->v_indices[j] += n;
		this->f_adjust(a_head, a_tail, -a_n);
	} else {
		p->f_merge(i, a_n, r);
		r->f_unlink();
		this->f_erase_branch(a_head, a_tail, a_n);
	}
	return {p, i};
}

template<typename T_value, size_t A_leaf, size_t A_branch>
typename t_array<T_value, A_leaf, A_branch>::t_at t_array<T_value, A_leaf, A_branch>::f_merge_leaf(t_at* a_head, t_at* a_tail, t_leaf* a_p, size_t a_i)
{
	size_t n = A_leaf / 2 - a_p->v_size;
	auto v = a_p->f_values();
	auto q = static_cast<t_branch*>(a_tail[-1].v_node);
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = a_p->v_previous;
		if (r->v_size - n >= A_leaf / 2) {
			r->v_size -= n;
			a_p->v_size = A_leaf / 2;
			auto w = r->f_values() + r->v_size;
			this->f_move(w, w + n, this->f_shift(v, v + a_p->v_size, n));
			q->v_indices[j - 1] -= n;
			a_i += n;
			return a_i < A_leaf / 2 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			a_i += r->v_size;
			this->f_move(v, v + a_p->v_size, r->f_values() + r->v_size);
			r->v_size += a_p->v_size;
			a_p->f_unlink();
			this->f_erase_branch(a_head, a_tail, 0);
			return a_i < r->v_size ? t_at{r, a_i} : t_at{r->v_next, 0};
		}
	}
	auto r = a_p->v_next;
	auto w = r->f_values();
	if (r->v_size - n >= A_leaf / 2) {
		this->f_move(w, w + n, v + a_p->v_size);
		a_p->v_size = A_leaf / 2;
		r->v_size -= n;
		this->f_unshift(w, w + r->v_size, n);
		q->v_indices[j] += n;
	} else {
		this->f_move(w, w + r->v_size, v + a_p->v_size);
		a_p->v_size += r->v_size;
		r->f_unlink();
		this->f_erase_branch(a_head, a_tail, 0);
	}
	return {a_p, a_i};
}

template<typename T_value, size_t A_leaf, size_t A_branch>
typename t_array<T_value, A_leaf, A_branch>::t_at t_array<T_value, A_leaf, A_branch>::f_merge_leaf(t_at* a_head, t_at* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j)
{
	auto v = a_p->f_values() + a_i;
	size_t a = a_p->v_size - a_i;
	this->f_destruct(v, v + a);
	auto w = a_q->f_values();
	this->f_destruct(w, w + a_j);
	size_t b = a_q->v_size - a_j;
	auto q = static_cast<t_branch*>(a_tail->v_node);
	size_t j = a_tail->v_index - 1;
	q->v_indices[j] -= a;
	size_t m = a_i + b;
	if (m >= A_leaf / 2 * 2) {
		this->f_adjust(a_head, a_tail + 1, -(a + a_j));
		if (a_i < A_leaf / 2) {
			a_p->v_size = A_leaf / 2;
			a_q->v_size = m - A_leaf / 2;
			size_t d = A_leaf / 2 - a_i;
			this->f_move(w + a_j, w + a_j + d, v);
			this->f_unshift(w, w + a_q->v_size, a_j + d);
			q->v_indices[j] += d;
			return {a_p, a_i};
		} else if (b < A_leaf / 2) {
			a_p->v_size = m - A_leaf / 2;
			a_q->v_size = A_leaf / 2;
			size_t d = A_leaf / 2 - b;
			this->f_unshift(this->f_move(a_p->f_values() + a_p->v_size, v, w), w + a_q->v_size, a_j - d);
			q->v_indices[j] -= d;
			return b > 0 ? t_at{a_q, d} : t_at{a_q->v_next, 0};
		}
		a_p->v_size = a_i;
		a_q->v_size = b;
		this->f_unshift(w, w + a_q->v_size, a_j);
		return {a_q, 0};
	}
	a_p->v_size = m;
	this->f_move(w + a_j, w + a_j + b, v);
	q->v_indices[j] += b;
	a_tail->v_index = j;
	a_q->f_unlink();
	a_tail = this->f_erase_branch(a_head, a_tail + 1, a + a_j);
	if (a_tail == a_head || m >= A_leaf / 2) return b > 0 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
	return f_merge_leaf(a_head, a_tail, a_p, a_i);
}

}

#endif
