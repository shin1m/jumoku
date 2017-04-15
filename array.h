#ifndef XTREE__ARRAY_H
#define XTREE__ARRAY_H

#include <algorithm>
#include <memory>

#include "tree.h"

namespace xtree
{

template<typename T_value, size_t A_size>
class t_array : public t_tree
{
	struct t_leaf;
	struct t_link
	{
		t_leaf* v_previous;
		t_leaf* v_next;
	};
	struct t_leaf : t_link
	{
		size_t v_size;
		char v_data[sizeof(T_value) * A_size];

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
			f_construct(f_values(), std::forward<T>(a_value));
		}
		template<typename T>
		t_leaf(t_leaf* a_p, T a_first, size_t a_n) : t_leaf(a_p, a_n)
		{
			std::uninitialized_copy_n(a_first, a_n, f_values());
		}
		template<typename T>
		t_leaf(size_t a_i, T&& a_value, t_leaf* a_p) : t_leaf(a_p, (A_size + 1) / 2)
		{
			auto p = a_p->f_values() + A_size / 2;
			f_move(p, a_p->f_values() + A_size, f_values());
			f_construct(f_shift(a_p->f_values() + a_i, p, 1), std::forward<T>(a_value));
			a_p->v_size = A_size / 2 + 1;
		}
		template<typename T>
		t_leaf(t_leaf* a_p, size_t a_i, T&& a_value) : t_leaf(a_p, (A_size + 1) / 2)
		{
			auto p = a_p->f_values() + a_i;
			auto q = f_move(a_p->f_values() + A_size / 2 + 1, p, f_values());
			*q = std::forward<T>(a_value);
			f_move(p, a_p->f_values() + A_size, ++q);
			a_p->v_size = A_size / 2 + 1;
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
			f_construct(f_shift(p + a_i, p + this->v_size, 1), std::forward<T>(a_value));
			++this->v_size;
		}
		template<typename T>
		void f_insert(size_t a_i, T a_first, size_t a_n)
		{
			auto p = f_values();
			std::uninitialized_copy_n(a_first, a_n, f_shift(p + a_i, p + this->v_size, a_n));
			this->v_size += a_n;
		}
		template<typename T>
		void f_insert(t_leaf* a_p, size_t a_i, T&& a_value)
		{
			auto p = f_values();
			f_move(a_p->f_values() + a_p->v_size, p);
			++a_p->v_size;
			f_construct(f_unshift(p, p + a_i, 1), std::forward<T>(a_value));
		}
		template<typename T>
		void f_insert(size_t a_i, T&& a_value, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = a_p->f_values();
			f_move(f_shift(q, q + a_p->v_size, 1), p + A_size - 1);
			++a_p->v_size;
			f_construct(f_shift(p + a_i, p + A_size, 1), std::forward<T>(a_value));
		}
		void f_slide(size_t a_i, size_t a_j)
		{
			if (a_i < a_j) {
				auto p = f_values();
				f_shift(p + a_i, p + v_size, a_j - a_i);
			} else {
				auto p = f_values() + a_j;
				f_unshift(p, p + v_size - a_i, a_i - a_j);
			}
		}
		void f_erase(size_t a_i)
		{
			auto p = f_values();
			f_unshift(f_destruct(p + a_i), p + --this->v_size, 1);
		}
		void f_erase(t_leaf* a_p, size_t a_i)
		{
			auto p = a_p->f_values();
			f_move(f_shift(p, f_destruct(p + a_i), 1), f_values() + --this->v_size);
		}
		void f_erase(size_t a_i, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = a_p->f_values();
			f_move(f_unshift(f_destruct(p + a_i), p + A_size / 2 - 1, 1), q);
			f_unshift(q, q + --a_p->v_size, 1);
		}
		void f_merge(t_leaf* a_p, size_t a_i)
		{
			this->v_size = A_size / 2 * 2 - 1;
			auto p = a_p->f_values();
			auto q = p + a_i;
			f_move(q + 1, p + A_size / 2, f_move(p, q, f_values() + A_size / 2));
		}
		void f_merge(size_t a_i, t_leaf* a_p)
		{
			this->v_size = A_size / 2 * 2 - 1;
			auto p = f_values();
			auto q = a_p->f_values();
			f_move(q, q + A_size / 2, f_unshift(f_destruct(p + a_i), p + A_size / 2 - 1, 1));
		}
	};
	struct t_branch
	{
		static size_t* f_copy_forward(const size_t* a_p, const size_t* a_q, size_t* a_r, int a_delta)
		{
			while (a_p != a_q) *a_r++ = *a_p++ + a_delta;
			return a_r;
		}
		static size_t* f_copy_backward(const size_t* a_p, const size_t* a_q, size_t* a_r, int a_delta)
		{
			while (a_q != a_p) *--a_r = *--a_q + a_delta;
			return a_r;
		}

