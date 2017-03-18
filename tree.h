#ifndef XTREE__TREE_H
#define XTREE__TREE_H

#include <vector>

namespace xtree
{

class t_tree
{
protected:
	struct t_node
	{
		size_t v_size;

		t_node(size_t a_size) : v_size(a_size)
		{
		}
	};
	struct t_location
	{
		t_node* v_node;
		size_t v_index;

		t_location(t_node* a_node, size_t a_index) : v_node(a_node), v_index(a_index)
		{
		}
		bool operator==(const t_location& a_x) const
		{
			return v_node == a_x.v_node && v_index == a_x.v_index;
		}
		bool operator!=(const t_location& a_x) const
		{
			return !(*this == a_x);
		}
		bool f_more() const
		{
			return v_index < v_node->v_size;
		}
	};
	struct t_path
	{
		size_t v_count = 0;
		std::vector<t_location> v_path;

		t_path() = default;
		t_path(const std::vector<t_location>& a_path) : v_path(a_path)
		{
		}
	};

	template<typename T>
	static T* f_construct(T* a_p)
	{
		return new (static_cast<void*>(a_p)) T();
	}
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

	t_node* v_root = nullptr;
	size_t v_size = 0;
	size_t v_depth = 0;

public:
	class t_trivial
	{
		friend class t_tree;

	protected:
		const t_tree* v_tree = nullptr;
		t_path* v_p;

		t_trivial() : v_p(new t_path())
		{
			++v_p->v_count;
		}
		t_trivial(const t_trivial& a_x) : v_tree(a_x.v_tree), v_p(a_x.v_p)
		{
			++v_p->v_count;
		}
		t_trivial(const t_tree* a_tree, t_path* a_p) : v_tree(a_tree), v_p(a_p)
		{
			++v_p->v_count;
		}
		~t_trivial()
		{
			if (--v_p->v_count <= 0) delete v_p;
		}
		void f_assign(const t_trivial& a_x)
		{
			v_tree = a_x.v_tree;
			++a_x.v_p->v_count;
			if (--v_p->v_count <= 0) delete v_p;
			v_p = a_x.v_p;
		}
		void f_own()
		{
			if (v_p->v_count <= 1) return;
			--v_p->v_count;
			v_p = new t_path(v_p->v_path);
			++v_p->v_count;
		}

	public:
		bool operator==(const t_trivial& a_x) const
		{
			return v_p->v_path.empty() && a_x.v_p->v_path.empty() || v_p->v_path.back() == a_x.v_p->v_path.back();
		}
		bool operator!=(const t_trivial& a_x) const
		{
			return !(*this == a_x);
		}
	};

private:
	t_trivial v_begin;
	t_trivial v_end;

protected:
	t_path* f_begin_path() const
	{
		return v_begin.v_p;
	}
	void f_push_root()
	{
		auto& path = v_begin.v_p->v_path;
		path.insert(path.begin(), t_location(v_root, 0));
		++v_depth;
	}
	void f_pop_root()
	{
		auto& path = v_begin.v_p->v_path;
		path.erase(path.begin());
		--v_depth;
	}
	t_path* f_end_path() const
	{
		return v_end.v_p;
	}

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
