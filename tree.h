#ifndef XTREE__TREE_H
#define XTREE__TREE_H

namespace xtree
{

class t_tree
{
protected:
	struct t_location
	{
		void* v_node;
		size_t v_index;
	};

	template<typename T>
	static T* f_construct(T* a_p, const T& a_x)
	{
		return new (static_cast<void*>(a_p)) T(a_x);
	}
	template<typename T>
	static T* f_destruct(T* a_p)
	{
		a_p->~T();
		return a_p;
	}
	template<typename T>
	static void f_destruct(T* a_i, T* a_j)
	{
		while (a_i != a_j) {
			f_destruct(a_i);
			++a_i;
		}
	}
	template<typename T>
	static void f_move(T* a_p, T* a_q)
	{
		f_construct(a_p, *a_q);
		f_destruct(a_q);
	}
	template<typename T>
	static T* f_shift(T* a_i, T* a_j)
	{
		while (a_j != a_i) {
			T* p = a_j;
			f_move(a_j, --p);
			a_j = p;
		}
		return a_j;
	}
	template<typename T>
	static T* f_unshift(T* a_i, T* a_j)
	{
		while (a_i != a_j) {
			T* p = a_i;
			f_move(a_i, ++p);
			a_i = p;
		}
		return a_i;
	}

	void* v_root = nullptr;
	size_t v_size = 0;
	size_t v_depth = 0;

public:
	size_t f_size() const
	{
		return v_size;
	}
	bool f_empty() const
	{
		return v_size <= 0;
	}
};

}

#endif
