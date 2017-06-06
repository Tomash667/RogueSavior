#pragma once

//-----------------------------------------------------------------------------
// Global functions
//-----------------------------------------------------------------------------
// Return true if item is in container
template<typename T>
bool Contains(const vector<T>& v, const T& item)
{
	for(auto& e : v)
	{
		if(e == item)
			return true;
	}
	return false;
}

// Find item in container using predicate
template<typename T, typename Pred>
T* Find(vector<T>& v, Pred p)
{
	for(auto& e : v)
	{
		if(p(e))
			return &e;
	}
	return nullptr;
}
template<typename T, typename Pred>
T* Find(vector<T*>& v, Pred p)
{
	for(auto& e : v)
	{
		if(p(e))
			return e;
	}
	return nullptr;
}

// Delete all elements from container
template<typename T>
inline void DeleteElements(vector<T>& v)
{
	for(vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
		delete *it;
	v.clear();
}
template<typename T>
inline void DeleteElements(vector<T>* v)
{
	assert(v);
	DeleteElements(*v);
}

// Delete all elements from container with assert for nullptr
template<typename T>
inline void DeleteElementsChecked(vector<T>& v)
{
	for(T& item : v)
	{
		assert(item);
		delete item;
	}
	v.clear();
}

// Try to remove and delete item from container, don't preserve order
template<typename T>
inline bool DeleteElementTry(vector<T>& v, T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			delete *it;
			std::iter_swap(it, end - 1);
			v.pop_back();
			return true;
		}
	}

	return false;
}

// Delete element and remove it from container, don't preserve order
template<typename T>
inline void DeleteElement(vector<T>& v, T& e)
{
	if(!DeleteElementTry(v, e))
		assert(0 && "Item to delete not found");
}
template<typename T>
void DeleteElement(vector<T>* v, T& e)
{
	assert(v);
	DeleteElement(*v, e);
}

// Try to remove element from container, doesn't preserve order
template<typename T>
inline bool RemoveElementTry(vector<T>& v, const T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			std::iter_swap(it, end - 1);
			v.pop_back();
			return true;
		}
	}

	return false;
}
template<typename T>
inline bool RemoveElementTry(vector<T>* v, const T& e)
{
	assert(v);
	return RemoveElementTry(*v, e);
}

// Remove element from container, don't preserve order
template<typename T>
inline void RemoveElement(vector<T>& v, const T& e)
{
	if(!RemoveElementTry(v, e))
		assert(0 && "Item to remove not found");
}
template<typename T>
inline void RemoveElement(vector<T>* v, const T& e)
{
	assert(v);
	RemoveElement(*v, e);
}

// Remove element from container
template<typename T>
inline void RemoveElementOrder(vector<T>& v, const T& e)
{
	for(typename vector<T>::iterator it = v.begin(), end = v.end(); it != end; ++it)
	{
		if(e == *it)
		{
			v.erase(it);
			return;
		}
	}

	assert(0 && "Item to remove not found");
}
template<typename T>
inline void RemoveElementOrder(vector<T>* v, const T& e)
{
	assert(v);
	RemoveElementOrder(*v, e);
}

// Remove element from container by index
template<typename T>
inline void RemoveElementIndex(vector<T>& v, int index)
{
	std::iter_swap(v.begin() + index, v.end() - 1);
	v.pop_back();
}

// Remove element from container by index, preserve order
template<typename T>
inline void RemoveElementIndexOrder(vector<T>& v, int index)
{
	v.erase(v.begin() + index);
}

// Remove null elements from container
template<typename T>
inline void RemoveNullElements(vector<T>& v)
{
	auto it = std::remove_if(v.begin(), v.end(), IsNull<T>);
	auto end = v.end();
	if(it != end)
		v.erase(it, end);
}
template<typename T>
inline void RemoveNullElements(vector<T>* v)
{
	assert(v);
	RemoveNullElements(*v);
}

// Remove elements from container matching predicate
template<typename T, typename T2>
inline void RemoveElements(vector<T>& v, T2 pred)
{
	auto it = std::remove_if(v.begin(), v.end(), pred);
	auto end = v.end();
	if(it != end)
		v.erase(it, end);
}
template<typename T, typename T2>
inline void RemoveElements(vector<T>* v, T2 pred)
{
	assert(v);
	RemoveElements(*v, pred);
}

// Add new item to end of container
template<typename T>
inline T& Add1(vector<T>& v)
{
	v.resize(v.size() + 1);
	return v.back();
}
template<typename T>
inline T& Add1(vector<T>* v)
{
	assert(v);
	v->resize(v->size() + 1);
	return v->back();
}
template<typename T>
inline T& Add1(list<T>& v)
{
	v.resize(v.size() + 1);
	return v.back();
}

