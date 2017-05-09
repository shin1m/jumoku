#ifndef XTREE__TREE_H
#define XTREE__TREE_H

#include <algorithm>
#include <functional>

namespace xtree
{

template<typename T, typename U>
inline T* f_construct(T* a_p, U&& a_x)
{
	return new (static_cast<void*>(a_p)) T(std::forward<U>(a_x));
}

template<typename T>
inline T* f_destruct(T* a_p)
{
	a_p->~T();
	return a_p;
}

template<typename T>
inline void f_destruct(T* a_i, T* a_j)
{
	while (a_i != a_j) {
		f_destruct(a_i);
		++a_i;
	}
}

template<typename T>
inline void f_move(T* a_p, T* a_q)
{
	f_construct(a_p, std::move(*a_q));
	f_destruct(a_q);
}

template<typename T>
inline T* f_move(T* a_i, T* a_j, T* a_p)
{
	while (a_i != a_j) f_move(a_p++, a_i++);
	return a_p;
}

template<typename T>
inline T* f_shift(T* a_i, T* a_j, size_t a_n)
{
	while (a_j != a_i) {
		--a_j;
		f_move(a_j + a_n, a_j);
	}
	return a_j;
}

template<typename T>
inline T* f_unshift(T* a_i, T* a_j, size_t a_n)
{
	while (a_i != a_j) {
		f_move(a_i, a_i + a_n);
		++a_i;
	}
	return a_i;
}

template<typename T_traits, size_t A_size>
class t_tree
{
	using t_index = typename T_traits::t_index;
	using t_delta = typename T_traits::t_delta;

protected:
	struct t_branch
	{
		static t_index* f_copy_forward(const t_index* a_p, const t_index* a_q, t_index* a_r, const t_delta& a_delta)
		{
			while (a_p != a_q) *a_r++ = *a_p++ + a_delta;
			return a_r;
		}
		static t_index* f_copy_backward(const t_index* a_p, const t_index* a_q, t_index* a_r, const t_delta& a_delta)
		{
			while (a_q != a_p) *--a_r = *--a_q + a_delta;
			return a_r;
		}

		size_t v_size;
		t_index v_indices[A_size];
		void* v_nodes[A_size + 1];

