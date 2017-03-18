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
		virtual void f_clear() = 0;
		virtual t_node* f_node(size_t a_i) = 0;
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
		t_node* operator*() const
		{
			return v_node->f_node(v_index);
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
	static void f_first(std::vector<t_location>& a_path)
	{
		for (auto p = *a_path.back(); p; p = p->f_node(0)) a_path.push_back(t_location(p, 0));
	}
	static void f_last(std::vector<t_location>& a_path)
	{
		for (auto p = *a_path.back(); p; p = p->f_node(p->v_size)) a_path.push_back(t_location(p, p->v_size));
	}
	static void f_normalize(std::vector<t_location>& a_path)
	{
		while (!a_path.back().f_more() && a_path.size() > 1) a_path.pop_back();
	}

	t_node* v_root = nullptr;
	size_t v_size = 0;

public:
	class t_trivial
	{
		friend class t_tree;

	protected:
		t_path* v_p;

		t_trivial() : v_p(new t_path())
		{
			++v_p->v_count;
		}
		t_trivial(const t_trivial& a_x) : v_p(a_x.v_p)
		{
			++v_p->v_count;
		}
		t_trivial(t_path* a_p) : v_p(a_p)
		{
			++v_p->v_count;
		}
		~t_trivial()
		{
			if (--v_p->v_count <= 0) delete v_p;
		}
		void f_assign(const t_trivial& a_x)
		{
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
	}
	void f_pop_root()
	{
		auto& path = v_begin.v_p->v_path;
		path.erase(path.begin());
	}
	t_path* f_end_path() const
	{
		return v_end.v_p;
	}

public:
	~t_tree()
	{
		f_clear();
	}
	size_t f_size() const
	{
		return v_size;
	}
	bool f_empty() const
	{
		return v_size <= 0;
	}
	void f_clear()
	{
		if (!v_root) return;
		v_root->f_clear();
		v_root = nullptr;
		v_size = 0;
		v_begin.v_p->v_path.clear();
		v_end.v_p->v_path.clear();
	}
};

}

#endif
