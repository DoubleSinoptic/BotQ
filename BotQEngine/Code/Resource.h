#pragma once

#include "Core.h"

class ResourceAdapter
{
public:
	virtual ~ResourceAdapter() {}
};

template<typename T>
class ResourceAdapterOfType : public ResourceAdapter
{
public:
	ResourceAdapterOfType(const T& o) 
		: object(o)
	{}
	T object;
};

class SGE_EXPORT Resource
{
	ResourceAdapter*	mAdapter;
	String				mPath;
public:
	virtual ~Resource();

	Resource(const String& path, ResourceAdapter* adapter);

	template<typename T>
	Resource(const T& o, const String& path)
		: Resource(path, new ResourceAdapterOfType<T>(o))
	{
		Log("Added resource: %s --> %s", typeid(T).name(), *path);
	}

	template<typename T>
	T& GetObject() 
	{
		if (dynamic_cast<ResourceAdapterOfType<T>*>(mAdapter) == nullptr)
			Error("ivalid resoruce type to cast");
		return static_cast<ResourceAdapterOfType<T>*>(mAdapter)->object;
	}

	String GetPath() const { return mPath; }

	static DynamicArray<Resource*> FindAll(const String& folder);
	static Resource* Find(const String& path, bool throwError = true);

	static void FreeAllResources();
};