		size_t v_size;
		size_t v_indices[A_size];
		void* v_nodes[A_size + 1];

		t_branch(void* a_left, size_t a_index, void* a_right) : v_size(1)
		{
			v_indices[0] = a_index;
			v_nodes[0] = a_left;
			v_nodes[1] = a_right;
		}
		t_branch(size_t a_i, size_t a_index, size_t a_delta, void* a_node, bool a_right, t_branch* a_p) : v_size(A_size / 2)
		{
			{
				size_t* p = a_p->v_indices + (A_size + 1) / 2;
				size_t* q = p - 1;
				f_copy_forward(p, a_p->v_indices + A_size, v_indices, -*q);
				a_p->v_size = (A_size + 1) / 2;
				size_t* r = a_p->v_indices + a_i;
				f_copy_backward(r, q, p, a_delta);
				*r = a_index;
			}
			auto p = a_p->v_nodes + (A_size + 1) / 2;
			std::copy(p, a_p->v_nodes + A_size + 1, v_nodes);
			*f_shift(a_p->v_nodes + a_i + (a_right ? 1 : 0), p, 1) = a_node;
		}
		t_branch(t_branch* a_p, size_t a_i, size_t a_index, size_t a_delta, void* a_node, bool a_right) : v_size(A_size / 2)
		{
			{
				size_t* p = a_p->v_indices + (A_size + 1) / 2;
				int delta = -*p;
				size_t* q = a_p->v_indices + a_i;
				size_t* r = f_copy_forward(++p, q, v_indices, delta);
				*r = a_index + delta;
				f_copy_forward(q, a_p->v_indices + A_size, ++r, delta + a_delta);
				a_p->v_size = (A_size + 1) / 2;
			}
			auto p = a_p->v_nodes + (A_size + 1) / 2 + 1;
			auto q = a_p->v_nodes + a_i + (a_right ? 1 : 0);
			auto i = std::copy(p, q, v_nodes);
			*i = a_node;
			std::copy(q, a_p->v_nodes + A_size + 1, ++i);
		}
		t_branch(t_branch* a_p, size_t a_index, size_t a_delta, void* a_node, bool a_right) : v_size(A_size / 2)
		{
			f_copy_forward(a_p->v_indices + (A_size + 1) / 2, a_p->v_indices + A_size, v_indices, -a_index + a_delta);
			a_p->v_size = (A_size + 1) / 2;
			auto p = a_p->v_nodes + (A_size + 1) / 2;
			auto q = a_p->v_nodes + A_size + 1;
			if (a_right) {
				v_nodes[0] = a_node;
				std::copy(p + 1, q, v_nodes + 1);
			} else {
				std::copy(p, q, v_nodes);
				*p = a_node;
			}
		}
		void f_insert(size_t a_i, size_t a_index, size_t a_delta, void* a_node, bool a_right)
		{
			auto p = v_nodes + this->v_size + 1;
			*f_shift(v_nodes + a_i + (a_right ? 1 : 0), p, 1) = a_node;
			{
				size_t* p = v_indices + a_i;
				size_t* q = v_indices + this->v_size;
				f_copy_backward(p, q, q + 1, a_delta);
				*p = a_index;
			}
			++this->v_size;
		}
		void f_erase(size_t a_i)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + this->v_size, 1);
			size_t* p = v_indices + a_i;
			f_copy_forward(p + 1, v_indices + this->v_size, p, -1);
			--this->v_size;
		}
		void f_erase(size_t a_base, size_t& a_index, t_branch* a_p, size_t a_i)
		{
			auto p = a_p->v_nodes;
			*f_shift(p, p + a_i + 1, 1) = v_nodes[this->v_size];
			size_t index = a_base + v_indices[--this->v_size];
			size_t delta = a_index - index;
			a_index = index;
			{
				size_t* p = a_p->v_indices + a_i;
				f_copy_backward(a_p->v_indices, p, p + 1, delta);
				a_p->v_indices[0] = delta;
				--delta;
				size_t* q = a_p->v_indices + a_p->v_size;
				while (++p != q) *p += delta;
			}
		}
		void f_erase(size_t a_i, size_t a_base, size_t& a_index, t_branch* a_p)
		{
			auto p = v_nodes;
			auto q = a_p->v_nodes;
			*f_unshift(p + a_i + 1, p + A_size / 2, 1) = *q;
			f_unshift(q, q + a_p->v_size, 1);
			size_t index = a_index - a_base;
			size_t delta = a_p->v_indices[0];
			a_index += delta;
			{
				size_t* p = v_indices + a_i;
				size_t* q = f_copy_forward(p + 1, v_indices + A_size / 2, p, -1);
				*q = index - 1;
				f_copy_forward(a_p->v_indices + 1, a_p->v_indices + a_p->v_size, a_p->v_indices, -delta);
			}
			--a_p->v_size;
		}
		void f_merge(size_t a_base, size_t a_index, t_branch* a_p, size_t a_i)
		{
			auto p = a_p->v_nodes;
			auto i = std::copy(p, p + a_i + 1, v_nodes + A_size / 2 + 1);
			std::copy(p + a_i + 2, p + A_size / 2 + 1, i);
			this->v_size = A_size / 2 * 2;
			size_t delta = a_index - a_base;
			{
				size_t* p = v_indices + A_size / 2;
				*p = delta;
				size_t* q = a_p->v_indices + a_i;
				p = f_copy_forward(a_p->v_indices, q, ++p, delta);
				f_copy_forward(++q, a_p->v_indices + A_size / 2, p, --delta);
			}
		}
		void f_merge(size_t a_i, size_t a_base, size_t a_index, t_branch* a_p)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + A_size / 2, 1);
			auto q = a_p->v_nodes;
			std::copy(q, q + A_size / 2 + 1, v_nodes + A_size / 2);
			this->v_size = A_size / 2 * 2;
			size_t delta = a_index - a_base;
			{
				size_t* p = v_indices + a_i;
				size_t* q = v_indices + A_size / 2;
				p = f_copy_forward(p + 1, q, p, -1);
				*p = --delta;
				f_copy_forward(a_p->v_indices, a_p->v_indices + A_size / 2, ++p, delta);
			}
		}
	};
	struct t_location
	{
		void* v_node;
		size_t v_index;

		t_location f_step()
		{
			auto p = static_cast<t_branch*>(v_node);
			size_t i = std::upper_bound(p->v_indices, p->v_indices + p->v_size, v_index) - p->v_indices;
			if (i > 0) v_index -= p->v_indices[i - 1];
			v_node = p->v_nodes[i];
			return {p, i};
		}
	};

	static void f_adjust(const t_location* a_head, const t_location* a_tail, int a_delta)
	{
		while (a_head != a_tail) {
			auto p = static_cast<t_branch*>((--a_tail)->v_node);
			for (size_t j = a_tail->v_index; j < p->v_size; ++j) p->v_indices[j] += a_delta;
		}
	}

	t_link v_link;

	void f_clear(size_t a_level, void* a_node)
	{
		if (++a_level < v_depth) {
			auto p = static_cast<t_branch*>(a_node);
			for (size_t i = 0; i <= p->v_size; ++i) f_clear(a_level, p->v_nodes[i]);
			delete p;
		} else {
			delete static_cast<t_leaf*>(a_node);
		}
	}
	template<typename T>
	t_location f_insert_leaf(t_location* a_head, t_location* a_tail, T&& a_value);
	template<typename T>
	t_location f_insert_leaf(t_location* a_head, t_location* a_tail, T a_first, size_t a_n);
	t_location* f_insert_branch(t_location* a_head, t_location* a_tail, size_t a_index, size_t a_delta, void* a_node, bool a_put_right, bool a_get_right);
	t_location f_erase_leaf(t_location* a_head, t_location* a_tail);
	t_location* f_erase_branch(t_location* a_head, t_location* a_tail);
	void f_at(size_t a_index, t_location* a_path) const
	{
		t_location i{v_root, a_index};
		for (size_t n = v_depth; --n > 0;) *a_path++ = i.f_step();
		*a_path = i;
	}
	void f_dump(size_t a_level, void* a_node, const std::function<void(size_t, size_t)>& a_dump) const
	{
		if (a_level + 1 >= v_depth) return;
		auto p = static_cast<t_branch*>(a_node);
		for (size_t i = 0; i < p->v_size; ++i) {
			f_dump(a_level + 1, p->v_nodes[i], a_dump);
			a_dump(a_level, p->v_indices[i]);
		}
		f_dump(a_level + 1, p->v_nodes[p->v_size], a_dump);
	}

