#pragma once

#include "Defines.h"
#include "Assert.h"

template<typename T>
class SGE_EXPORT Ref
{
public:
	template<typename ObjT>
	static Ref<ObjT> MakePlacement(T* sl)
	{
		Ref<ObjT> o;
		o._ptr = sl;
		return o;
	}
	//------------------------------------------------------
	FORCEINLINE Ref()
		: _count(nullptr),
		_ptr(nullptr)
	{}
	//------------------------------------------------------
	FORCEINLINE Ref(std::nullptr_t nullvalue)
	{
		Assert(nullvalue == nullptr);
		_count = nullptr;
		_ptr = nullptr;
	}
	//------------------------------------------------------
	FORCEINLINE Ref(T* newObject)
	{
		if (!newObject)
		{
			_count = nullptr;
			_ptr = nullptr;
			return;
		}
		_count = new size_t(1);
		_ptr = newObject;
	}
	//------------------------------------------------------
	template<typename Tb>
	FORCEINLINE Ref(const Ref<Tb>& ref)
		: _ptr(ref.GetPtr()),
		_count(ref.GetCounterPtr())
	{
		if (_count)
			(*_count)++;
	}
	//------------------------------------------------------
	FORCEINLINE Ref(const Ref& ref)
		: _ptr(ref._ptr),
		_count(ref._count)
	{
		if (_count)
			(*_count)++;
	}
	//------------------------------------------------------
	FORCEINLINE void Realase()
	{
		if (!_count)
			return;
		(*_count)--;
		if ((*_count) == 0)
		{
			delete _ptr;
			delete _count;
			_count = nullptr;
		}
	}
	//------------------------------------------------------
    FORCEINLINE Ref operator=(std::nullptr_t val)
	{
		Assert(val == nullptr);
		Realase();
		_count = (nullptr);
		_ptr = (nullptr);
		return *this;
	}
	//------------------------------------------------------
	template<typename Tb>
	FORCEINLINE Ref& operator =(const Ref<Tb>& ref)
	{
		Realase();
		_ptr = ref.GetPtr();
		_count = ref.GetCounterPtr();
		if (_count)
			(*_count)++;
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE Ref& operator =(const Ref& ref)
	{
		Realase();
		_ptr = ref._ptr;
		_count = ref._count;
		if (_count)
			(*_count)++;
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE Ref& operator =(T* newObject)
	{
		Realase();
		if (!newObject)
		{
			_count = nullptr;
			_ptr = nullptr;
			return *this;
		}
		_count = new size_t(1);
		_ptr = newObject;
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE ~Ref()
	{
		Realase();
	}
	//------------------------------------------------------
	size_t* GetCounterPtr() const
	{
		return _count;
	}
	//------------------------------------------------------
	FORCEINLINE T* operator ->() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T& operator *()
	{
		return *_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE const T& operator *() const
	{
		return *_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE operator bool() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE bool IsNull() const
	{
		return !_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T* GetPtr() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T* GetPtr()
	{
		return _ptr;
	}
private:
	T* _ptr;
	size_t* _count;
};

template<typename T, typename... Args>
Ref<T> New(const Args&... args) 
{
	/*Не очень правильное new в будушем будет улучшенна.*/
	return Ref<T>(new T(args...));
}


template<typename T> 
FORCEINLINE bool operator ==(const Ref<T>& a, const Ref<T>& b)
{
	return a.GetPtr() == b.GetPtr();
}

template<typename T>
FORCEINLINE bool operator !=(const Ref<T>& a, const Ref<T>& b)
{
	return a.GetPtr() != b.GetPtr();
}

template<typename T>
class SGE_EXPORT Ref<T[]>
{
public:
	static Ref MakePlacement(T* sl)
	{
		Ref<T[]> o;
		o._ptr = sl;
		return o;
	}
	//------------------------------------------------------
	FORCEINLINE Ref()
		: _count(nullptr),
		_ptr(nullptr)
	{}
	//------------------------------------------------------
	FORCEINLINE Ref(std::nullptr_t nullvalue)
	{
		Assert(nullvalue == nullptr);
		_count = nullptr;
		_ptr = nullptr;
	}
	//------------------------------------------------------
	FORCEINLINE Ref(T* newObject)
	{
		if (!newObject)
		{
			_count = nullptr;
			_ptr = nullptr;
			return;
		}
		_count = new size_t(1);
		_ptr = newObject;
	}
	//------------------------------------------------------
	FORCEINLINE Ref(const Ref& ref)
		: _ptr(ref._ptr),
		_count(ref._count)
	{
		if (_count)
			(*_count)++;
	}
	//------------------------------------------------------
	FORCEINLINE Ref operator=(std::nullptr_t val)
	{
		Assert(val == nullptr);
		Realase();
		_count = (nullptr);
		_ptr = (nullptr);
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE void Realase()
	{
		if (!_count)
			return;
		(*_count)--;
		if ((*_count) == 0) 
		{
			delete[] _ptr;
			delete _count;
			_count = nullptr;
		}		
	}
	//------------------------------------------------------
	FORCEINLINE ~Ref()
	{
		Realase();
	}
	//------------------------------------------------------
	FORCEINLINE Ref& operator =(const Ref& ref)
	{
		Realase();
		_ptr = ref._ptr;
		_count = ref._count;
		if(_count)
			(*_count)++;
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE Ref& operator =(T* newObject)
	{
		Realase();
		if (!newObject)
		{
			_count = nullptr;
			_ptr = nullptr;
			return *this;
		}
		_count = new size_t(1);
		_ptr = newObject;
		return *this;
	}
	//------------------------------------------------------
	FORCEINLINE const T* operator ->() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T* operator ->() 
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T& operator *()
	{
		return *_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE const T& operator *() const
	{
		return *_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE operator bool() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE bool IsNull() const
	{
		return !_ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T& operator [](size_t index) 
	{
		return _ptr[index];
	}
	//------------------------------------------------------
	FORCEINLINE const T& operator [](size_t index) const
	{
		return _ptr[index];
	}
	//------------------------------------------------------
	FORCEINLINE const T* GetPtr() const
	{
		return _ptr;
	}
	//------------------------------------------------------
	FORCEINLINE T* GetPtr() 
	{
		return _ptr;
	}
private:
	T* _ptr;
	size_t* _count;
};

template<typename T>
FORCEINLINE bool operator ==(const Ref<T[]>& a, const Ref<T[]>& b)
{
	return a.GetPtr() == b.GetPtr();
}

template<typename T>
FORCEINLINE bool operator !=(const Ref<T[]>& a, const Ref<T[]>& b)
{
	return a.GetPtr() != b.GetPtr();
}