		t_branch(void* a_left, const t_index& a_index, void* a_right) : v_size(1)
		{
			v_indices[0] = a_index;
			v_nodes[0] = a_left;
			v_nodes[1] = a_right;
		}
		t_branch(size_t a_i, const t_index& a_index, const t_index& a_delta, void* a_node, bool a_right, t_branch* a_p) : v_size(A_size / 2)
		{
			{
				auto p = a_p->v_indices + (A_size + 1) / 2;
				auto q = p - 1;
				f_copy_forward(p, a_p->v_indices + A_size, v_indices, -*q);
				a_p->v_size = (A_size + 1) / 2;
				auto r = a_p->v_indices + a_i;
				f_copy_backward(r, q, p, a_delta);
				*r = a_index;
			}
			auto p = a_p->v_nodes + (A_size + 1) / 2;
			std::copy(p, a_p->v_nodes + A_size + 1, v_nodes);
			*f_shift(a_p->v_nodes + a_i + (a_right ? 1 : 0), p, 1) = a_node;
		}
		t_branch(t_branch* a_p, size_t a_i, const t_index& a_index, const t_index& a_delta, void* a_node, bool a_right) : v_size(A_size / 2)
		{
			{
				auto p = a_p->v_indices + (A_size + 1) / 2;
				auto delta = -*p;
				auto q = a_p->v_indices + a_i;
				auto r = f_copy_forward(++p, q, v_indices, delta);
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
		t_branch(t_branch* a_p, const t_index& a_index, const t_index& a_delta, void* a_node, bool a_right) : v_size(A_size / 2)
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
		void f_insert(size_t a_i, const t_index& a_index, const t_index& a_delta, void* a_node, bool a_right)
		{
			auto p = v_nodes + this->v_size + 1;
			*f_shift(v_nodes + a_i + (a_right ? 1 : 0), p, 1) = a_node;
			{
				auto p = v_indices + a_i;
				auto q = v_indices + this->v_size;
				f_copy_backward(p, q, q + 1, a_delta);
				*p = a_index;
			}
			++this->v_size;
		}
		void f_erase(size_t a_i, const t_index& a_delta)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + this->v_size, 1);
			auto p = v_indices + a_i;
			f_copy_forward(p + 1, v_indices + this->v_size, p, -a_delta);
			--this->v_size;
		}
		void f_erase(const t_index& a_base, t_index& a_index, t_branch* a_p, size_t a_i, const t_index& a_delta)
		{
			auto p = a_p->v_nodes;
			*f_shift(p, p + a_i + 1, 1) = v_nodes[this->v_size];
			auto index = a_base + v_indices[--this->v_size];
			auto delta = a_index - index;
			a_index = index;
			{
				auto p = a_p->v_indices + a_i;
				f_copy_backward(a_p->v_indices, p, p + 1, delta);
				a_p->v_indices[0] = delta;
				delta -= a_delta;
				auto q = a_p->v_indices + a_p->v_size;
				while (++p != q) *p += delta;
			}
		}
		void f_erase(size_t a_i, const t_index& a_delta, const t_index& a_base, t_index& a_index, t_branch* a_p)
		{
			auto p = v_nodes;
			auto q = a_p->v_nodes;
			*f_unshift(p + a_i + 1, p + A_size / 2, 1) = *q;
			f_unshift(q, q + a_p->v_size, 1);
			auto index = a_index - a_base;
			auto delta = a_p->v_indices[0];
			a_index += delta;
			{
				auto p = v_indices + a_i;
				auto q = f_copy_forward(p + 1, v_indices + A_size / 2, p, -a_delta);
				*q = index - a_delta;
				p = a_p->v_indices;
				f_copy_forward(p + 1, p + a_p->v_size, p, -delta);
			}
			--a_p->v_size;
		}
		void f_merge(const t_index& a_base, const t_index& a_index, t_branch* a_p, size_t a_i, const t_index& a_delta)
		{
			auto p = a_p->v_nodes;
			auto i = std::copy(p, p + a_i + 1, v_nodes + A_size / 2 + 1);
			std::copy(p + a_i + 2, p + A_size / 2 + 1, i);
			this->v_size = A_size / 2 * 2;
			auto delta = a_index - a_base;
			{
				auto p = v_indices + A_size / 2;
				*p = delta;
				auto q = a_p->v_indices + a_i;
				p = f_copy_forward(a_p->v_indices, q, ++p, delta);
				f_copy_forward(++q, a_p->v_indices + A_size / 2, p, delta - a_delta);
			}
		}
		void f_merge(size_t a_i, const t_index& a_delta, const t_index& a_base, const t_index& a_index, t_branch* a_p)
		{
			f_unshift(v_nodes + a_i + 1, v_nodes + A_size / 2, 1);
			auto q = a_p->v_nodes;
			std::copy(q, q + A_size / 2 + 1, v_nodes + A_size / 2);
			this->v_size = A_size / 2 * 2;
			auto delta = a_index - a_base - a_delta;
			{
				auto p = v_indices + a_i;
				auto q = v_indices + A_size / 2;
				p = f_copy_forward(p + 1, q, p, -a_delta);
				*p = delta;
				f_copy_forward(a_p->v_indices, a_p->v_indices + A_size / 2, ++p, delta);
			}
		}
	};
	struct t_via
	{
		void* v_node;
		size_t v_index;
	};
	struct t_at
	{
		void* v_node;
		t_index v_index;

		template<typename T_less>
		t_via f_step(T_less&& a_less)
		{
			auto p = static_cast<t_branch*>(v_node);
			size_t i = std::upper_bound(p->v_indices, p->v_indices + p->v_size, v_index, std::forward<T_less>(a_less)) - p->v_indices;
			if (i > 0) v_index -= p->v_indices[i - 1];
			v_node = p->v_nodes[i];
			return {p, i};
		}
	};

	static void f_adjust(const t_via* a_head, const t_via* a_tail, const t_delta& a_delta)
	{
		while (a_head != a_tail) {
			auto p = static_cast<t_branch*>((--a_tail)->v_node);
			for (size_t j = a_tail->v_index; j < p->v_size; ++j) p->v_indices[j] += a_delta;
		}
	}

