#ifndef __TYPEDOBJECT_H__
#define __TYPEDOBJECT_H__

#include <typeinfo>
#include <cstring>

#define TYPED_OBJECT
#include "StorageFunction.h"
#include "Defines.h"

/*
Замена RTII... мда правда без самого RTII 
это не работет
*/


struct SGE_EXPORT ReferencableProxy
{
	virtual ~ReferencableProxy() {}
	ReferencableProxy* last;
	const char* name;
	inline ReferencableProxy() {}

	virtual void ExportTo(ReferencableProxy* proxy) = 0;

	inline void Set(ReferencableProxy*  &_last, const char* _name)
	{
		name = _name;
		last = _last;
		_last = this;
	}
};

template<typename T>
struct SGE_EXPORT ReferenceOfPropetry : public ReferencableProxy
{
	template<typename C0, typename C1>
	inline ReferenceOfPropetry(const C0& c0,const C1& c1, ReferencableProxy*  &last, const char* name)
	{
		Set(last, name);
		get = c0;
		set = c1;
	}
	StorageFunction<T(void)> get;
	StorageFunction<void(const T&)> set;

	virtual void ExportTo(ReferencableProxy* proxy) 
	{
		ReferenceOfPropetry<T>* th = dynamic_cast<ReferenceOfPropetry<T>*>(proxy);
		if (!th)
		{
			static char error_buffer[1024];
			snprintf(error_buffer, 1024, "invalid export entry of: %s to: %s", this->name, proxy->name);
			throw std::runtime_error(error_buffer);
		}
		th->set(this->get());
	}
};

class SGE_EXPORT TypedObject
{
protected:
	ReferencableProxy* _prefproxy = nullptr;
public:
	virtual ~TypedObject();

	virtual void CloneTo(TypedObject* obj) const;

	explicit TypedObject();

	ReferencableProxy* BeginProxy();

	int GetProxyCount();

	ReferencableProxy* FindPropetry(const char* name);

	template<typename T>
	void SetPropetry(const char* name, const T& value)
	{
		static char error_buffer[1024];
		ReferencableProxy* __prox = FindPropetry(name);
		if (__prox == nullptr)
		{
			snprintf(error_buffer, 1024, "propetry not found: %s", name);
			throw std::runtime_error(error_buffer);
		}
		ReferenceOfPropetry<T>* __proxprop = dynamic_cast<ReferenceOfPropetry<T>*>(__prox);
		if (__proxprop == nullptr)
		{
			snprintf(error_buffer, 1024, "invlid propetry type: %s", name);
			throw std::runtime_error(error_buffer);
		}
		__proxprop->_set(value);
	}

	template<typename T>
	T GetPropetry(const char* name)
	{
		static char error_buffer[1024];
		ReferencableProxy* __prox = FindPropetry(name);
		if (__prox == nullptr)
		{
			snprintf(error_buffer, 1024, "propetry not found: %s", name);
			throw std::runtime_error(error_buffer);
		}
		ReferenceOfPropetry<T>* __proxprop = dynamic_cast<ReferenceOfPropetry<T>*>(__prox);
		if (__proxprop == nullptr)
		{
			snprintf(error_buffer, 1024, "invlid propetry type: %s", name);
			throw std::runtime_error(error_buffer);
		}
		return __proxprop->_get();
	}


private:	
};

#define REGISTER_PROPETRY(type, name) private: ReferenceOfPropetry< type > __propproxy ## name =  ReferenceOfPropetry< type > ([&]() { return Get ## name () ; }, [&](const type & ____value ) { Set ## name ( ____value ); }, _prefproxy , # name ); public:
#define REGISTER_VARIALBE(type, name) private: ReferenceOfPropetry< type > __propproxy ## name =  ReferenceOfPropetry< type > ([&]() { return name; }, [&](const type & ____value ) { name = ____value; }, _prefproxy , # name ); public:


#endif //__TYPEDOBJECT_H__