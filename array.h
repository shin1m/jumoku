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
	struct t_leaf : t_node
	{
		char v_data[sizeof(T_value) * A_size];

		t_leaf() : t_node(0)
		{
		}
		t_leaf(const T_value& a_value) : t_node(1)
		{
			f_construct(f_values(), a_value);
		}
		t_leaf(size_t a_i, const T_value& a_value, t_leaf* a_p) : t_node((A_size + 1) / 2)
		{
			auto p = a_p->f_values() + A_size / 2;
			auto q = a_p->f_values() + A_size;
			std::copy(p, q, std::raw_storage_iterator<T_value*, T_value>(f_values()));
			a_p->v_size = A_size / 2 + 1;
			f_destruct(p, q);
			f_construct(f_shift(a_p->f_values() + a_i, p), a_value);
		}
		t_leaf(t_leaf* a_p, size_t a_i, const T_value& a_value) : t_node((A_size + 1) / 2)
		{
			auto p = a_p->f_values() + A_size / 2 + 1;
			auto q = a_p->f_values() + a_i;
			auto r = a_p->f_values() + A_size;
			auto i = std::copy(p, q, std::raw_storage_iterator<T_value*, T_value>(f_values()));
			*i = a_value;
			std::copy(q, r, ++i);
			a_p->v_size = A_size / 2 + 1;
			f_destruct(p, r);
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
	struct t_branch : t_node
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

		size_t v_indices[A_size];
		t_node* v_nodes[A_size + 1];

		t_branch(t_node* a_root, size_t a_index, t_node* a_node) : t_node(1)
		{
			v_indices[0] = a_index;
			v_nodes[0] = a_root;
			v_nodes[1] = a_node;
		}
		t_branch(size_t a_i, size_t a_index, t_node* a_node, t_branch* a_p) : t_node(A_size / 2)
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
		t_branch(t_branch* a_p, size_t a_i, size_t a_index, t_node* a_node) : t_node(A_size / 2)
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
		t_branch(t_branch* a_p, size_t a_index, t_node* a_node) : t_node(A_size / 2)
		{
			f_copy_forward(a_p->v_indices + (A_size + 1) / 2, a_p->v_indices + A_size, v_indices, -a_index + 1);
			a_p->v_size = (A_size + 1) / 2;
			auto p = a_p->v_nodes + (A_size + 1) / 2 + 1;
			auto q = a_p->v_nodes + A_size + 1;
			v_nodes[0] = a_node;
			std::copy(p, q, v_nodes + 1);
		}
		void f_insert(size_t a_i, size_t a_index, t_node* a_node)
		{
			auto p = v_nodes + 1;
			f_construct(f_shift(p + a_i, p + this->v_size), a_node);
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
			f_move(f_shift(p, p + a_i + 1), v_nodes + this->v_size);
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
			f_move(f_unshift(p + a_i + 1, p + A_size / 2), q);
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

	void f_last(std::vector<t_location>& a_path) const
	{
		while (a_path.size() < v_depth) {
			auto p = a_path.back();
			auto q = static_cast<t_branch*>(p.v_node)->v_nodes[p.v_index];
			a_path.push_back(t_location(q, q->v_size));
		}
	}
	void f_normalize(std::vector<t_location>& a_path) const
	{
		if (a_path.back().f_more()) return;
		while (!a_path.back().f_more() && a_path.size() > 1) a_path.pop_back();
		if (!a_path.back().f_more()) return;
		++a_path.back().v_index;
		while (a_path.size() < v_depth) {
			auto p = a_path.back();
			auto q = static_cast<t_branch*>(p.v_node)->v_nodes[p.v_index];
			a_path.push_back(t_location(q, 0));
		}
	}
	void f_forward(std::vector<t_location>& a_path) const
	{
		++a_path.back().v_index;
		f_normalize(a_path);
	}
	void f_backward(std::vector<t_location>& a_path) const
	{
		if (a_path.back().v_index <= 0) {
			do a_path.pop_back(); while (a_path.back().v_index <= 0);
			--a_path.back().v_index;
		}
		f_last(a_path);
		--a_path.back().v_index;
	}
	static T_value& f_value(std::vector<t_location>& a_path)
	{
		return static_cast<t_leaf*>(static_cast<t_node*>(a_path.back().v_node))->f_values()[a_path.back().v_index];
	}
	static void f_adjust(const std::vector<t_location>& a_path, int a_delta)
	{
		auto i = a_path.end();
		while (i != a_path.begin()) {
			if (!(--i)->f_more()) continue;
			auto p = static_cast<t_branch*>(i->v_node);
			size_t j = i->v_index;
			do p->v_indices[j] += a_delta; while (++j < p->v_size);
		}
	}

	void f_clear(size_t a_level, t_node* a_node)
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
	void f_insert_branch(std::vector<t_location>& a_path, size_t a_index, t_node* a_node, bool a_right);
	void f_erase_leaf(std::vector<t_location>& a_path);
	void f_erase_branch(std::vector<t_location>& a_path);
	t_path* f_at_path(size_t a_index) const
	{
		if (a_index >= v_size) return f_end_path();
		auto p = new t_path();
		auto q = v_root;
		for (auto n = v_depth; --n > 0;) {
			auto r = static_cast<t_branch*>(q);
			auto& indices = r->v_indices;
			size_t i = std::upper_bound(indices, indices + r->v_size, a_index) - indices;
			p->v_path.push_back(t_location(r, i));
			if (i > 0) a_index -= indices[i - 1];
			q = r->v_nodes[i];
		}
		p->v_path.push_back(t_location(q, a_index));
		return p;
	}
	void f_dump(size_t a_level, t_node* a_node, const std::function<void(size_t, size_t)>& a_dump) const
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
	struct t_iterator : std::iterator<std::bidirectional_iterator_tag, T_value>, t_trivial
	{
		friend class t_array<T_value, A_size>;
		friend struct t_constant_iterator;

		typedef T_value* pointer;
		typedef T_value& reference;

		t_iterator() = default;
		t_iterator(const t_iterator& a_x) : t_trivial(a_x)
		{
		}
		t_iterator(t_array* a_tree, t_path* a_p) : t_trivial(a_tree, a_p)
		{
		}
		t_iterator& operator=(const t_iterator& a_x)
		{
			f_assign(a_x);
			return *this;
		}
		T_value& operator*() const
		{
			return f_value(v_p->v_path);
		}
		T_value* operator->() const
		{
			return &**this;
		}
		t_iterator& operator++()
		{
			f_own();
			if (!v_p->v_path.empty()) static_cast<const t_array*>(v_tree)->f_forward(v_p->v_path);
			return *this;
		}
		t_iterator operator++(int) const
		{
			t_iterator i(v_p->v_path);
			++*this;
			return i;
		}
		t_iterator& operator--()
		{
			f_own();
			if (!v_p->v_path.empty()) static_cast<const t_array*>(v_tree)->f_backward(v_p->v_path);
			return *this;
		}
		t_iterator operator--(int) const
		{
			t_iterator i(v_p->v_path);
			--*this;
			return i;
		}
	};
	struct t_constant_iterator : std::iterator<std::bidirectional_iterator_tag, T_value>, t_trivial
	{
		friend class t_array<T_value, A_size>;

		typedef const T_value* pointer;
		typedef const T_value& reference;

		t_constant_iterator() = default;
		t_constant_iterator(const t_constant_iterator& a_x) : t_trivial(a_x)
		{
		}
		t_constant_iterator(const t_iterator& a_x) : t_trivial(a_x)
		{
		}
		t_constant_iterator(const t_array* a_tree, t_path* a_p) : t_trivial(a_tree, a_p)
		{
		}
		t_constant_iterator& operator=(const t_constant_iterator& a_x)
		{
			f_assign(a_x);
			return *this;
		}
		t_constant_iterator& operator=(const t_iterator& a_x)
		{
			f_assign(a_x);
			return *this;
		}
		const T_value& operator*() const
		{
			return f_value(v_p->v_path);
		}
		const T_value* operator->() const
		{
			return &**this;
		}
		t_constant_iterator& operator++()
		{
			f_own();
			if (!v_p->v_path.empty()) static_cast<const t_array*>(v_tree)->f_forward(v_p->v_path);
			return *this;
		}
		t_constant_iterator operator++(int) const
		{
			t_constant_iterator i(v_p->v_path);
			++*this;
			return i;
		}
		t_constant_iterator& operator--()
		{
			f_own();
			if (!v_p->v_path.empty()) static_cast<const t_array*>(v_tree)->f_backward(v_p->v_path);
			return *this;
		}
		t_constant_iterator operator--(int) const
		{
			t_constant_iterator i(v_p->v_path);
			--*this;
			return i;
		}
	};

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
		f_begin_path()->v_path.clear();
		f_end_path()->v_path.clear();
	}
	t_iterator f_begin()
	{
		return t_iterator(this, f_begin_path());
	}
	t_constant_iterator f_begin() const
	{
		return t_constant_iterator(this, f_begin_path());
	}
	t_iterator f_end()
	{
		return t_iterator(this, f_end_path());
	}
	t_constant_iterator f_end() const
	{
		return t_constant_iterator(this, f_end_path());
	}
	t_iterator f_insert(t_iterator a_i, const T_value& a_value)
	{
		a_i.f_own();
		auto& path = a_i.v_p->v_path;
		if (!path.empty()) f_last(path);
		f_insert_leaf(path, a_value);
		++v_size;
		return a_i;
	}
	t_iterator f_erase(t_iterator a_i)
	{
		a_i.f_own();
		auto& path = a_i.v_p->v_path;
		f_erase_leaf(path);
		--v_size;
		if (!path.empty()) f_normalize(path);
		return a_i;
	}
	t_iterator f_at(size_t a_index)
	{
		return t_iterator(this, f_at_path(a_index));
	}
	t_constant_iterator f_at(size_t a_index) const
	{
		return t_constant_iterator(this, f_at_path(a_index));
	}
	void f_dump(const std::function<void(size_t, size_t)>& a_dump) const
	{
		f_dump(0, v_root, a_dump);
	}
};

template<typename T_value, size_t A_size>
void t_array<T_value, A_size>::f_insert_leaf(std::vector<t_location>& a_path, const T_value& a_value)
{
	if (a_path.empty()) {
		v_root = new t_leaf(a_value);
		f_push_root();
		f_end_path()->v_path.push_back(t_location(v_root, 1));
		a_path.push_back(t_location(v_root, 0));
		return;
	}
	auto p = static_cast<t_leaf*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	a_path.pop_back();
	if (p->v_size < A_size) {
		p->f_insert(i, a_value);
		if (a_path.empty())
			f_end_path()->v_path[0].v_index = p->v_size;
		else
			f_adjust(a_path, 1);
		a_path.push_back(t_location(p, i));
	} else if (i < (A_size + 1) / 2) {
		auto q = new t_leaf(i, a_value, p);
		f_insert_branch(a_path, (A_size + 1) / 2, q, false);
		a_path.push_back(t_location(p, i));
	} else {
		auto q = new t_leaf(p, i, a_value);
		f_insert_branch(a_path, (A_size + 1) / 2, q, true);
		a_path.push_back(t_location(q, i - (A_size + 1) / 2));
	}
}

template<typename T_value, size_t A_size>
void t_array<T_value, A_size>::f_insert_branch(std::vector<t_location>& a_path, size_t a_index, t_node* a_node, bool a_right)
{
	if (a_path.empty()) {
		v_root = new t_branch(static_cast<t_node*>(v_root), a_index, a_node);
		f_push_root();
		f_end_path()->v_path[0] = t_location(v_root, v_root->v_size);
		a_path.push_back(t_location(v_root, a_right ? 1 : 0));
		return;
	}
	auto p = static_cast<t_branch*>(a_path.back().v_node);
	size_t i = a_path.back().v_index;
	if (i > 0) a_index += p->v_indices[i - 1];
	a_path.pop_back();
	if (p->v_size < A_size) {
		p->f_insert(i, a_index, a_node);
		if (a_path.empty())
			f_end_path()->v_path[0].v_index = p->v_size;
		else
			f_adjust(a_path, 1);
		if (a_right) ++i;
		a_path.push_back(t_location(p, i));
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
		a_path.push_back(t_location(p, i));
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
		a_path.push_back(t_location(q, i - (A_size + 1) / 2 - 1));
	} else {
		//      p
		//  i i i I i
		// x x x X x x
		auto q = new t_branch(p, a_index, a_node);
		//    p      q
		//  i i i   i i
		// x x x X X x x
		f_insert_branch(a_path, a_index, q, a_right);
		a_path.push_back(a_right ? t_location(q, 0) : t_location(p, (A_size + 1) / 2));
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
			f_end_path()->v_path[0].v_index = v_root->v_size;
			a_path.push_back(t_location(p, i));
		} else {
			v_root = nullptr;
			delete p;
			f_pop_root();
			f_end_path()->v_path.clear();
		}
		return;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_path, -1);
		a_path.push_back(t_location(p, i));
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
			a_path.push_back(t_location(p, i + 1));
			return;
		} else if (j >= q->v_size) {
			a_path.back().v_index = --j;
			r->f_merge(p, i);
			delete p;
			f_erase_branch(a_path);
			a_path.push_back(t_location(r, i + A_size / 2));
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
		delete r;
		f_erase_branch(a_path);
	}
	a_path.push_back(t_location(p, i));
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
			f_end_path()->v_path[0].v_index = v_root->v_size;
			a_path.push_back(t_location(p, i));
		} else {
			v_root = p->v_nodes[0];
			delete p;
			f_pop_root();
			f_end_path()->v_path[0] = t_location(v_root, v_root->v_size);
		}
		return;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i);
		f_adjust(a_path, -1);
		a_path.push_back(t_location(p, i));
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
			a_path.push_back(t_location(p, i + 1));
			return;
		} else if (j >= q->v_size) {
			a_path.back().v_index = --j;
			r->f_merge(j > 0 ? q->v_indices[j - 1] : 0, q->v_indices[j], p, i);
			delete p;
			f_erase_branch(a_path);
			a_path.push_back(t_location(r, i + A_size / 2 + 1));
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
	a_path.push_back(t_location(p, i));
}

}

#endif