	void* v_root = nullptr;
	size_t v_depth = 0;
	t_index v_size{};

	size_t f_depth(size_t a_n) const
	{
		size_t depth = v_depth + 1;
		for (; a_n > 0; a_n /= (A_size / 2) + 1) ++depth;
		return depth;
	}
	t_via* f_insert_branch(t_via* a_head, t_via* a_tail, t_index a_index, const t_index& a_delta, void* a_node, bool a_put_right, bool a_get_right);
	t_via* f_erase_branch(t_via* a_head, t_via* a_tail, const t_index& a_delta);
	template<typename T_less>
	t_at f_at(const t_index& a_index, t_via* a_path, T_less&& a_less) const
	{
		t_at i{v_root, a_index};
		for (size_t n = v_depth; --n > 0;) *a_path++ = i.f_step(std::forward<T_less>(a_less));
		return i;
	}
	void f_dump(size_t a_level, void* a_node, const std::function<void(size_t, const t_index&)>& a_dump) const
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
	const t_index& f_size() const
	{
		return v_size;
	}
	bool f_empty() const
	{
		return v_root == nullptr;
	}
	void f_dump(const std::function<void(size_t, const t_index&)>& a_dump) const
	{
		f_dump(0, v_root, a_dump);
	}
};

template<typename t_index, size_t A_size>
typename t_tree<t_index, A_size>::t_via* t_tree<t_index, A_size>::f_insert_branch(t_via* a_head, t_via* a_tail, t_index a_index, const t_index& a_delta, void* a_node, bool a_put_right, bool a_get_right)
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
		auto index = p->v_indices[(A_size + 1) / 2 - 1];
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
		auto index = p->v_indices[(A_size + 1) / 2];
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
		*a_tail = a_get_right ? t_via{q, 0} : t_via{p, (A_size + 1) / 2};
	}
	return ++a_tail;
}

template<typename t_index, size_t A_size>
typename t_tree<t_index, A_size>::t_via* t_tree<t_index, A_size>::f_erase_branch(t_via* a_head, t_via* a_tail, const t_index& a_delta)
{
	auto p = static_cast<t_branch*>((--a_tail)->v_node);
	size_t i = a_tail->v_index;
	if (a_head == a_tail) {
		if (p->v_size > 1) {
			p->f_erase(i, a_delta);
			*a_tail++ = {p, i};
		} else {
			v_root = p->v_nodes[0];
			delete p;
			--v_depth;
		}
		return a_tail;
	} else if (p->v_size > A_size / 2) {
		p->f_erase(i, a_delta);
		f_adjust(a_head, a_tail, -a_delta);
		*a_tail = {p, i};
		return ++a_tail;
	}
	auto q = static_cast<t_branch*>(a_tail[-1].v_node);
	size_t j = a_tail[-1].v_index;
	if (j > 0) {
		auto r = static_cast<t_branch*>(q->v_nodes[j - 1]);
		if (r->v_size > A_size / 2) {
			auto base = j > 1 ? q->v_indices[j - 2] : t_index{};
			r->f_erase(base, q->v_indices[j - 1], p, i, a_delta);
			f_adjust(a_head, a_tail, -a_delta);
			*a_tail = {p, i + 1};
			return ++a_tail;
		} else if (j >= q->v_size) {
			a_tail[-1].v_index = --j;
			r->f_merge(j > 0 ? q->v_indices[j - 1] : t_index{}, q->v_indices[j], p, i, a_delta);
			delete p;
			a_tail = f_erase_branch(a_head, a_tail, a_delta);
			*a_tail = {r, i + A_size / 2 + 1};
			return ++a_tail;
		}
	}
	auto r = static_cast<t_branch*>(q->v_nodes[j + 1]);
	auto base = j > 0 ? q->v_indices[j - 1] : t_index{};
	if (r->v_size > A_size / 2) {
		p->f_erase(i, a_delta, base, q->v_indices[j], r);
		f_adjust(a_head, a_tail, -a_delta);
	} else {
		p->f_merge(i, a_delta, base, q->v_indices[j], r);
		delete r;
		a_tail = f_erase_branch(a_head, a_tail, a_delta);
	}
	*a_tail = {p, i};
	return ++a_tail;
}

template<typename T>
struct t_traits
{
};

}

#endif
