#pragma once

// #define VI_NEW_DBG

#ifdef VI_NEW_DBG
#include <cstdlib>
#include <new>
#include <map>
#include <memoryapi.h>
#include <mutex>

#pragma warning (disable: 4595) 

class gNewMapClass {
public:
	static int set;

	std::map<void*, size_t> vptr;
	std::recursive_mutex	vlock;

	gNewMapClass() {
		set = 101;
	}

	~gNewMapClass() {
		set = 0;
	}

	void* mem_alloc(size_t s) {
		if (s > 0xFFFF)
		{
			std::unique_lock<std::recursive_mutex> lock(vlock);
			void *p = VirtualAlloc(0, s, MEM_COMMIT, PAGE_READWRITE);
			vptr[p] = s;
			return p;
		}
		return malloc(s);
	}

	void mem_free(void* p) {
		std::unique_lock<std::recursive_mutex> lock(vlock);

		std::map<void*, size_t>::iterator i = vptr.find(p);
		if (i == vptr.end()) {
			::free(p);
			return;
		}
		VirtualFree(p, 0, MEM_RELEASE);
		vptr.erase(i);
	}


};

extern gNewMapClass gNewMap;


inline void* operator new (size_t size)
{
	void* ptr;
	if (gNewMapClass::set != 101)
		ptr = malloc(size);
	else
		ptr = gNewMap.mem_alloc(size);

	if (!ptr)
	{
		std::bad_alloc error;
		throw error;
	}


	return ptr;
}
inline void* operator new[](size_t size)
{
	void* ptr;
	
	if (gNewMapClass::set != 101)
		ptr = malloc(size);
	else
		ptr = gNewMap.mem_alloc(size);

	if (!ptr)
	{
		std::bad_alloc error;
		throw error;
	}
		

	return ptr;
}

inline void operator delete (void* p)
{
	if (gNewMapClass::set == 101)
		gNewMap.mem_free(p);
	else
		free(p);
}
inline void operator delete[](void* p)
{
	if (gNewMapClass::set == 101)
		gNewMap.mem_free(p);
	else
		free(p);
}
#endif