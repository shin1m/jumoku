#ifndef JUMOKU__ARRAY_H
#define JUMOKU__ARRAY_H

#include "tree.h"

#include <memory>

namespace jumoku
{

struct t_traits
{
	typedef size_t t_index;
	typedef int t_delta;
	struct t_default
	{
		constexpr size_t operator()(size_t a_index) const
		{
			return a_index;
		}
	};

	template<typename T>
	static constexpr size_t f_index(size_t a_n, T)
	{
		return a_n;
	}
	template<typename T>
	static constexpr void f_add(T& a_value, size_t a_index)
	{
	}
	template<typename T>
	static constexpr T& f_get(T*, T* a_p)
	{
		return *a_p;
	}
	template<typename T>
	static constexpr size_t f_delta(T*, T*)
	{
		return 1;
	}
};

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits = t_traits>
class t_array : public t_tree<T_traits, A_branch>
{
	using typename t_tree<T_traits, A_branch>::t_branch;
	using typename t_tree<T_traits, A_branch>::t_via;
	using typename t_tree<T_traits, A_branch>::t_at;
	using t_tree<T_traits, A_branch>::f_at;
	using t_index = typename T_traits::t_index;
	using t_delta = typename T_traits::t_delta;

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

		t_delta f_place(size_t a_i, size_t a_n)
		{
			auto p = f_values() + a_i;
			auto delta = T_traits::f_index(a_n, *p);
			if (a_i > 0) T_traits::f_add(*p, T_traits::f_index(0, p[-1]));
			while (--a_n > 0) {
				delta += T_traits::f_index(0, *++p);
				T_traits::f_add(*p, T_traits::f_index(0, p[-1]));
			}
			return delta;
		}
		t_delta f_delta(size_t a_i, size_t a_n)
		{
			auto p = f_values() + a_i;
			auto delta = T_traits::f_index(a_n, p[a_n - 1]);
			if (a_i > 0) delta -= T_traits::f_index(0, p[-1]);
			return delta;
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
		t_leaf(size_t a_i, T&& a_value, t_leaf* a_p) : t_leaf(a_p, (A_leaf + 1) / 2)
		{
			auto p = a_p->f_values() + A_leaf / 2;
			f_move(p, a_p->f_values() + A_leaf, f_values());
			f_add(f_values(), f_values() + (A_leaf + 1) / 2, -T_traits::f_index(0, p[-1]));
			auto q = f_construct(f_shift(a_p->f_values() + a_i, p, 1), std::forward<T>(a_value));
			a_p->v_size = A_leaf / 2 + 1;
			f_add(q + 1, p + 1, a_p->f_place(a_i, 1));
		}
		template<typename T>
		t_leaf(t_leaf* a_p, size_t a_i, T&& a_value) : t_leaf(a_p, (A_leaf + 1) / 2)
		{
			auto p = a_p->f_values() + a_i;
			auto q = f_move(a_p->f_values() + A_leaf / 2 + 1, p, f_values());
			*q = std::forward<T>(a_value);
			f_move(p, a_p->f_values() + A_leaf, q + 1);
			a_p->v_size = A_leaf / 2 + 1;
			auto delta = -T_traits::f_index(0, a_p->f_values()[A_leaf / 2]);
			f_add(f_values(), q, delta);
			delta += f_place(a_i - (A_leaf / 2 + 1), 1);
			f_add(q + 1, f_values() + (A_leaf + 1) / 2, delta);
		}
		template<typename T>
		t_leaf(t_leaf* a_p, T a_first, size_t a_n) : t_leaf(a_p, a_n)
		{
			std::uninitialized_copy_n(a_first, a_n, f_values());
			f_place(0, a_n);
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
			f_add(p + a_i + 1, p + this->v_size, f_place(a_i, 1));
		}
		template<typename T>
		void f_insert(t_leaf* a_p, size_t a_i, T&& a_value)
		{
			auto p = f_values();
			auto q = a_p->f_values() + a_p->v_size;
			f_move(q, p);
			++a_p->v_size;
			f_construct(f_unshift(p, p + a_i, 1), std::forward<T>(a_value));
			auto delta = -T_traits::f_index(0, *q);
			T_traits::f_add(*q, T_traits::f_index(0, q[-1]));
			f_add(p, p + a_i, delta);
			f_add(p + a_i + 1, p + A_leaf, delta + f_place(a_i, 1));
		}
		template<typename T>
		void f_insert(size_t a_i, T&& a_value, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = a_p->f_values();
			f_move(f_shift(q, q + a_p->v_size, 1), p + A_leaf - 1);
			T_traits::f_add(*q, -T_traits::f_index(0, p[A_leaf - 2]));
			++a_p->v_size;
			f_construct(f_shift(p + a_i, p + A_leaf - 1, 1), std::forward<T>(a_value));
			f_add(q + 1, q + a_p->v_size, T_traits::f_index(0, *q));
			f_add(p + a_i + 1, p + A_leaf, f_place(a_i, 1));
		}
		template<typename T>
		t_delta f_insert(size_t a_i, T a_first, size_t a_n)
		{
			auto p = f_values();
			std::uninitialized_copy_n(a_first, a_n, f_shift(p + a_i, p + this->v_size, a_n));
			this->v_size += a_n;
			auto delta = f_place(a_i, a_n);
			f_add(p + a_i + a_n, p + this->v_size, delta);
			return delta;
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
		t_delta f_erase(size_t a_i, size_t a_n)
		{
			this->v_size -= a_n;
			auto delta = f_delta(a_i, a_n);
			auto p = f_values() + a_i;
			f_destruct(p, p + a_n);
			auto q = f_values() + this->v_size;
			f_unshift(p, q, a_n);
			f_add(p, q, -delta);
			return delta;
		}
		t_delta f_erase(t_leaf* a_p, size_t a_shift, size_t a_i, size_t a_n)
		{
			this->v_size -= a_shift;
			a_p->v_size = A_leaf / 2;
			auto delta = a_p->f_delta(a_i, a_n);
			auto p = a_p->f_values();
			auto q = p + a_i;
			f_destruct(q, q + a_n);
			auto r = f_values() + this->v_size;
			f_move(r, r + a_shift, f_shift(p, q, a_shift));
			f_unshift(q + a_shift, p + A_leaf / 2, a_n - a_shift);
			f_add(p, p + a_shift, -T_traits::f_index(0, r[-1]));
			auto d = T_traits::f_index(0, p[a_shift - 1]);
			f_add(p + a_shift, q + a_shift, d);
			f_add(q + a_shift, p + A_leaf / 2, d - delta);
			return delta;
		}
		t_delta f_erase(size_t a_i, size_t a_n, size_t a_shift, t_leaf* a_p)
		{
			auto delta = f_delta(a_i, a_n);
			auto p = f_values() + a_i;
			f_destruct(p, p + a_n);
			auto q = f_values() + this->v_size - a_n;
			auto r = a_p->f_values();
			auto d = -T_traits::f_index(0, r[a_shift - 1]);
			f_move(r, r + a_shift, f_unshift(p, q, a_n));
			f_add(p, q, -delta);
			if (this->v_size - a_n > 0) f_add(q, f_values() + A_leaf / 2, T_traits::f_index(0, q[-1]));
			this->v_size = A_leaf / 2;
			a_p->v_size -= a_shift;
			f_unshift(r, r + a_p->v_size, a_shift);
			f_add(r, r + a_p->v_size, d);
			return delta;
		}
		t_delta f_merge(t_leaf* a_p, size_t a_i, size_t a_n)
		{
			auto delta = a_p->f_delta(a_i, a_n);
			auto p = a_p->f_values();
			auto q = p + a_i;
			f_destruct(q, q + a_n);
			auto r = f_values() + this->v_size;
			f_move(q + a_n, p + a_p->v_size, f_move(p, q, r));
			this->v_size += a_p->v_size - a_n;
			auto d = T_traits::f_index(0, r[-1]);
			f_add(r, r + a_i, d);
			f_add(r + a_i, f_values() + this->v_size, d - delta);
			return delta;
		}
		t_delta f_merge(size_t a_i, size_t a_n, t_leaf* a_p)
		{
			auto delta = f_delta(a_i, a_n);
			auto p = f_values() + a_i;
			f_destruct(p, p + a_n);
			this->v_size -= a_n;
			auto q = f_values() + this->v_size;
			auto r = a_p->f_values();
			f_move(r, r + a_p->v_size, f_unshift(p, q, a_n));
			f_add(p, q, -delta);
			if (this->v_size > 0) f_add(q, q + a_p->v_size, T_traits::f_index(0, q[-1]));
			this->v_size += a_p->v_size;
			return delta;
		}
	};

	static T_value* f_add(T_value* a_i, T_value* a_j, const t_delta& a_n)
	{
		while (a_i != a_j) T_traits::f_add(*a_i++, a_n);
		return a_i;
	}

	t_link v_link;

	void f_clear(size_t a_level, void* a_node)
	{
		if (++a_level < this->v_depth) {
			auto p = static_cast<t_branch*>(a_node);
			for (size_t i = 0; i <= p->v_size; ++i) f_clear(a_level, p->v_nodes[i]);
			delete p;
		} else {
			auto p = static_cast<t_leaf*>(a_node);
			f_destruct(p->f_values(), p->f_values() + p->v_size);
			delete p;
		}
	}
	template<typename T>
	t_at f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T&& a_value);
	template<typename T>
	t_at f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T a_first, size_t a_n);
	t_at f_erase_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, size_t a_n);
	t_at f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i);
	t_at f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j);

