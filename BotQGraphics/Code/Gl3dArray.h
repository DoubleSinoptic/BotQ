#pragma once

#include "Gl3dCore.h"
#include <initializer_list>
#include <cstring>

enum class Gl3dArrayTarget
{
	Array = 0,
	Element = 1,
	DynamicArray = 2,
	DynamicElement = 3
};

class GL3DC_EXPORT Gl3dArrayImpl
{
	unsigned int mTarget;
	unsigned int mId;
public:
	virtual ~Gl3dArrayImpl();
	Gl3dArrayImpl(Gl3dArrayTarget target);
	unsigned int GetObject() const;
	unsigned int GetTarget() const;
protected:
	void __reserve_data(const void* data, size_t length);
	void __write_data(const void* data, int offset, size_t length);
};

template<typename T>
class GL3DC_EXPORT Gl3dArray : public Gl3dArrayImpl
{
	T*		mArr;
	size_t	mLength;
	size_t	mCapacity;
public:
	Gl3dArray(const Gl3dArray& arr) = delete;
	Gl3dArray& operator=(const Gl3dArray& arr) = delete;
	

	Gl3dArray(Gl3dArrayTarget target)
		: mArr(nullptr), 
		mLength(0), 
		mCapacity(0), 
		Gl3dArrayImpl(target)
	{}

	void DetachRamData() 
	{
		if (mArr)
		{
			delete mArr;
			mArr = nullptr;
			mLength = 0;
			mCapacity = 0;
		}
	}

	~Gl3dArray() 
	{
		if(mArr)
			delete mArr;
	}

	void __PrivateSet(const void* data, size_t len)
	{
		__reserve_data(data, len);
	}

	const T& Get(size_t index) const
	{
		return mArr[index];
	}

	void Set(size_t index, const T& val)
	{
		mArr[index] = val;
		this->__write_data((const void*)&val, index * sizeof(T), sizeof(T));
	}

	void CopyTo(T* toArr) const
	{
		for (int i = 0; i < mLength; i++)
			toArr[i] = mArr[i];
	}

	void EnsureCapacity(size_t len) 
	{
		if (!mArr)
		{
			mArr = new T[len];
			mCapacity = len;
			__reserve_data(nullptr, mCapacity * sizeof(T));
		}
		
		if (mCapacity < len)
		{
			T* newArr = new T[len];
			CopyTo(newArr);
			delete[] mArr;
			mArr = newArr;
			mCapacity = len;
			__reserve_data(newArr, mCapacity * sizeof(T));
		}
	}

	int IndexOf(const T& key) const
	{
		for (int i = 0; i < mLength; i++) 
			if (mArr[i] == key)
				return i;		
		return -1;
	}

	size_t Length() const
	{
		return mLength;
	}

	void Remove(const T& key)
	{
		RemoveAt(IndexOf(key));
	}

	void RemoveAt(size_t index)
	{
	/*	if (index < 0)
			return;*/
		mLength--;
		mArr[index] = mArr[mLength];
		__write_data(&(mArr[mLength]), index * sizeof(T), sizeof(T));
	}

	template<size_t count>
	void Add(const T beg[count])
	{
		Add(beg, beg + count);
	}

	void Add(const T* beg, size_t count)
	{
		Add(beg, beg + count);
	}

	void Add(const T* beg, const T* end)
	{
		size_t needLength = end - beg;
		if (mLength + needLength > mCapacity)
			EnsureCapacity(mLength + needLength + 1);
		memcpy(mArr + mLength, beg, needLength * sizeof(T));
		__write_data(beg, mLength * sizeof(T), needLength * sizeof(T));
		mLength += needLength;
	}

	void Add(const std::initializer_list<T>& list)
	{
		Add(list.begin(), list.end());
	}

	void Add(const T& elem)
	{
		if(mLength + 1 > mCapacity)
			EnsureCapacity((mLength + 1) * 2);
		mArr[mLength] = elem;
		__write_data(&elem, mLength * sizeof(T), sizeof(T));
		mLength++;		
	}

	void Clear() 
	{
		mLength = 0;
	}


};