public:
	template<typename T>
	class t_iterator
	{
		friend class t_array;

		std::remove_const_t<T>* v_p;
		t_leaf* v_leaf;
		size_t v_index;

		t_iterator(const t_location& a_p, size_t a_index) : v_p(static_cast<t_leaf*>(a_p.v_node)->f_values() + a_p.v_index), v_leaf(static_cast<t_leaf*>(a_p.v_node)), v_index(a_index)
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
		if (!v_root) return;
		f_clear(0, v_root);
		v_root = nullptr;
		v_size = v_depth = 0;
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
		return {{&v_link, 0}, v_size};
	}
	t_constant_iterator f_end() const
	{
		return {{const_cast<t_link*>(&v_link), 0}, v_size};
	}
	template<typename T>
	t_mutable_iterator f_insert(t_constant_iterator a_i, T&& a_value)
	{
		if (++v_size > 1) {
			t_location path[v_depth + 1];
			f_at(a_i.v_index, path);
			return {f_insert_leaf(path, path + v_depth, std::forward<T>(a_value)), a_i.v_index};
		}
		v_root = new t_leaf(static_cast<t_leaf*>(&v_link), std::forward<T>(a_value));
		++v_depth;
		return f_begin();
	}
	template<typename T>
	t_mutable_iterator f_insert(t_constant_iterator a_i, T a_first, T a_last)
	{
		size_t n = a_last - a_first;
		v_size += n;
		if (v_depth > 0) {
			size_t depth = v_depth;
			for (size_t d = n; d > 0; d /= A_size / 2) ++depth;
			t_location path[depth];
			f_at(a_i.v_index, path);
			return {f_insert_leaf(path, path + v_depth, a_first, n), a_i.v_index};
		}
		size_t m = std::min(n, A_size);
		v_root = new t_leaf(static_cast<t_leaf*>(&v_link), a_first, m);
		++v_depth;
		n -= m;
		if (n > 0) {
			size_t depth = v_depth;
			for (size_t d = n; d > 0; d /= A_size / 2) ++depth;
			t_location path[depth] = {{v_root, m}};
			f_insert_leaf(path, path + v_depth, a_first + m, n);
		}
		return f_begin();
	}
	t_mutable_iterator f_erase(t_constant_iterator a_i)
	{
		t_location path[v_depth];
		f_at(a_i.v_index, path);
		auto p = f_erase_leaf(path, path + v_depth);
		--v_size;
		auto q = static_cast<t_leaf*>(p.v_node);
		return {p.v_index < q->v_size ? p : t_location{q->v_next, 0}, a_i.v_index};
	}
	t_mutable_iterator f_at(size_t a_index)
	{
		if (a_index >= v_size) return f_end();
		t_location i{v_root, a_index};
		for (size_t n = v_depth; --n > 0;) i.f_step();
		return {i, a_index};
	}
	t_constant_iterator f_at(size_t a_index) const
	{
		return const_cast<t_array*>(this)->f_at(a_index);
	}
	void f_dump(const std::function<void(size_t, size_t)>& a_dump) const
	{
		f_dump(0, v_root, a_dump);
	}
};

