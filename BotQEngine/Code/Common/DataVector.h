#pragma once

#include <initializer_list>
#include <cassert>

template<typename T>
class DataVector 
{
	enum ReferenceType : int
	{
		RT_CONST,
		RT_NON_CONST
	};
	union {
		T* _begin;
		const T* _cbegin;
	};
	size_t _count;
	ReferenceType _type;
public:
	DataVector(T* begin, T* end) :
		_begin(begin),
		_count(end - begin),
		_type(RT_NON_CONST)
	{}

	DataVector(const T* begin, const T* end) :
		_cbegin(begin),
		_count(end - begin),
		_type(RT_CONST)
	{}

	DataVector(T* begin, size_t len) :
		_begin(begin),
		_count(len),
		_type(RT_NON_CONST)
	{}

	DataVector(const T* begin, size_t len) :
		_cbegin(begin),
		_count(len),
		_type(RT_CONST)
	{}

	DataVector(std::initializer_list<T> val) :
		_begin(val.begin()),
		_count(val.size()),
		_type(RT_NON_CONST)
	{}

	size_t Length() const 
	{
		return _count;
	}

	T* Data()  
	{
		assert(_type == RT_NON_CONST && "data vector invalid reference");
		return _begin;
	}

	const T* Data() const 
	{
		return _cbegin;
	}

	T* begin() const
	{
		assert(_type == RT_NON_CONST && "data vector invalid reference");
		return _begin;
	}

	T* end() const
	{
		assert(_type == RT_NON_CONST && "data vector invalid reference");
		return _begin + _count;
	}

	const T* cbegin() const
	{
		return _cbegin;
	}

	const T* cend() const
	{
		return _cbegin + _count;
	}
};