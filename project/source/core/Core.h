#pragma once

//-----------------------------------------------------------------------------
// Typedefs
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int uint;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef const char* cstring;

//-----------------------------------------------------------------------------
// Constants
const float PI = 3.14159265358979323846f;
const float SQRT_2 = 1.41421356237f;
const float G = 9.8105f;
const float MAX_ANGLE = PI - FLT_EPSILON;

//-----------------------------------------------------------------------------
// Bit operation macros
#define BIT(bit) (1<<(bit))
#define IS_SET(flag,bit) (((flag) & (bit)) != 0)
#define IS_CLEAR(flag,bit) (((flag) & (bit)) == 0)
#define IS_ALL_SET(flag,bits) (((flag) & (bits)) == (bits))
#define SET_BIT(flag,bit) ((flag) |= (bit))
#define CLEAR_BIT(flag,bit) ((flag) &= ~(bit))
#define SET_BIT_VALUE(flag,bit,wartos) { if(wartos) SET_BIT(flag,bit); else CLEAR_BIT(flag,bit); }
#define COPY_BIT(flag,flag2,bit) { if(((flag2) & (bit)) != 0) SET_BIT(flag,bit); else CLEAR_BIT(flag,bit); }

// Countof for array size
template <typename T, size_t N>
char(&_ArraySizeHelper(T(&array)[N]))[N];
#define countof(array) (sizeof(_ArraySizeHelper(array)))

// Helper macros
#ifndef STRING
#	define _STRING(str) #str
#	define STRING(str) _STRING(str)
#endif
#define _JOIN(a,b) a##b
#define JOIN(a,b) _JOIN(a,b)
#define __LOC2__ __FILE__ "("STRING(__LINE__)") : "
#ifndef _DEBUG
#	define FIXME __pragma(message(__LOC2__ "error: FIXME in release build!"))
#	define assert_once(expression)
#else
#	define FIXME
#	define assert_once(expression) { static bool _once = false; if(!_once && !(expression)) { _once = true; \
            _wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)); }}
#endif

//-----------------------------------------------------------------------------
// Delegates
template<typename T>
using delegate = ssvu::FastFunc<T>;

template<typename T>
inline bool IsNull(const T ptr)
{
	static_assert(std::is_pointer<T>::value, "T must be pointer");
	return !ptr;
}

//-----------------------------------------------------------------------------
// Offset cast
template<typename T>
inline T& offset_cast(void* data, uint offset)
{
	byte* b = ((byte*)data) + offset;
	return *(T*)b;
}

//-----------------------------------------------------------------------------
// Cast using union
template<typename To, typename From>
inline To union_cast(const From& f)
{
	union
	{
		To to;
		From from;
	} a;

	a.from = f;
	return a.to;
}

//-----------------------------------------------------------------------------
// Return true if any element matches condition
template<typename T, typename Arg>
inline bool Any(const T& item, const Arg& arg)
{
	return item == arg;
}
template<typename T, typename Arg, typename... Args>
inline bool Any(const T& item, const Arg& arg, const Args&... args)
{
	return item == arg || Any(item, args...);
}

//-----------------------------------------------------------------------------
// Allocators
template<typename T>
struct IAllocator
{
	virtual T* Create() = 0;
	virtual void Destroy(T* item) = 0;
};

template<typename T>
struct IVectorAllocator
{
	virtual void Destroy(vector<T*>& items) = 0;
};

namespace internal
{
	template<typename T>
	class AllocateHelper
	{
	public:
		template<typename Q = T>
		static typename std::enable_if<std::is_abstract<Q>::value, Q>::type* Allocate()
		{
			return nullptr;
		}

		template<typename Q = T>
		static typename std::enable_if<!std::is_abstract<Q>::value, Q>::type* Allocate()
		{
			return new T;
		}
	};

	template<typename T>
	struct StandardAllocator : IAllocator<T>
	{
		T* Create()
		{
			return AllocateHelper<T>::Allocate();
		}

		void Destroy(T* item)
		{
			delete item;
		}
	};
}

//-----------------------------------------------------------------------------
// RAII for simple pointer
template<typename T, typename Allocator = internal::StandardAllocator<T>>
class Ptr
{
	static_assert(std::is_base_of<IAllocator<T>, Allocator>::value, "Allocator must inherit from IAllocator.");
public:
	Ptr(nullptr_t) : ptr(nullptr)
	{
	}
	Ptr(T* ptr) : ptr(ptr)
	{
	}
	template<bool = !std::is_abstract<T>::value>
	Ptr()
	{
		ptr = allocator.Create();
	}
	~Ptr()
	{
		if(ptr)
			allocator.Destroy(ptr);
	}
	void operator = (T* new_ptr)
	{
		if(ptr)
			allocator.Destroy(ptr);
		ptr = new_ptr;
	}
	operator T* ()
	{
		return ptr;
	}
	T* operator -> ()
	{
		return ptr;
	}
	void Ensure()
	{
		if(!ptr)
			ptr = allocator.Create();
	}
	T* Pin()
	{
		T* t = ptr;
		ptr = nullptr;
		return t;
	}
	T*& Get()
	{
		return ptr;
	}

private:
	T* ptr;
	Allocator allocator;
};

//-----------------------------------------------------------------------------
// RAII for vector of pointers
template<typename T, typename Allocator>
class VectorPtr
{
public:
	static_assert(std::is_base_of<IVectorAllocator<T>, Allocator>::value, "Allocator must inherit from IVectorAllocator.");

	VectorPtr() : pinned(false)
	{

	}

	~VectorPtr()
	{
		if(!pinned)
			allocator.Destroy(items);
	}

	vector<T*>* operator -> ()
	{
		return &items;
	}

	vector<T*>&& Pin()
	{
		pinned = true;
		return std::move(items);
	}

private:
	vector<T*> items;
	Allocator allocator;
	bool pinned;
};

//-----------------------------------------------------------------------------
// Singleton template
template<typename T>
class Singleton
{
	static T instance;
public:
	static T& Get() { return instance; }
};

//-----------------------------------------------------------------------------
// Pseudohandle for system objects
struct _Handle {};
typedef _Handle* Handle;

//-----------------------------------------------------------------------------
// More includes
#include "Containers.h"
#include "CoreMath.h"
#include "File.h"
#include "Logger.h"
#include "Text.h"
#include "Tokenizer.h"
