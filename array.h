#ifndef XTREE__ARRAY_H
#define XTREE__ARRAY_H

#include <algorithm>
#include <memory>
#include <vector>

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

		t_leaf(t_leaf* a_p, const T_value& a_value) : v_size(1)
		{
			f_link(a_p);
			f_construct(f_values(), a_value);
		}
		t_leaf(size_t a_i, const T_value& a_value, t_leaf* a_p) : v_size((A_size + 1) / 2)
		{
			f_link(a_p);
			auto p = a_p->f_values() + A_size / 2;
			auto q = a_p->f_values() + A_size;
			std::copy(p, q, std::raw_storage_iterator<T_value*, T_value>(f_values()));
			a_p->v_size = A_size / 2 + 1;
			f_destruct(p, q);
			f_construct(f_shift(a_p->f_values() + a_i, p), a_value);
		}
		t_leaf(t_leaf* a_p, size_t a_i, const T_value& a_value) : v_size((A_size + 1) / 2)
		{
			f_link(a_p);
			auto p = a_p->f_values() + A_size / 2 + 1;
			auto q = a_p->f_values() + a_i;
			auto r = a_p->f_values() + A_size;
			auto i = std::copy(p, q, std::raw_storage_iterator<T_value*, T_value>(f_values()));
			*i = a_value;
			std::copy(q, r, ++i);
			a_p->v_size = A_size / 2 + 1;
			f_destruct(p, r);
		}
		void f_link(t_leaf* a_p)
		{
			this->v_previous = a_p;
			this->v_next = a_p->v_next;
			this->v_previous->v_next = this->v_next->v_previous = this;
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
		void f_insert(size_t a_i, const T_value& a_value)
		{
			auto p = f_values();
			f_construct(f_shift(p + a_i, p + this->v_size), a_value);
			++this->v_size;
		}
		void f_erase(size_t a_i)
		{
			auto p = f_values();
			f_unshift(f_destruct(p + a_i), p + --this->v_size);
		}
		void f_rotate(t_leaf* a_p, size_t a_i)
		{
			auto p = a_p->f_values();
			f_move(f_shift(p, f_destruct(p + a_i)), f_values() + --this->v_size);
		}
		void f_rotate(size_t a_i, t_leaf* a_p)
		{
			auto p = f_values();
			auto q = a_p->f_values();
			f_move(f_unshift(f_destruct(p + a_i), p + A_size / 2 - 1), q);
			f_unshift(q, q + --a_p->v_size);
		}
		void f_merge(t_leaf* a_p, size_t a_i)
		{
			this->v_size = A_size / 2 * 2 - 1;
			auto p = a_p->f_values();
			auto i = std::copy(p, p + a_i, std::raw_storage_iterator<T_value*, T_value>(f_values() + A_size / 2));
			std::copy(p + a_i + 1, p + A_size / 2, i);
		}
		void f_merge(size_t a_i, t_leaf* a_p)
		{
			this->v_size = A_size / 2 * 2 - 1;
			auto p = f_values();
			auto q = a_p->f_values();
			f_unshift(f_destruct(p + a_i), p + A_size / 2 - 1);
			std::copy(q, q + A_size / 2, std::raw_storage_iterator<T_value*, T_value>(p + A_size / 2 - 1));
		}
	};
	struct t_branch
	{
		static size_t* f_copy_forward(const size_t* a_p, const size_t* a_q, size_t* a_r, int a_shift)
		{
			while (a_p != a_q) *a_r++ = *a_p++ + a_shift;
			return a_r;
		}
		static size_t* f_copy_backward(const size_t* a_p, const size_t* a_q, size_t* a_r, int a_shift)
		{
			while (a_q != a_p) *--a_r = *--a_q + a_shift;
			return a_r;
		}

		size_t v_size;
		size_t v_indices[A_size];
		void* v_nodes[A_size + 1];

		t_branch(void* a_root, size_t a_index, void* a_node) : v_size(1)
		{
			v_indices[0] = a_index;
			v_nodes[0] = a_root;
			v_nodes[1] = a_node;
		}
		t_branch(size_t a_i, size_t a_index, void* a_node, t_branch* a_p) : v_size(A_size / 2)
		{
			{
				size_t* p = a_p->v_indices + (A_size + 1) / 2;
				size_t* q = p - 1;
				f_copy_forward(p, a_p->v_indices + A_size, v_indices, -*q);
				a_p->v_size = (A_size + 1) / 2;
				size_t* r = a_p->v_indices + a_i;
				f_copy_backward(r, q, p, 1);
				*r = a_index;
			}
			auto p = a_p->v_nodes + (A_size + 1) / 2;
			std::copy(p, a_p->v_nodes + A_size + 1, v_nodes);
			*f_shift(a_p->v_nodes + a_i + 1, p) = a_node;
		}
		t_branch(t_branch* a_p, size_t a_i, size_t a_index, void* a_node) : v_size(A_size / 2)
		{
			{
				size_t* p = a_p->v_indices + (A_size + 1) / 2;
				size_t shift = -*p;
				size_t* q = a_p->v_indices + a_i;
				size_t* r = f_copy_forward(++p, q, v_indices, shift);
				*r = a_index + shift;
				f_copy_forward(q, a_p->v_indices + A_size, ++r, ++shift);
				a_p->v_size = (A_size + 1) / 2;
			}
			auto p = a_p->v_nodes + (A_size + 1) / 2 + 1;
			auto q = a_p->v_nodes + a_i + 1;
			auto i = std::copy(p, q, v_nodes);
			*i = a_node;
			std::copy(q, a_p->v_nodes + A_size + 1, ++i);
		}
		t_branch(t_branch* a_p, size_t a_index, void* a_node) : v_size(A_size / 2)
		{
			f_copy_forward(a_p->v_indices + (A_size + 1) / 2, a_p->v_indices + A_size, v_indices, -a_index + 1);
			a_p->v_size = (A_size + 1) / 2;
			auto p = a_p->v_nodes + (A_size + 1) / 2 + 1;
			auto q = a_p->v_nodes + A_size + 1;
			v_nodes[0] = a_node;
			std::copy(p, q, v_nodes + 1);
		}
		void f_insert(size_t a_i, size_t a_index, void* a_node)
		{
			auto p = v_nodes + 1;
			*f_shift(p + a_i, p + this->v_size) = a_node;
			{
				size_t* p = v_indices + a_i;
				size_t* q = v_indices + this->v_size;
				f_copy_backward(p, q, q + 1, 1);
				*p = a_index;
			}
			++this->v_size;
		}
		void f_erase(size_t a_i)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + this->v_size);
			size_t* p = v_indices + a_i;
			f_copy_forward(p + 1, v_indices + this->v_size, p, -1);
			--this->v_size;
		}
		void f_rotate(size_t a_base, size_t& a_index, t_branch* a_p, size_t a_i)
		{
			auto p = a_p->v_nodes;
			*f_shift(p, p + a_i + 1) = v_nodes[this->v_size];
			size_t index = a_base + v_indices[--this->v_size];
			size_t shift = a_index - index;
			a_index = index;
			{
				size_t* p = a_p->v_indices + a_i;
				f_copy_backward(a_p->v_indices, p, p + 1, shift);
				a_p->v_indices[0] = shift;
				--shift;
				size_t* q = a_p->v_indices + a_p->v_size;
				while (++p != q) *p += shift;
			}
		}
		void f_rotate(size_t a_i, size_t a_base, size_t& a_index, t_branch* a_p)
		{
			auto p = v_nodes;
			auto q = a_p->v_nodes;
			*f_unshift(p + a_i + 1, p + A_size / 2) = *q;
			f_unshift(q, q + a_p->v_size);
			size_t index = a_index - a_base;
			size_t shift = a_p->v_indices[0];
			a_index += shift;
			{
				size_t* p = v_indices + a_i;
				size_t* q = f_copy_forward(p + 1, v_indices + A_size / 2, p, -1);
				*q = index - 1;
				f_copy_forward(a_p->v_indices + 1, a_p->v_indices + a_p->v_size, a_p->v_indices, -shift);
			}
			--a_p->v_size;
		}
		void f_merge(size_t a_base, size_t a_index, t_branch* a_p, size_t a_i)
		{
			auto p = a_p->v_nodes;
			auto i = std::copy(p, p + a_i + 1, v_nodes + A_size / 2 + 1);
			std::copy(p + a_i + 2, p + A_size / 2 + 1, i);
			this->v_size = A_size / 2 * 2;
			size_t shift = a_index - a_base;
			{
				size_t* p = v_indices + A_size / 2;
				*p = shift;
				size_t* q = a_p->v_indices + a_i;
				p = f_copy_forward(a_p->v_indices, q, ++p, shift);
				f_copy_forward(++q, a_p->v_indices + A_size / 2, p, --shift);
			}
		}
		void f_merge(size_t a_i, size_t a_base, size_t a_index, t_branch* a_p)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + A_size / 2);
			auto q = a_p->v_nodes;
			std::copy(q, q + A_size / 2 + 1, v_nodes + A_size / 2);
			this->v_size = A_size / 2 * 2;
			size_t shift = a_index - a_base;
			{
				size_t* p = v_indices + a_i;
				size_t* q = v_indices + A_size / 2;
				p = f_copy_forward(p + 1, q, p, -1);
				*p = --shift;
				f_copy_forward(a_p->v_indices, a_p->v_indices + A_size / 2, ++p, shift);
			}
		}
	};

	static void f_adjust(const std::vector<t_location>& a_path, int a_delta)
	{
		auto i = a_path.end();
		while (i != a_path.begin()) {
			auto p = static_cast<t_branch*>((--i)->v_node);
			for (size_t j = i->v_index; j < p->v_size; ++j) p->v_indices[j] += a_delta;
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
	void f_insert_leaf(std::vector<t_location>& a_path, const T_value& a_value);
	void f_insert_branch(std::vector<t_location>& a_path, size_t a_index, void* a_node, bool a_right);
	void f_erase_leaf(std::vector<t_location>& a_path);
	void f_erase_branch(std::vector<t_location>& a_path);
	void f_at(size_t a_index, std::vector<t_location>& a_path) const
	{
		auto p = v_root;
		for (size_t n = v_depth; --n > 0;) {
			auto q = static_cast<t_branch*>(p);
			auto& indices = q->v_indices;
			size_t i = std::upper_bound(indices, indices + q->v_size, a_index) - indices;
			a_path.push_back({q, i});
			if (i > 0) a_index -= indices[i - 1];
			p = q->v_nodes[i];
		}
		a_path.push_back({p, a_index});
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
		return {{&v_link, 0}, 0};
	}
	t_constant_iterator f_end() const
	{
		return {{const_cast<t_link*>(&v_link), 0}, 0};
	}
	t_mutable_iterator f_insert(t_mutable_iterator a_i, const T_value& a_value)
	{
		if (v_size++ <= 0) {
			v_root = new t_leaf(static_cast<t_leaf*>(&v_link), a_value);
			++v_depth;
			return f_begin();
		}
		std::vector<t_location> path;
		f_at(a_i.v_index, path);
		f_insert_leaf(path, a_value);
		return {path.back(), a_i.v_index};
	}
	t_mutable_iterator f_erase(t_mutable_iterator a_i)
	{
		std::vector<t_location> path;
		f_at(a_i.v_index, path);
		f_erase_leaf(path);
		if (--v_size <= 0) return f_end();
		return {path.back(), a_i.v_index};
	}
	t_mutable_iterator f_at(size_t a_index)
	{
		if (v_size <= 0) return f_end();
		std::vector<t_location> path;
		f_at(a_index, path);
		return {path.back(), a_index};
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
void t_array<T_value, A_size>::f_insert_leaf(std::vector<t_location>& a_path, const T_value& a_value)
{
	auto p = static_cast<t_leaf*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	a_path.pop_back();
	if (p->v_size < A_size) {
		p->f_insert(i, a_value);
		if (!a_path.empty()) f_adjust(a_path, 1);
		a_path.push_back({p, i});
	} else if (i < (A_size + 1) / 2) {
		auto q = new t_leaf(i, a_value, p);
		f_insert_branch(a_path, (A_size + 1) / 2, q, false);
		a_path.push_back({p, i});
	} else {
		auto q = new t_leaf(p, i, a_value);
		f_insert_branch(a_path, (A_size + 1) / 2, q, true);
		a_path.push_back({q, i - (A_size + 1) / 2});
	}
}

template<typename T_value, size_t A_size>
void t_array<T_value, A_size>::f_insert_branch(std::vector<t_location>& a_path, size_t a_index, void* a_node, bool a_right)
{
	if (a_path.empty()) {
		v_root = new t_branch(v_root, a_index, a_node);
		++v_depth;
		a_path.push_back({v_root, a_right ? 1u : 0u});
		return;
	}
	auto p = static_cast<t_branch*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	if (i > 0) a_index += p->v_indices[i - 1];
	a_path.pop_back();
	if (p->v_size < A_size) {
		p->f_insert(i, a_index, a_node);
		if (!a_path.empty()) f_adjust(a_path, 1);
		if (a_right) ++i;
		a_path.push_back({p, i});
	} else if (i < (A_size + 1) / 2) {
		size_t index = p->v_indices[(A_size + 1) / 2 - 1];
		//      p
		//  i i I i i
		// x X x x x x
		auto q = new t_branch(i, a_index, a_node, p);
		//    p      q
		//  i i i   i i
		// x X X x x x x
		f_insert_branch(a_path, index + 1, q, false);
		if (a_right) ++i;
		a_path.push_back({p, i});
	} else if (i > (A_size + 1) / 2) {
		size_t index = p->v_indices[(A_size + 1) / 2];
		//      p
		//  i i i I i
		// x x x x X x
		auto q = new t_branch(p, i, a_index, a_node);
		//    p      q
		//  i i i   i i
		// x x x x X X x
		f_insert_branch(a_path, index, q, true);
		if (a_right) ++i;
		a_path.push_back({q, i - (A_size + 1) / 2 - 1});
	} else {
		//      p
		//  i i i I i
		// x x x X x x
		auto q = new t_branch(p, a_index, a_node);
		//    p      q
		//  i i i   i i
		// x x x X X x x
		f_insert_branch(a_path, a_index, q, a_right);
		a_path.push_back(a_right ? t_location{q, 0} : t_location{p, (A_size + 1) / 2});
	}
}

template<typename T_value, size_t A_size>
void t_array<T_value, A_size>::f_erase_leaf(std::vector<t_location>& a_path)
{
	auto p = static_cast<t_leaf*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	a_path.pop_back();
	if (a_path.empty()) {
		if (p->v_size > 1) {
			p->f_erase(i);
			a_path.push_back({p, i});
		} else {
			v_root = nullptr;
			p->f_unlink();
			--v_depth;
		}
		return;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_path, -1);
		a_path.push_back({p, i});
		return;
	}
	auto q = static_cast<t_branch*>(a_path.back().v_node);
	size_t j = a_path.back().v_index;
	if (j > 0) {
		auto r = static_cast<t_leaf*>(q->v_nodes[j - 1]);
		if (r->v_size > A_size / 2) {
			r->f_rotate(p, i);
			--q->v_indices[j - 1];
			f_adjust(a_path, -1);
			a_path.push_back({p, i + 1});
			return;
		} else if (j >= q->v_size) {
			a_path.back().v_index = --j;
			r->f_merge(p, i);
			p->f_unlink();
			f_erase_branch(a_path);
			a_path.push_back({r, i + A_size / 2});
			return;
		}
	}
	auto r = static_cast<t_leaf*>(q->v_nodes[j + 1]);
	if (r->v_size > A_size / 2) {
		p->f_rotate(i, r);
		++q->v_indices[j];
		f_adjust(a_path, -1);
	} else {
		p->f_merge(i, r);
		r->f_unlink();
		f_erase_branch(a_path);
	}
	a_path.push_back({p, i});
}

template<typename T_value, size_t A_size>
void t_array<T_value, A_size>::f_erase_branch(std::vector<t_location>& a_path)
{
	auto p = static_cast<t_branch*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	a_path.pop_back();
	if (a_path.empty()) {
		if (p->v_size > 1) {
			p->f_erase(i);
			a_path.push_back({p, i});
		} else {
			v_root = p->v_nodes[0];
			delete p;
			--v_depth;
		}
		return;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_path, -1);
		a_path.push_back({p, i});
		return;
	}
	auto q = static_cast<t_branch*>(a_path.back().v_node);
	size_t j = a_path.back().v_index;
	if (j > 0) {
		auto r = static_cast<t_branch*>(q->v_nodes[j - 1]);
		if (r->v_size > A_size / 2) {
			size_t base = j > 1 ? q->v_indices[j - 2] : 0;
			r->f_rotate(base, q->v_indices[j - 1], p, i);
			f_adjust(a_path, -1);
			a_path.push_back({p, i + 1});
			return;
		} else if (j >= q->v_size) {
			a_path.back().v_index = --j;
			r->f_merge(j > 0 ? q->v_indices[j - 1] : 0, q->v_indices[j], p, i);
			delete p;
			f_erase_branch(a_path);
			a_path.push_back({r, i + A_size / 2 + 1});
			return;
		}
	}
	auto r = static_cast<t_branch*>(q->v_nodes[j + 1]);
	size_t base = j > 0 ? q->v_indices[j - 1] : 0;
	if (r->v_size > A_size / 2) {
		p->f_rotate(i, base, q->v_indices[j], r);
		f_adjust(a_path, -1);
	} else {
		p->f_merge(i, base, q->v_indices[j], r);
		delete r;
		f_erase_branch(a_path);
	}
	a_path.push_back({p, i});
}

}

#endif
