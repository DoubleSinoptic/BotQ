#include "CacheAlloc.h"
#include "Common/DynamicArray.h"
#include "SyncObject.h"
#include "LockGuard.h"

namespace CacheAllocator
{
	struct CacheAllocInstance
	{
		SyncObject						  m_sync;
		DynamicArray<DynamicArray<char*>> m_frames;
		~CacheAllocInstance()
		{
			int s = 0;
			int b = 0;
			for (size_t i = 0; i < m_frames.Length(); i++)
				for (auto& x : m_frames[i])
				{
					b++;
					s += (i + 1);
					delete[] x;
				}
			printf("cached data cleaned frames: %d, totals bytes: %d\n", b, s);

		}
	} cacheInstance;


	void* Alloc(size_t length)
	{
		size_t cacheIndex = length - 1;
		LockGuard<SyncObject> _(cacheInstance.m_sync);
		if (cacheInstance.m_frames.Length() <= cacheIndex)
			return new char[length];
		DynamicArray<char*>& cache = cacheInstance.m_frames[cacheIndex];
		if (cache.Length() != 0)
		{
			char* data = cache.Back();
			cache.PopBack();
			return reinterpret_cast<void*>(data);
		}
		return new char[length];
	}

	void* Free(void* data, size_t length)
	{
		size_t cacheIndex = length - 1;
		LockGuard<SyncObject> _(cacheInstance.m_sync);
		if (cacheInstance.m_frames.Length() <= cacheIndex)
			cacheInstance.m_frames.AddEmpty((cacheIndex + 1) - cacheInstance.m_frames.Length());
		cacheInstance.m_frames[cacheIndex].Add(reinterpret_cast<char*>(data));
		return reinterpret_cast<void*>(data);
	}
}