// Return random item from container
template<typename T>
inline T& RandomItem(vector<T>& v)
{
	return v[rand2() % v.size()];
}
template<class T>
inline T RandomItem(std::initializer_list<T> cont)
{
	int index = rand2() % cont.size();
	auto it = cont.begin();
	std::advance(it, index);
	return *it;
}

// Return random item and remove it from container
template<typename T>
inline T RandomItemPop(vector<T>& v)
{
	uint index = rand2() % v.size();
	T item = v[index];
	v.erase(v.begin() + index);
	return item;
}

// Get index of item inside container or -1
template<typename T>
inline int GetIndex(const vector<T>& items, const T& item)
{
	int index = 0;
	for(const T& it : items)
	{
		if(it == item)
			return index;
		++index;
	}
	return -1;
}

// Get index of ite matching predicate inside container or -1
template<typename T, typename Pred>
inline int GetIndex(const vector<T>& items, Pred pred)
{
	int index = 0;
	for(const T& it : items)
	{
		if(pred(it))
			return index;
		++index;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Global pooled container
//-----------------------------------------------------------------------------
//#define CHECK_POOL_LEAK
#if defined(CHECK_POOL_LEAK) && !defined(_DEBUG)
#	pragma message("Warning: Disabling CHECK_POOL_LEAK in release mode.")
#	undef CHECK_POOL_LEAK
#endif
template<typename T>
struct ObjectPool
{
	~ObjectPool()
	{
		Cleanup();
	}

	T* Get()
	{
		T* t;
		if(pool.empty())
			t = new T;
		else
		{
			t = pool.back();
			pool.pop_back();
		}
		__if_exists(T::OnGet)
		{
			t->OnGet();
		}
		return t;
	}

	void VerifyElement(T* t)
	{
		for(T* e : pool)
			assert(t != e);
	}

	void CheckDuplicates(vector<T*>& elems)
	{
		for(uint i = 0, count = elems.size(); i < count; ++i)
		{
			T* t = elems[i];
			if(!t)
				continue;
			for(uint j = i + 1; j < count; ++j)
				assert(t != elems[j]);
		}
	}

	void Free(T* t)
	{
		assert(t);
#ifdef _DEBUG
		VerifyElement(t);
#endif
		__if_exists(T::OnFree)
		{
			t->OnFree();
		}
#ifdef CHECK_POOL_LEAK
		delete t;
#else
		pool.push_back(t);
#endif
	}

	void Free(vector<T*>& elems)
	{
		if(elems.empty())
			return;

		__if_exists(T::OnFree)
		{
			for(T* e : elems)
			{
				assert(e);
				e->OnFree();
			}
		}

#ifdef CHECK_POOL_LEAK
		DeleteElementsChecked(elems);
#else
#ifdef _DEBUG
		CheckDuplicates(elems);
		for(T* e : elems)
		{
			assert(e);
			VerifyElement(e);
		}
#endif
		pool.insert(pool.end(), elems.begin(), elems.end());
		elems.clear();
#endif
	}

	void SafeFree(vector<T*>& elems)
	{
#ifdef _DEBUG
		CheckDuplicates(elems);
#endif
		for(T* e : elems)
		{
			if(e)
			{
#ifdef _DEBUG
				VerifyElement(e);
#endif
				__if_exists(T::OnFree)
				{
					e->OnFree();
				}
#ifdef CHECK_POOL_LEAK
				delete e;
#else
				pool.push_back(e);
#endif
			}
		}
		elems.clear();
	}

	void Cleanup()
	{
		DeleteElements(pool);
	}

private:
	vector<T*> pool;
};

// Proxy for object pool
template<typename T>
class ObjectPoolProxy
{
public:
	static T* Get() { return GetPool().Get(); }
	static void Free(T* t) { GetPool().Free(t); }
	static void Free(vector<T*>& ts) { GetPool().Free(ts); }
	static void SafeFree(vector <T*>& ts) { GetPool().SafeFree(ts); }
	static void Cleanup() { GetPool().Cleanup(); }
	virtual void Free() { Free((T*)this); }

private:
	static ObjectPool<T>& GetPool() { static ObjectPool<T> pool; return pool; }
};

// Allocators for object pool
namespace internal
{
	template<typename T>
	struct ObjectPoolAllocator : IAllocator<T>
	{
		static_assert(std::is_base_of<ObjectPoolProxy<T>, T>::value, "T must inherit from ObjectPoolProxy<T>");

		T* Create()
		{
			return T::Get();
		}

		void Destroy(T* item)
		{
			T::Free(item);
		}
	};

	template<typename T>
	struct ObjectPoolVectorAllocator : IVectorAllocator<T>
	{
		static_assert(std::is_base_of<ObjectPoolProxy<T>, T>::value, "T must inherit from ObjectPoolProxy<T>");

		void Destroy(vector<T*>& items)
		{
			T::Free(items);
		}
	};
}

template<typename T>
using ObjectPoolRef = Ptr<T, internal::ObjectPoolAllocator<T>>;

template<typename T>
using ObjectPoolVectorRef = VectorPtr<T, internal::ObjectPoolVectorAllocator<T>>;

// Global object pools for string and any pointer vector
extern ObjectPool<string> StringPool;
extern ObjectPool<vector<void*>> VectorPool;
extern ObjectPool<vector<byte>> BufPool;

//-----------------------------------------------------------------------------
// Temporary string using StringPool
//-----------------------------------------------------------------------------
struct LocalString
{
	LocalString()
	{
		s = StringPool.Get();
		s->clear();
	}

	LocalString(cstring str)
	{
		assert(str);
		s = StringPool.Get();
		*s = str;
	}

	LocalString(cstring str, cstring str_to)
	{
		s = StringPool.Get();
		uint len = str_to - str;
		s->resize(len);
		memcpy((char*)s->data(), str, len);
	}

	LocalString(const string& str)
	{
		s = StringPool.Get();
		*s = str;
	}

	~LocalString()
	{
		StringPool.Free(s);
	}

	void operator = (cstring str)
	{
		*s = str;
	}

	void operator = (const string& str)
	{
		*s = str;
	}

	char at_back(uint offset) const
	{
		assert(offset < s->size());
		return s->at(s->size() - 1 - offset);
	}

	void pop(uint count)
	{
		assert(s->size() > count);
		s->resize(s->size() - count);
	}

	void operator += (cstring str)
	{
		*s += str;
	}

	void operator += (const string& str)
	{
		*s += str;
	}

	void operator += (char c)
	{
		*s += c;
	}

	operator cstring() const
	{
		return s->c_str();
	}

	string& get_ref()
	{
		return *s;
	}

	string* get_ptr()
	{
		return s;
	}

	string* operator -> ()
	{
		return s;
	}

	const string* operator -> () const
	{
		return s;
	}

	bool operator == (cstring str) const
	{
		return *s == str;
	}

	bool operator == (const string& str) const
	{
		return *s == str;
	}

	bool operator == (const LocalString& str) const
	{
		return *s == *str.s;
	}

	bool operator != (cstring str) const
	{
		return *s != str;
	}

	bool operator != (const string& str) const
	{
		return *s != str;
	}

	bool operator != (const LocalString& str) const
	{
		return *s != *str.s;
	}

	bool empty() const
	{
		return s->empty();
	}

	cstring c_str() const
	{
		return s->c_str();
	}

	void clear()
	{
		s->clear();
	}

private:
	string* s;
};

//-----------------------------------------------------------------------------
// Temporary vector for storing pointers using VectorPool
//-----------------------------------------------------------------------------
template<typename T>
struct LocalVector
{
	typedef vector<T> Vector;
	typedef Vector* VectorPtr;
	typedef typename Vector::iterator Iter;

	static_assert(sizeof(T) == sizeof(void*), "LocalVector element must be pointer or have sizeof pointer.");

	LocalVector()
	{
		v = (VectorPtr)VectorPool.Get();
		v->clear();
	}

	explicit LocalVector(Vector& v2)
	{
		v = (VectorPtr*)VectorPool.Get();
		*v = v2;
	}

	~LocalVector()
	{
		VectorPool.Free((vector<void*>*)v);
	}

	void push_back(T e)
	{
		v->push_back(e);
	}

	bool empty() const
	{
		return v->empty();
	}

	Iter begin()
	{
		return v->begin();
	}

	Iter end()
	{
		return v->end();
	}

	uint size() const
	{
		return v->size();
	}

	T& RandomItem()
	{
		return v->at(rand2() % v->size());
	}

	T& operator [] (int n)
	{
		return v->at(n);
	}

	const T& operator [] (int n) const
	{
		return v->at(n);
	}

	vector<T>& Get()
	{
		return *v;
	}

	void clear()
	{
		v->clear();
	}

private:
	VectorPtr v;
};

template<typename T>
inline T& RandomItem(LocalVector<T>& v)
{
	return v.RandomItem();
}

//-----------------------------------------------------------------------------
// Loop over list and erase elements that returned true
template<typename T, typename Action>
inline void LoopAndRemove(vector<T>& items, Action action)
{
	items.erase(std::remove_if(items.begin(), items.end(), action), items.end());
}

//-----------------------------------------------------------------------------
// Container for item/weight pair
template<typename T>
struct WeightPair
{
	T item;
	int weight;

	WeightPair(T& item, int weight) : item(item), weight(weight) {}
};

// Return random item using weighted item pair
template<typename T>
inline T& RandomItemWeight(vector<WeightPair<T>>& items, int max_weight)
{
	int a = rand2() % max_weight, b = 0;
	for(auto& item : items)
	{
		b += item.weight;
		if(a < b)
			return item.item;
	}
	// if it gets here max_count is wrong, return random item
	return random_item(items).item;
}

// Helper to return random weighted item
template<typename T>
class WeightData
{
	vector<WeightPair<T>> data;
	int max;

public:
	void Add(T item, int weight)
	{
		assert(weight > 0);
		data.push_back({ item, weight });
		max += weight;
	}

	T Get()
	{
		return RandomItemWeight(data, max);
	}

	bool IsEmpty() const
	{
		return max == 0;
	}
};

//-----------------------------------------------------------------------------
// Like local vector but can contain any data, not just pointers
//-----------------------------------------------------------------------------
template<typename T>
class AnyLocalVector
{
public:
	struct iterator : std::iterator<std::input_iterator_tag, T>
	{
		friend class AnyLocalVector;

		T& operator * ()
		{
			return v->at(offset);
		}

		bool operator == (const iterator& it) const
		{
			assert(it.v == v);
			return offset == it.offset;
		}

		bool operator != (const iterator& it) const
		{
			assert(it.v == v);
			return offset != it.offset;
		}

		bool operator < (const iterator& it) const
		{
			assert(it.v == v);
			return offset < it.offset;
		}

		iterator& operator ++ ()
		{
			++offset;
			return *this;
		}

		iterator operator ++ (int)
		{
			iterator it(v, offset);
			++offset;
			return it;
		}

		iterator& operator -- ()
		{
			--offset;
			return *this;
		}

		iterator& operator + (uint count)
		{
			offset += count;
			return *this;
		}

		iterator& operator - (uint count)
		{
			offset -= count;
			return *this;
		}

		int operator - (const iterator& it) const
		{
			assert(it.v == v);
			return offset - it.offset;
		}

	private:
		iterator(AnyLocalVector* v, uint offset) : v(v), offset(offset) {}

		AnyLocalVector* v;
		uint offset;
	};

	AnyLocalVector()
	{
		buf = BufPool.Get();
	}

	~AnyLocalVector()
	{
		buf->clear();
		BufPool.Free(buf);
	}

	T& at(uint offset)
	{
		assert(offset < size());
		return ((T*)buf->data())[offset];
	}

	T& back()
	{
		assert(!empty());
		return ((T*)buf->data())[size() - 1];
	}

	iterator begin()
	{
		return iterator(this, 0);
	}

	bool empty() const
	{
		return buf->empty();
	}

	iterator end()
	{
		return iterator(this, size());
	}

	void push_back(T& item)
	{
		uint s = buf->size();
		buf->resize(buf->size() + sizeof(T));
		memcpy(buf->data() + s, &item, sizeof(T));
	}

	uint size() const
	{
		return buf->size() / sizeof(T);
	}

	template<typename Pred>
	void sort(Pred pred)
	{
		std::sort((T*)buf->data(), (T*)(buf->data() + buf->size()), pred);
	}

private:
	vector<byte>* buf;
};

//-----------------------------------------------------------------------------
template<typename T, typename U>
inline bool In(T val, const std::initializer_list<U>& lis)
{
	for(U t : lis)
	{
		if(t == (T)val)
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Dereference vector for forech on vector of pointer
namespace helper
{
	template<typename T>
	struct DerefEnumerator
	{
		struct Iterator
		{
			typedef typename vector<T*>::iterator It;

			Iterator(It it) : it(it)
			{

			}

			bool operator != (const Iterator& i) const
			{
				return it != i.it;
			}

			void operator ++ ()
			{
				++it;
			}

			T& operator * ()
			{
				return **it;
			}

			It it;
		};

		DerefEnumerator(vector<T*>& v) : v(v)
		{

		}

		Iterator begin()
		{
			return Iterator(v.begin());
		}

		Iterator end()
		{
			return Iterator(v.end());
		}

		vector<T*>& v;
	};
}

// Dereference foreach vector of pointers
template<typename T>
helper::DerefEnumerator<T> Deref(vector<T*>& v)
{
	return helper::DerefEnumerator<T>(v);
}
