#pragma once
/*
Developed in 2016 by Alexandr Kharin
*/
#include "Ref.h"
#include <stdlib.h>
#include <initializer_list>

template<typename T>
class SGE_EXPORT DynamicArray
{
public:
	//------------------------------------------------------
	DynamicArray() 
		: _length(0), _capacity(0), LengthReference(_length)
	{}
	//------------------------------------------------------
	[[deprecated]]
	explicit DynamicArray(size_t capacity)
		: _length(0), _capacity(capacity), LengthReference(_length)
	{
		_data = new T[capacity];
	}
	//------------------------------------------------------
	[[deprecated]]
	explicit DynamicArray(size_t arraylength, T* array, bool placmentTag)
		: _length(0), _capacity(arraylength), LengthReference(_length)
	{
		_data = Ref<T[]>::MakePlacement(array);
		_capacity = arraylength;
	}
	//------------------------------------------------------
	explicit DynamicArray(size_t length, const T& item)
		: _length(length), _capacity(length), LengthReference(_length)
	{
		_data = new T[_capacity];
		for (size_t i = 0; i < length; i++)
			_data[i] = item;
	}
	//------------------------------------------------------
	void Initialize(size_t count)
	{
		EnsureCapacity(count);
		for (size_t i = 0; i < count; i++)
			Add(T());
	}
	//------------------------------------------------------
	explicit DynamicArray(const T* beg, const T* end)
		: _length(end - beg), _capacity(end - beg), LengthReference(_length)
	{
		_data = new T[_capacity];
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < _length; i++)
			ptr[i] = beg[i];			
	}
	//------------------------------------------------------
    DynamicArray(std::initializer_list<T> lst)
		: _length(lst.end() - lst.begin()), _capacity(lst.end() - lst.begin()), LengthReference(_length)
	{
		_data = new T[_capacity];
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < _length; i++)
			ptr[i] = lst.begin()[i];
	}
	//------------------------------------------------------
	const T& Get(size_t index) const
	{
		return _data[index];
	}
	//------------------------------------------------------
	T& Get(size_t index) 
	{
		return _data[index];
	}
	//------------------------------------------------------
	template<size_t sz>
	DynamicArray(T arr[sz])
		: _length(sz), _capacity(sz), LengthReference(_length)
	{
		_data = new T[_capacity];
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < _length; i++)
			ptr[i] = arr[i];
	}
	//------------------------------------------------------
	DynamicArray(const DynamicArray& arr) : LengthReference(_length)
	{
		_capacity = arr._capacity;
		_length = arr._length;
		_data = new T[arr._capacity];
		arr.CopyTo(_data.GetPtr());
	}
	//------------------------------------------------------
	template<size_t sz>
	DynamicArray& operator =(T (&arr)[sz])
	{
		_length = sz;
		_capacity = sz;
		_data = new T[_capacity];
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < _length; i++)
			ptr[i] = arr[i];
		return *this;
	}
	//------------------------------------------------------
	void Sort() 
	{
		T* ptr = _data.GetPtr();
		qsort(ptr, _length, sizeof(T), [](const void* a, const void* b) 
		{
			if (*(const T*)a <  *(const T*)b) return -1;
			if (*(const T*)a == *(const T*)b) return 0;
			if (*(const T*)a >  *(const T*)b) return 1;
		});
	}
	//------------------------------------------------------
	void Reverse() 
	{
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < (_length / 2); i++)
			Swap(i, _length - i - 1);
	}
	//------------------------------------------------------
	void EnsureCapacity(size_t capacity)
	{
		if (_capacity < capacity)
		{
			Ref<T[]> newArr = new T[capacity];
			CopyTo(newArr.GetPtr());
			_data = newArr;
			_capacity = capacity;
		}
	}
	//------------------------------------------------------
	void AddEmpty(size_t count)
	{
		for (int i = 0; i < count; i++)
			Add(T());
	}
	//------------------------------------------------------
	void CopyTo(T* dest) const
	{
		const T* ptr = _data.GetPtr();
		for (int i = 0; i < _length; i++) 
			dest[i] = ptr[i];
	}
	//------------------------------------------------------
	inline size_t Add(const T& elem)
	{
		if ((_length + 1) > _capacity)
			EnsureCapacity((_length + 1) * 2);
		T* ptr = _data.GetPtr();
		ptr[_length] = elem;
		_length++;
		return _length - 1;
	}
	//------------------------------------------------------
	T* GetData()
	{
		return _data.GetPtr();
	}
	//------------------------------------------------------
	const T* GetData() const
	{
		return _data.GetPtr();
	}
	//------------------------------------------------------
	void Clear()
	{
		_length = 0;
	}
	//------------------------------------------------------
	void AddRange(const T* beg, size_t len)
	{
		EnsureCapacity(_length + len);
		for (size_t i = 0; i < len; i++)
			Add(beg[i]);
	}
	//------------------------------------------------------
	void AddRange(const T* beg, const T* last)
	{
		EnsureCapacity(_length + (last - beg));
		for (; beg != last; beg++)
			Add(*beg);
	}
	//------------------------------------------------------
	void UnsafeRTIIAddRange(const T* beg, const T* last)
	{
		EnsureCapacity(_length + (last - beg));
		memcpy(_data.GetPtr() + _length, beg, (last - beg) * sizeof(T));
		_length += (last - beg);
	}
	//------------------------------------------------------
	void AddRange(const std::initializer_list<T>& range)
	{
		for (const T* i = range.begin() ; i != range.end(); i++)
			Add(*i);
	}
	//------------------------------------------------------
	void Swap(size_t indexA, size_t indexB)
	{
		T* ptr = _data.GetPtr();
		T tmp = ptr[indexA];
		ptr[indexA] = ptr[indexB];
		ptr[indexB] = tmp;
	}
	//------------------------------------------------------
	size_t RemoveVectored(const T& elem)
	{
		size_t rm = IndexOf(elem);
		RemoveAtVectored(rm);
		return rm;
	}
	//------------------------------------------------------
	void RemoveAtVectored(size_t index)
	{
		if (index >= _length)
			return;
		T* ptr = _data.GetPtr();
		size_t endIndex = _length - 1;
		for (size_t i = index; i < endIndex; i++)
			Swap(i,  i + 1);
		_length--;
	}
	//------------------------------------------------------
	size_t Remove(const T& elem)
	{
		size_t idx = IndexOf(elem);
		RemoveAt(idx);
		return idx;
	}
	//------------------------------------------------------
	void RemoveAt(size_t index)
	{
		if (index >= _length)
			return;
		T* ptr = _data.GetPtr();
		Swap(index, _length - 1);
		_length--;
	}
	//------------------------------------------------------
	void PopBack() 
	{
		RemoveAt(_length - 1);
	}
	//------------------------------------------------------
	size_t Length() const
	{
		return _length;
	}
	//------------------------------------------------------
	size_t IndexOf(const T& elem)
	{
		T* ptr = _data.GetPtr();
		for (size_t i = 0; i < _length; i++)
			if (elem == ptr[i])
				return i;
		return -1;
	}
	//------------------------------------------------------
	T& Back() 
	{
		return _data[_length - 1];
	}
	//------------------------------------------------------
    T& operator [](size_t index)
	{
		return _data[index];
	}
	//------------------------------------------------------
	const T& operator [](size_t index) const
	{
		return _data[index];
	}
	//------------------------------------------------------
	Ref<T[]> GetShared() 
	{
		return _data;
	}
	//------------------------------------------------------
	//STL adapters
	FORCEINLINE T* begin()
	{
		return _data.GetPtr();
	}
	//------------------------------------------------------
	FORCEINLINE T* end()
	{
		return begin() + _length;
	}
	//------------------------------------------------------
	FORCEINLINE const T* begin() const
	{
		return _data.GetPtr();
	}
	//------------------------------------------------------
	FORCEINLINE const T* end() const
	{
		return begin() + _length;
	}
	//------------------------------------------------------
	const size_t& LengthReference;
private:
	Ref<T[]> _data;
	size_t _length;
	size_t _capacity;
};