template<typename T_value, size_t A_size>
template<typename T>
typename t_array<T_value, A_size>::t_location t_array<T_value, A_size>::f_insert_leaf(t_location* a_head, t_location* a_tail, T&& a_value)
{
	auto p = static_cast<t_leaf*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (p->v_size < A_size) {
		p->f_insert(i, std::forward<T>(a_value));
		f_adjust(a_head, a_tail, 1);
		return {p, i};
	}
	if (a_head != a_tail) {
		auto q = static_cast<t_branch*>(a_tail[-1].v_node);
		size_t j = a_tail[-1].v_index;
		if (j > 0) {
			auto r = p->v_previous;
			if (r->v_size < A_size) {
				++q->v_indices[j - 1];
				f_adjust(a_head, a_tail, 1);
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
			if (r->v_size < A_size) {
				p->f_insert(i, std::forward<T>(a_value), r);
				--q->v_indices[j];
				f_adjust(a_head, a_tail, 1);
				return {p, i};
			}
		}
	}
	if (i < (A_size + 1) / 2) {
		auto q = new t_leaf(i, std::forward<T>(a_value), p);
		f_insert_branch(a_head, a_tail, (A_size + 1) / 2, 1, q, true, false);
		return {p, i};
	} else {
		auto q = new t_leaf(p, i, std::forward<T>(a_value));
		f_insert_branch(a_head, a_tail, (A_size + 1) / 2, 1, q, true, true);
		return {q, i - (A_size + 1) / 2};
	}
}

template<typename T_value, size_t A_size>
template<typename T>
typename t_array<T_value, A_size>::t_location t_array<T_value, A_size>::f_insert_leaf(t_location* a_head, t_location* a_tail, T a_first, size_t a_n)
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
	if (p->v_size + a_n <= A_size) {
		// |=     N     =|
		// |=i=|*|=n-i=|.|
		p->f_insert(i, a_first, a_n);
		f_adjust(a_head, a_tail, a_n);
		return {p, i};
	}
	t_location l{p, i};
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
			if (r->v_size + i < A_size) {
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|.| ? |.i.|=n-i=|.| ...
				// l = {node: n0, index: n0 + i}
				f_move(v, v + i, r->f_values() + r->v_size);
				r->v_size += i;
				q->v_indices[j - 1] += i;
				l = {r, r->v_size};
				// if n0 + i + m <= N
				if (r->v_size + a_n <= A_size) {
					// |=   N    =| |=  N  =|
					// |=n0+i=|*|.| |=n-i=|.|
					std::uninitialized_copy_n(a_first, a_n, r->f_values() + r->v_size);
					r->v_size += a_n;
					q->v_indices[j - 1] += a_n;
					f_unshift(v, v + ni, i);
					p->v_size = ni;
					f_adjust(a_head, a_tail, a_n);
					return l;
				}
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|*| ? |.i.|=n-i=|.| ...
				// m' = i' + m' - N
				// i' = 0
				size_t n = A_size - r->v_size;
				std::uninitialized_copy_n(a_first, n, r->f_values() + r->v_size);
				a_first += n;
				a_n -= n;
				r->v_size = A_size;
				q->v_indices[j - 1] += n;
				f_adjust(a_head, a_tail, n);
				ii = 0;
				// m' + n - i > N
			} else {
				// i' = n0 + i - N
				// |=N=| |=     N      =| ...
				// |=N=| |=i'=|.|=n-i=|.| ...
				// l.index = i'
				size_t n = A_size - r->v_size;
				f_move(v, v + n, r->f_values() + r->v_size);
				r->v_size = A_size;
				q->v_indices[j - 1] += n;
				ii = l.v_index -= n;
				f_unshift(v, v + ii, n);
			}
		}
		// if i' + m' + n - i <= N
		if (ii + a_n + ni <= A_size) {
			// |=     N      =|
			// |=i'=|*|=n-i=|.|
			p->f_slide(i, ii + a_n);
			std::uninitialized_copy_n(a_first, a_n, v + ii);
			p->v_size = ii + a_n + ni;
			f_adjust(a_head, a_tail, a_n);
			return l;
		}
	}
	// m + n > N && n0 >= N / 2
	// m' + n - i (= n0 + m + n - N) > N / 2
	// if l.node == n && i' + m' + n - i > N * 2
	if (l.v_node == p && ii + a_n + ni > A_size * 2) {
		// |= N  =| ? |=    N    =| ...
		// |=i'=|*| ? |.i.|=n-i=|.| ...
		// m' = i' + m' - N
		// l.node = ?
		// i' = 0
		auto r = new t_leaf(A_size, p);
		size_t n = A_size - ii;
		std::uninitialized_copy_n(a_first, n, f_move(v, v + ii, r->f_values()));
		a_first += n;
		a_n -= n;
		a_tail = f_insert_branch(a_head, a_tail, A_size, n, r, false, true);
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
	if (l.v_node != p) while (a_n + ni > A_size * 2) {
		auto r = new t_leaf(A_size, p);
		std::uninitialized_copy_n(a_first, A_size, r->f_values());
		a_first += A_size;
		a_n -= A_size;
		a_tail = f_insert_branch(a_head, a_tail, A_size, A_size, r, false, true);
	}
	// N < i' + m' + n - i <= N * 2
	size_t k = ii + a_n + ni;
	// if n1 exists && i' + m' + n - i + n1 <= N * 2
	if (a_head != a_tail) {
		auto q = static_cast<t_branch*>(a_tail[-1].v_node);
		size_t j = a_tail[-1].v_index;
		if (j < q->v_size) {
			auto r = p->v_next;
			if (k + r->v_size <= A_size * 2) {
				auto w = r->f_values();
				size_t d = k - A_size;
				f_shift(w, w + r->v_size, d);
				if (ii + a_n <= A_size) {
					// |=  N   =| |=  N   =|
					// |=i'=|*|=n-i=|=n1=|.|
					f_move(v + p->v_size - d, v + p->v_size, w);
					f_shift(v + i, v + p->v_size - d, ii + a_n - i);
					std::uninitialized_copy_n(a_first, a_n, v + ii);
				} else {
					// |= N  =| |=     N      =|
					// |=i'=|*****|=n-i=|=n1=|.|
					size_t e = A_size - ii;
					f_move(v + i, v + p->v_size, w + a_n - e);
					std::uninitialized_copy_n(a_first, e, v + ii);
					std::uninitialized_copy_n(a_first + e, a_n - e, w);
				}
				p->v_size = A_size;
				r->v_size += d;
				q->v_indices[j] -= d;
				f_adjust(a_head, a_tail, a_n);
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
		f_move(w, w + r->v_size, r->f_values());
		f_shift(v + i, w, ii + a_n - i);
		std::uninitialized_copy_n(a_first, a_n, v + ii);
		p->v_size = k - r->v_size;
	} else if (a_n + ni > r->v_size) {
		// |= N  =| |=   N   =|
		// |=i'=|*****|=n-i=|.|
		auto w = r->f_values();
		size_t e = r->v_size - ni;
		f_move(v + i, v + p->v_size, w + e);
		std::uninitialized_copy_n(a_first, a_n - e, v + ii);
		std::uninitialized_copy_n(a_first + a_n - e, e, w);
		p->v_size = k - r->v_size;
	} else {
		// |=N=| |=    N    =|
		// |= i'  =|*|=n-i=|.|
		auto w = r->f_values() + r->v_size - ni;
		f_move(v + i, v + p->v_size, w);
		std::uninitialized_copy_n(a_first, a_n, w - a_n);
		p->v_size = k - r->v_size;
		f_move(v + p->v_size, v + ii, r->f_values());
		l = {r, ii - p->v_size};
	}
	f_insert_branch(a_head, a_tail, p->v_size, a_n, r, true, false);
	return l;
}

template<typename T_value, size_t A_size>
typename t_array<T_value, A_size>::t_location* t_array<T_value, A_size>::f_insert_branch(t_location* a_head, t_location* a_tail, size_t a_index, size_t a_delta, void* a_node, bool a_put_right, bool a_get_right)
{
	if (a_head == a_tail) {
		v_root = a_put_right ? new t_branch(v_root, a_index, a_node) : new t_branch(a_node, a_index, v_root);
		++v_depth;
		*a_tail = {v_root, a_get_right ? 1u : 0u};
		return ++a_tail;
	}
	auto p = static_cast<t_branch*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (i > 0) a_index += p->v_indices[i - 1];
	if (p->v_size < A_size) {
		p->f_insert(i, a_index, a_delta, a_node, a_put_right);
		f_adjust(a_head, a_tail, a_delta);
		if (a_get_right) ++i;
		*a_tail = {p, i};
	} else if (i < (A_size + 1) / 2) {
		size_t index = p->v_indices[(A_size + 1) / 2 - 1];
		//      p
		//  i i I i i
		// x X x x x x
		auto q = new t_branch(i, a_index, a_delta, a_node, a_put_right, p);
		//    p      q
		//  i i i   i i
		// x X X x x x x
		a_tail = f_insert_branch(a_head, a_tail, index + a_delta, a_delta, q, true, false);
		if (a_get_right) ++i;
		*a_tail = {p, i};
	} else if (i > (A_size + 1) / 2) {
		size_t index = p->v_indices[(A_size + 1) / 2];
		//      p
		//  i i i i I
		// x x x x X x
		auto q = new t_branch(p, i, a_index, a_delta, a_node, a_put_right);
		//    p      q
		//  i i i   i i
		// x x x x X X x
		a_tail = f_insert_branch(a_head, a_tail, index, a_delta, q, true, true);
		if (a_get_right) ++i;
		*a_tail = {q, i - (A_size + 1) / 2 - 1};
	} else {
		//      p
		//  i i i I i
		// x x x X x x
		auto q = new t_branch(p, a_index, a_delta, a_node, a_put_right);
		//    p      q
		//  i i i   i i
		// x x x X X x x
		a_tail = f_insert_branch(a_head, a_tail, a_index, a_delta, q, true, a_get_right);
		*a_tail = a_get_right ? t_location{q, 0} : t_location{p, (A_size + 1) / 2};
	}
	return ++a_tail;
}

template<typename T_value, size_t A_size>
typename t_array<T_value, A_size>::t_location t_array<T_value, A_size>::f_erase_leaf(t_location* a_head, t_location* a_tail)
{
	auto p = static_cast<t_leaf*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (a_head == a_tail) {
		if (p->v_size > 1) {
			p->f_erase(i);
			return {p, i};
		} else {
			v_root = nullptr;
			p->f_unlink();
			--v_depth;
			return {&v_link, 0};
		}
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_head, a_tail, -1);
		return {p, i};
	}
	auto q = static_cast<t_branch*>(a_tail[-1].v_node);
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = p->v_previous;
		if (r->v_size > A_size / 2) {
			r->f_erase(p, i);
			--q->v_indices[j - 1];
			f_adjust(a_head, a_tail, -1);
			return {p, i + 1};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			r->f_merge(p, i);
			p->f_unlink();
			f_erase_branch(a_head, a_tail);
			return {r, i + A_size / 2};
		}
	}
	auto r = p->v_next;
	if (r->v_size > A_size / 2) {
		p->f_erase(i, r);
		++q->v_indices[j];
		f_adjust(a_head, a_tail, -1);
	} else {
		p->f_merge(i, r);
		r->f_unlink();
		f_erase_branch(a_head, a_tail);
	}
	return {p, i};
}

