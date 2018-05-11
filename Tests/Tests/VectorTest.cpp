#include <vector>
#include "stdio.h"
#include <memory>

/*
template<typename T>
class Allocator {
public:
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public:
	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef Allocator<U> other;
	};

public:
	inline explicit Allocator() {}
	inline ~Allocator() {}
	inline explicit Allocator(Allocator const&) {}
	template<typename U>
	inline explicit Allocator(Allocator<U> const&) {}

	//    address
	inline pointer address(reference r) { return &r; }
	inline const_pointer address(const_reference r) { return &r; }

	//    memory allocation
	inline pointer allocate(size_type cnt,
		typename std::allocator<void>::const_pointer = 0) {
		return reinterpret_cast<pointer>(::operator new(cnt * sizeof(T)));
	}
	inline void deallocate(pointer p, size_type) {
		::operator delete(p);
	}

	//    size
	inline size_type max_size() const {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t) { new(p) T(t); }
	inline void destroy(pointer p) { p->~T(); }

	inline bool operator==(Allocator const&) { return true; }
	inline bool operator!=(Allocator const& a) { return !operator==(a); }
};    //    
*/

template <typename T>
class TestClsAllocater : public std::allocator<T>
{
public:
	typedef size_t size_type;
	typedef T* pointer;
	typedef const T* const_pointer;

	template<typename _Tp1>
	struct rebind
	{
		typedef TestClsAllocater<_Tp1> other;
	};

	pointer allocate(size_type n, const void *hint = 0)
	{
		fprintf(stderr, "Alloc %d bytes.\n", n * sizeof(T));
		return std::allocator<T>::allocate(n, hint);
	}

	void deallocate(pointer p, size_type n)
	{
		fprintf(stderr, "Dealloc %d bytes (%p).\n", n * sizeof(T), p);
		return std::allocator<T>::deallocate(p, n);
	}

	TestClsAllocater() throw() : std::allocator<T>() { fprintf(stderr, "Hello allocator!\n"); }
	TestClsAllocater(const TestClsAllocater &a) throw() : std::allocator<T>(a) { }
	template <class U>
	TestClsAllocater(const TestClsAllocater<U> &a) throw() : std::allocator<T>(a) { }
	~TestClsAllocater() throw() { }
};

class TestCls
{
public:
	TestCls() {};
	~TestCls() {};

	void* operator new[](size_t size) throw()
	{
		printf("AllowNew");
		return malloc(size);
	}

	void* operator new[](size_t size, void* ptr) throw()
	{
		printf("AllowNew");
		return malloc(size);
	}


	void* operator new[](size_t size, const std::nothrow_t& nothrow_value) throw()
	{
		printf("AllowNew");
		return malloc(size);
	}
	
	static void* __CRTDECL operator new(size_t const size)
	{
		printf("AllowNew");
		return malloc(size);
	}
};

/*
void* __CRTDECL operator new(size_t const size)
{
	printf("AllowNew");
	return malloc(size);
}
*/

/*

void* __CRTDECL operator new(size_t const size)
{
for (;;)
{
if (void* const block = malloc(size))
{
return block;
}

if (_callnewh(size) == 0)
{
if (size == SIZE_MAX)
{
__scrt_throw_std_bad_array_new_length();
}
else
{
__scrt_throw_std_bad_alloc();
}
}

// The new handler was successful; try to allocate again...
}
}

/*

*/
void main()
{
	std::vector<TestCls, TestClsAllocater<TestCls>> a;
	printf("-------------------------\n");
	a.reserve(10);
	printf("-------------------------\n");
	a.resize(10);
	printf("-------------------------\n");

	a.emplace_back();
	printf("-------------------------\n");

	a.emplace_back();
	printf("-------------------------\n");

	a.emplace_back();
	printf("-------------------------\n");
}




