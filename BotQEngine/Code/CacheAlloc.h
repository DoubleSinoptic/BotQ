#pragma once
#ifndef __CACHEALLOC_H__
#define __CACHEALLOC_H__

#include "Common/DynamicArray.h"

namespace CacheAllocator
{
	void* Alloc(size_t length);
	void* Free(void* data, size_t length);
}

template<typename T, typename... Args>
T* CacheAlloc(const Args&... args) 
{
	void* d = CacheAllocator::Alloc(sizeof(T));
	return new(d) T(args...);
}

template<typename T>
void CacheDestroy(T* o) 
{
	o->~T();
	CacheAllocator::Free(reinterpret_cast<void*>(o), sizeof(T));
}

#endif