template<typename T_value, size_t A_size>
typename t_array<T_value, A_size>::t_location* t_array<T_value, A_size>::f_erase_branch(t_location* a_head, t_location* a_tail)
{
	auto p = static_cast<t_branch*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (a_head == a_tail) {
		if (p->v_size > 1) {
			p->f_erase(i);
			*a_tail++ = {p, i};
		} else {
			v_root = p->v_nodes[0];
			delete p;
			--v_depth;
		}
		return a_tail;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_head, a_tail, -1);
		*a_tail = {p, i};
		return ++a_tail;
	}
	auto q = static_cast<t_branch*>(a_tail[-1].v_node);
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = static_cast<t_branch*>(q->v_nodes[j - 1]);
		if (r->v_size > A_size / 2) {
			size_t base = j > 1 ? q->v_indices[j - 2] : 0;
			r->f_erase(base, q->v_indices[j - 1], p, i);
			f_adjust(a_head, a_tail, -1);
			*a_tail = {p, i + 1};
			return ++a_tail;
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			r->f_merge(j > 0 ? q->v_indices[j - 1] : 0, q->v_indices[j], p, i);
			delete p;
			a_tail = f_erase_branch(a_head, a_tail);
			*a_tail = {r, i + A_size / 2 + 1};
			return ++a_tail;
		}
	}
	auto r = static_cast<t_branch*>(q->v_nodes[j + 1]);
	size_t base = j > 0 ? q->v_indices[j - 1] : 0;
	if (r->v_size > A_size / 2) {
		p->f_erase(i, base, q->v_indices[j], r);
		f_adjust(a_head, a_tail, -1);
	} else {
		p->f_merge(i, base, q->v_indices[j], r);
		delete r;
		a_tail = f_erase_branch(a_head, a_tail);
	}
	*a_tail = {p, i};
	return ++a_tail;
}

}

#endif