public:
	template<typename T>
	class t_iterator
	{
		friend class t_array;

		std::remove_const_t<T>* v_p;
		t_leaf* v_leaf;
		t_index v_index;

		t_iterator(const t_at& a_p, const t_index& a_index) : v_p(static_cast<t_leaf*>(a_p.v_node)->f_values() + a_p.v_index), v_leaf(static_cast<t_leaf*>(a_p.v_node)), v_index(a_index)
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
		decltype(T_traits::f_get(v_p, v_p)) operator*() const
		{
			return T_traits::f_get(v_leaf->f_values(), v_p);
		}
		T* operator->() const
		{
			return v_p;
		}
		const t_index& f_index() const
		{
			return v_index;
		}
		t_index f_delta() const
		{
			return T_traits::f_delta(v_leaf->f_values(), v_p);
		}
		t_iterator& operator++()
		{
			v_index += f_delta();
			if (++v_p == v_leaf->f_values() + v_leaf->v_size) {
				v_leaf = v_leaf->v_next;
				v_p = v_leaf->f_values();
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
			if (v_p == v_leaf->f_values()) {
				v_leaf = v_leaf->v_previous;
				v_p = v_leaf->f_values() + v_leaf->v_size;
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
		this->v_depth = 0;
		this->v_size = {};
		v_link.v_previous = v_link.v_next = static_cast<t_leaf*>(&v_link);
	}
	t_mutable_iterator f_begin()
	{
		return {{v_link.v_next, 0}, t_index{}};
	}
	t_constant_iterator f_begin() const
	{
		return {{v_link.v_next, 0}, t_index{}};
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
		this->v_size += T_traits::f_index(1, a_value);
		if (this->v_root) {
			t_via path[this->v_depth];
			auto at = f_at(a_i.v_index, path, typename T_traits::t_default());
			return {f_insert_leaf(path, path + this->v_depth - 1, at, std::forward<T>(a_value)), a_i.v_index};
		}
		auto p = new t_leaf(static_cast<t_leaf*>(&v_link), 1);
		f_construct(p->f_values(), std::forward<T>(a_value));
		this->v_root = p;
		++this->v_depth;
		return f_begin();
	}
	template<typename T>
	t_mutable_iterator f_insert(t_constant_iterator a_i, T a_first, T a_last)
	{
		size_t n = a_last - a_first;
		if (n <= 0) return {{a_i.v_leaf, size_t(a_i.v_p - a_i.v_leaf->f_values())}, a_i.v_index};
		if (this->v_root) {
			t_via path[this->f_depth(n / (A_leaf / 2))];
			auto at = f_at(a_i.v_index, path, typename T_traits::t_default());
			return {f_insert_leaf(path, path + this->v_depth - 1, at, a_first, n), a_i.v_index};
		}
		size_t m = std::min(n, A_leaf);
		this->v_root = new t_leaf(static_cast<t_leaf*>(&v_link), a_first, m);
		++this->v_depth;
		this->v_size += T_traits::f_index(m, static_cast<t_leaf*>(this->v_root)->f_values()[m - 1]);
		n -= m;
		if (n > 0) {
			t_via path[this->f_depth(n / (A_leaf / 2))];
			f_insert_leaf(path, path, {this->v_root, m}, a_first + m, n);
		}
		return f_begin();
	}
	t_mutable_iterator f_erase(t_constant_iterator a_i)
	{
		t_via path[this->v_depth - 1];
		auto at = f_at(a_i.v_index, path, typename T_traits::t_default());
		return {f_erase_leaf(path, path + this->v_depth - 1, at, 1), a_i.v_index};
	}
	t_mutable_iterator f_erase(t_constant_iterator a_first, t_constant_iterator a_last)
	{
		typename T_traits::t_default use;
		size_t n = use(a_last.v_index) - use(a_first.v_index);
		if (n >= use(this->v_size)) {
			f_clear();
			return f_begin();
		}
		t_via path[this->v_depth - 1];
		t_at at;
		size_t i = a_first.v_p - a_first.v_leaf->f_values();
		if (i + n > a_first.v_leaf->v_size) {
			n = a_first.v_leaf->v_size - i;
			size_t index = use(a_first.v_index) + n;
			at = f_at(T_traits::f_index(index, T_value{}), path, use);
			auto tail = path + this->v_depth - 2;
			size_t j = a_last.v_p - a_last.v_leaf->f_values();
			if (j <= 0) {
				a_last.v_leaf = a_last.v_leaf->v_previous;
				j = a_last.v_leaf->v_size;
			}
			for (size_t last = use(a_last.v_index) - j; index < last;) {
				auto p = static_cast<t_leaf*>(at.v_node);
				auto delta = T_traits::f_index(p->v_size, p->f_values()[p->v_size - 1]);
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
private:
	static void f_finalize(typename T_traits::t_default&, t_at&)
	{
	}
	template<typename T_use>
	static void f_finalize(T_use& a_use, t_at& i)
	{
		auto p = static_cast<t_leaf*>(i.v_node);
		auto q = p->f_values();
		i.v_index = std::upper_bound(q, q + p->v_size, i.v_index, [&a_use](size_t a_x, const T_value& a_y)
		{
			return a_x < a_use(T_traits::f_index(1, a_y));
		}) - q;
	}
public:
	template<typename T_use>
	t_mutable_iterator f_at(size_t a_index, T_use a_use)
	{
		if (a_index >= a_use(this->v_size)) return f_end();
		t_at i{this->v_root, a_index};
		t_index index{};
		for (size_t n = this->v_depth; --n > 0;) {
			auto p = i.f_step(a_use);
			if (p.v_index > 0) index += p.v_node->v_indices[p.v_index - 1];
		}
		f_finalize(a_use, i);
		if (i.v_index > 0) index += T_traits::f_index(i.v_index, static_cast<t_leaf*>(i.v_node)->f_values()[i.v_index - 1]);
		return {i, index};
	}
	template<typename T_use>
	t_constant_iterator f_at(size_t a_index, T_use a_use) const
	{
		return const_cast<t_array*>(this)->f_at(a_index, std::forward<T_use>(a_use));
	}
	t_mutable_iterator f_at(size_t a_index)
	{
		return f_at(a_index, typename T_traits::t_default());
	}
	t_constant_iterator f_at(size_t a_index) const
	{
		return const_cast<t_array*>(this)->f_at(a_index);
	}
};

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits>
template<typename T>
typename t_array<T_value, A_leaf, A_branch, T_traits>::t_at t_array<T_value, A_leaf, A_branch, T_traits>::f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T&& a_value)
{
	auto p = static_cast<t_leaf*>(a_at.v_node);
	size_t i = a_at.v_index;
	auto delta = T_traits::f_index(1, a_value);
	if (p->v_size < A_leaf) {
		this->f_adjust(a_head, a_tail, delta);
		p->f_insert(i, std::forward<T>(a_value));
		return {p, i};
	}
	if (a_head != a_tail) {
		auto q = a_tail[-1].v_node;
		size_t j = a_tail[-1].v_index;
		if (j > 0) {
			auto r = p->v_previous;
			if (r->v_size < A_leaf) {
				this->f_adjust(a_head, a_tail, delta);
				if (i > 0) {
					q->v_indices[j - 1] += T_traits::f_index(1, *p->f_values());
					p->f_insert(r, --i, std::forward<T>(a_value));
					return {p, i};
				} else {
					q->v_indices[j - 1] += delta;
					auto p = f_construct(r->f_values() + r->v_size, std::forward<T>(a_value));
					T_traits::f_add(*p, T_traits::f_index(0, p[-1]));
					return {r, r->v_size++};
				}
			}
		}
		if (j < q->v_size) {
			auto r = p->v_next;
			if (r->v_size < A_leaf) {
				p->f_insert(i, std::forward<T>(a_value), r);
				q->v_indices[j] -= T_traits::f_index(1, *r->f_values());
				this->f_adjust(a_head, a_tail, delta);
				return {p, i};
			}
		}
	}
	if (i < A_leaf / 2 + 1) {
		auto q = new t_leaf(i, std::forward<T>(a_value), p);
		this->f_insert_branch(a_head, a_tail, T_traits::f_index(A_leaf / 2 + 1, p->f_values()[A_leaf / 2]), delta, q, true, false);
		return {p, i};
	} else {
		auto q = new t_leaf(p, i, std::forward<T>(a_value));
		this->f_insert_branch(a_head, a_tail, T_traits::f_index(A_leaf / 2 + 1, p->f_values()[A_leaf / 2]), delta, q, true, true);
		return {q, i - (A_leaf / 2 + 1)};
	}
}

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits>
template<typename T>
typename t_array<T_value, A_leaf, A_branch, T_traits>::t_at t_array<T_value, A_leaf, A_branch, T_traits>::f_insert_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, T a_first, size_t a_n)
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
	auto v = p->f_values();
	size_t ii = i;
	size_t ni = p->v_size - i;
	auto di = i > 0 ? T_traits::f_index(i, v[i - 1]) : t_index{};
	t_delta dii{};
	if (a_head != a_tail) {
		auto q = a_tail[-1].v_node;
		size_t j = a_tail[-1].v_index;
		// if n0 exists
		if (j > 0) {
			auto r = p->v_previous;
			// if n0 + i < N
			if (r->v_size + i < A_leaf) {
				// |=  N   =| ? |=    N    =| ...
				// |=n0+i=|.| ? |.i.|=n-i=|.| ...
				// l = {node: n0, index: n0 + i}
				dii = di;
				auto w = r->f_values() + r->v_size;
				f_move(v, v + i, w);
				f_add(w, w + i, T_traits::f_index(0, w[-1]));
				r->v_size += i;
				q->v_indices[j - 1] += dii;
				l = {r, r->v_size};
				// if n0 + i + m <= N
				if (r->v_size + a_n <= A_leaf) {
					// |=   N    =| |=  N  =|
					// |=n0+i=|*|.| |=n-i=|.|
					std::uninitialized_copy_n(a_first, a_n, w + i);
					auto delta = r->f_place(r->v_size, a_n);
					r->v_size += a_n;
					q->v_indices[j - 1] += delta;
					f_unshift(v, v + ni, i);
					p->v_size = ni;
					f_add(v, v + ni, -dii);
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
				auto delta = r->f_place(r->v_size, n);
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
				dii = T_traits::f_index(n, v[n - 1]);
				auto w = r->f_values() + r->v_size;
				f_move(v, v + n, w);
				r->v_size = A_leaf;
				f_add(w, w + n, T_traits::f_index(0, w[-1]));
				q->v_indices[j - 1] += dii;
				ii = l.v_index -= n;
				f_unshift(v, v + ii, n);
				f_add(v, v + ii, -dii);
			}
		}
		// if i' + m' + n - i <= N
		if (ii + a_n + ni <= A_leaf) {
			// |=     N      =|
			// |=i'=|*|=n-i=|.|
			p->f_slide(i, ii + a_n);
			auto w = std::uninitialized_copy_n(a_first, a_n, v + ii);
			auto delta = p->f_place(ii, a_n);
			f_add(w, w + ni, delta - dii);
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
		if (ii > 0) dii += T_traits::f_index(ii, v[ii - 1]);
		auto r = new t_leaf(A_leaf, p);
		auto w = f_move(v, v + ii, r->f_values());
		if (ii < A_leaf) {
			size_t n = A_leaf - ii;
			std::uninitialized_copy_n(a_first, n, w);
			auto delta = r->f_place(ii, n);
			a_first += n;
			a_n -= n;
			a_tail = this->f_insert_branch(a_head, a_tail, T_traits::f_index(A_leaf, r->f_values()[A_leaf - 1]), delta, r, false, true);
			this->v_size += delta;
		} else {
			a_tail = this->f_insert_branch(a_head, a_tail, T_traits::f_index(A_leaf, r->f_values()[A_leaf - 1]), t_delta{}, r, false, true);
		}
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
		auto delta = r->f_place(0, A_leaf);
		a_first += A_leaf;
		a_n -= A_leaf;
		a_tail = this->f_insert_branch(a_head, a_tail, T_traits::f_index(A_leaf, r->f_values()[A_leaf - 1]), delta, r, false, true);
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
				auto w = r->f_values();
				size_t d = k - A_leaf;
				f_shift(w, w + r->v_size, d);
				t_delta delta;
				if (ii + a_n <= A_leaf) {
					// |=  N   =| |=  N   =|
					// |=i'=|*|=n-i=|=n1=|.|
					size_t e = p->v_size - d;
					f_move(v + e, v + p->v_size, w);
					f_add(w, w + d, -(e > i ? T_traits::f_index(0, v[e - 1]) : di));
					f_shift(v + i, v + e, ii + a_n - i);
					auto q = std::uninitialized_copy_n(a_first, a_n, v + ii);
					delta = p->f_place(ii, a_n);
					f_add(q, v + A_leaf, delta - dii);
				} else {
					// |= N  =| |=     N      =|
					// |=i'=|*****|=n-i=|=n1=|.|
					size_t e = A_leaf - ii;
					size_t f = a_n - e;
					auto q = f_move(v + i, v + p->v_size, w + f);
					std::uninitialized_copy_n(a_first, e, v + ii);
					std::uninitialized_copy_n(a_first + e, f, w);
					auto d = r->f_place(0, f);
					delta = p->f_place(ii, e) + d;
					f_add(w + f, q, d - di);
				}
				p->v_size = A_leaf;
				r->v_size += d;
				auto e = T_traits::f_index(d, w[d - 1]);
				f_add(w + d, w + r->v_size, e);
				q->v_indices[j] -= e;
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
		size_t e = p->v_size - r->v_size;
		auto w = v + e;
		auto q = f_move(w, w + r->v_size, r->f_values());
		f_add(r->f_values(), q, -(e > i ? T_traits::f_index(0, v[e - 1]) : di));
		f_shift(v + i, w, ii + a_n - i);
		std::uninitialized_copy_n(a_first, a_n, v + ii);
		delta = p->f_place(ii, a_n);
		p->v_size = k - r->v_size;
		f_add(v + ii + a_n, v + p->v_size, delta - dii);
	} else if (a_n + ni <= r->v_size) {
		// |=N=| |=    N    =|
		// |= i'  =|*|=n-i=|.|
		auto w = r->f_values() + r->v_size - ni;
		f_move(v + i, v + p->v_size, w);
		std::uninitialized_copy_n(a_first, a_n, w - a_n);
		p->v_size = k - r->v_size;
		auto q = f_move(v + p->v_size, v + ii, r->f_values());
		auto d = T_traits::f_index(0, v[p->v_size - 1]);
		f_add(r->f_values(), q, -d);
		delta = r->f_place(ii - p->v_size, a_n);
		f_add(w, w + ni, delta - dii - d);
		l = {r, ii - p->v_size};
	} else {
		// |= N  =| |=   N   =|
		// |=i'=|*****|=n-i=|.|
		auto w = r->f_values();
		size_t e = r->v_size - ni;
		f_move(v + i, v + p->v_size, w + e);
		std::uninitialized_copy_n(a_first, a_n - e, v + ii);
		std::uninitialized_copy_n(a_first + a_n - e, e, w);
		auto d = r->f_place(0, e);
		delta = p->f_place(ii, a_n - e) + d;
		f_add(w + e, w + r->v_size, d - di);
		p->v_size = k - r->v_size;
	}
	this->f_insert_branch(a_head, a_tail, T_traits::f_index(p->v_size, p->f_values()[p->v_size - 1]), delta, r, true, false);
	this->v_size += delta;
	return l;
}

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits>
typename t_array<T_value, A_leaf, A_branch, T_traits>::t_at t_array<T_value, A_leaf, A_branch, T_traits>::f_erase_leaf(t_via* a_head, t_via* a_tail, const t_at& a_at, size_t a_n)
{
	auto p = static_cast<t_leaf*>(a_at.v_node);
	size_t i = a_at.v_index;
	if (a_head == a_tail) {
		auto delta = p->f_erase(i, a_n);
		this->v_size -= delta;
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
	auto q = a_tail[-1].v_node;
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = p->v_previous;
		size_t n = A_leaf / 2 + a_n - p->v_size;
		if (r->v_size - n >= A_leaf / 2) {
			auto size = T_traits::f_index(n, r->f_values()[r->v_size - 1]);
			auto delta = r->f_erase(p, n, i, a_n);
			q->v_indices[j - 1] -= size - T_traits::f_index(0, r->f_values()[r->v_size - 1]);
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
	size_t n = A_leaf / 2 + a_n - p->v_size;
	if (r->v_size - n >= A_leaf / 2) {
		auto size = T_traits::f_index(p->v_size, p->f_values()[p->v_size - 1]);
		auto delta = p->f_erase(i, a_n, n, r);
		q->v_indices[j] += T_traits::f_index(A_leaf / 2, p->f_values()[A_leaf / 2 - 1]) + delta - size;
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

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits>
typename t_array<T_value, A_leaf, A_branch, T_traits>::t_at t_array<T_value, A_leaf, A_branch, T_traits>::f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i)
{
	size_t n = A_leaf / 2 - a_p->v_size;
	auto v = a_p->f_values();
	auto q = a_tail[-1].v_node;
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = a_p->v_previous;
		if (r->v_size - n >= A_leaf / 2) {
			r->v_size -= n;
			a_p->v_size = A_leaf / 2;
			auto w = r->f_values() + r->v_size;
			f_move(w, w + n, f_shift(v, v + a_p->v_size, n));
			f_add(v, v + n, -T_traits::f_index(0, w[-1]));
			auto delta = T_traits::f_index(n, v[n - 1]);
			f_add(v + n, v + A_leaf / 2, delta);
			q->v_indices[j - 1] -= delta;
			a_i += n;
			return a_i < A_leaf / 2 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			a_i += r->v_size;
			auto w = r->f_values() + r->v_size;
			f_move(v, v + a_p->v_size, w);
			f_add(w, w + a_p->v_size, T_traits::f_index(0, w[-1]));
			r->v_size += a_p->v_size;
			a_p->f_unlink();
			this->f_erase_branch(a_head, a_tail, t_index{});
			return a_i < r->v_size ? t_at{r, a_i} : t_at{r->v_next, 0};
		}
	}
	auto r = a_p->v_next;
	auto w = r->f_values();
	auto vv = v + a_p->v_size;
	if (r->v_size - n >= A_leaf / 2) {
		auto delta = T_traits::f_index(n, w[n - 1]);
		f_move(w, w + n, vv);
		f_add(vv, v + A_leaf / 2, T_traits::f_index(0, vv[-1]));
		a_p->v_size = A_leaf / 2;
		r->v_size -= n;
		f_unshift(w, w + r->v_size, n);
		f_add(w, w + r->v_size, -delta);
		q->v_indices[j] += delta;
	} else {
		f_move(w, w + r->v_size, v + a_p->v_size);
		f_add(vv, vv + r->v_size, T_traits::f_index(0, vv[-1]));
		a_p->v_size += r->v_size;
		r->f_unlink();
		this->f_erase_branch(a_head, a_tail, t_index{});
	}
	return {a_p, a_i};
}

template<typename T_value, size_t A_leaf, size_t A_branch, typename T_traits>
typename t_array<T_value, A_leaf, A_branch, T_traits>::t_at t_array<T_value, A_leaf, A_branch, T_traits>::f_merge_leaf(t_via* a_head, t_via* a_tail, t_leaf* a_p, size_t a_i, t_leaf* a_q, size_t a_j)
{
	auto v = a_p->f_values() + a_i;
	auto di = a_i > 0 ? T_traits::f_index(a_i, v[-1]) : t_index{};
	size_t a = a_p->v_size - a_i;
	auto delta = T_traits::f_index(a_p->v_size, v[a - 1]) - di;
	auto w = a_q->f_values();
	auto dj = T_traits::f_index(a_j, w[a_j - 1]);
	f_destruct(v, v + a);
	f_destruct(w, w + a_j);
	size_t b = a_q->v_size - a_j;
	auto q = a_tail->v_node;
	size_t j = a_tail->v_index - 1;
	q->v_indices[j] -= delta;
	delta += dj;
	this->v_size -= delta;
	size_t m = a_i + b;
	if (m >= A_leaf / 2 * 2) {
		this->f_adjust(a_head, a_tail + 1, -delta);
		if (a_i < A_leaf / 2) {
			a_p->v_size = A_leaf / 2;
			a_q->v_size = m - A_leaf / 2;
			size_t d = A_leaf / 2 - a_i;
			size_t jd = a_j + d;
			auto djd = T_traits::f_index(jd, w[jd - 1]);
			f_move(w + a_j, w + jd, v);
			f_add(v, v + d, di - dj);
			f_unshift(w, w + a_q->v_size, jd);
			f_add(w, w + a_q->v_size, -djd);
			q->v_indices[j] += djd - dj;
			return {a_p, a_i};
		} else if (b < A_leaf / 2) {
			a_p->v_size = m - A_leaf / 2;
			a_q->v_size = A_leaf / 2;
			size_t d = A_leaf / 2 - b;
			auto vv = a_p->f_values() + a_p->v_size;
			f_unshift(f_move(vv, v, w), w + a_q->v_size, a_j - d);
			f_add(w, w + d, -T_traits::f_index(0, vv[-1]));
			auto dd = T_traits::f_index(d, w[d - 1]);
			f_add(w + d, w + a_q->v_size, dd - dj);
			q->v_indices[j] -= dd;
			return b > 0 ? t_at{a_q, d} : t_at{a_q->v_next, 0};
		}
		a_p->v_size = a_i;
		a_q->v_size = b;
		f_unshift(w, w + a_q->v_size, a_j);
		f_add(w, w + a_q->v_size, -dj);
		return {a_q, 0};
	}
	a_p->v_size = m;
	f_move(w + a_j, w + a_j + b, v);
	f_add(v, v + b, di - dj);
	if (m > 0) q->v_indices[j] += T_traits::f_index(m, v[b - 1]) - di;
	a_tail->v_index = j;
	a_q->f_unlink();
	a_tail = this->f_erase_branch(a_head, a_tail + 1, delta);
	if (a_tail == a_head || m >= A_leaf / 2) return b > 0 ? t_at{a_p, a_i} : t_at{a_p->v_next, 0};
	return f_merge_leaf(a_head, a_tail, a_p, a_i);
}

}

#endif
