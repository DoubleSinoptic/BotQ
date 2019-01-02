#ifndef UNSAFE_FUNCTION_H
#define UNSAFE_FUNCTION_H

#include "Defines.h"
#include <stdexcept>
#include <cstdlib>
#include <cstdint>
#include <new>
/*
	Эту функциональную обёртку я написал для того чтобы отказаться
	от отстойной std::function которая при любом удобном моменте
	вызывает аллокатор. Я в жизни больше 16 байт не передавал
	в качестве захваченных из контекста переменных... дак зачем
	вызывать алакацию.
*/

template<size_t functionSizeScale>
class SGE_EXPORT GetStorageFunctionImpl
{
public:
	template<typename _UnusedType>
	class SGE_EXPORT Type;

	template<typename _RetType, typename... _ArgTypes>
	class SGE_EXPORT Type<_RetType(_ArgTypes...)>
	{
		class FunctionProxy
		{
		public:
			virtual ~FunctionProxy()
			{}

			virtual _RetType Call(const _ArgTypes&... arguments) const = 0;

			virtual bool IsEmpty() const = 0;

			virtual FunctionProxy* DupeTo(void* allocated_storage) const = 0;
		};

		class FunctionProxyEmpty : public FunctionProxy
		{
		public:
			FunctionProxyEmpty()
			{}

			virtual ~FunctionProxyEmpty()
			{}

			virtual _RetType Call(const _ArgTypes&... arguments) const
			{
				throw std::runtime_error("bad_function_call");
			}

			virtual bool IsEmpty() const
			{
				return true;
			}

			virtual FunctionProxy* DupeTo(void* allocated_storage) const
			{
				return new(allocated_storage) FunctionProxyEmpty();

			}
		};

		template<typename _CallableType>
		class FunctionProxyOf : public FunctionProxy
		{
			_CallableType _function;
		public:

			typedef _CallableType function_type;

			FunctionProxyOf(const _CallableType& function) : _function(function)
			{}

			virtual ~FunctionProxyOf()
			{}

			virtual _RetType Call(const _ArgTypes&... arguments) const
			{
				return _function(arguments...);
			}

			virtual bool IsEmpty() const
			{
				return false;
			}

			virtual FunctionProxy* DupeTo(void* allocated_storage) const
			{
				return new(allocated_storage) FunctionProxyOf<_CallableType>(_function);
			}
		};


		uint8_t storage[(sizeof(size_t) * functionSizeScale)];
	public:
		FunctionProxy* proxy;


		// CTORS/DTORS
		template<typename _callableType>
		Type(const _callableType& function)
		{
			static_assert(sizeof(FunctionProxyOf<_callableType>) <= sizeof(storage), "function proxy storage buffer overflow");
			proxy = (new((FunctionProxyOf<_callableType>*)storage)  FunctionProxyOf<_callableType>(function));

		}

		Type(const Type& value)
			: proxy(value.proxy->DupeTo(storage))
		{}

		Type()
			: proxy(new((FunctionProxyEmpty*)storage) FunctionProxyEmpty())
		{}

		~Type()
		{
			proxy->~FunctionProxy();
		}

		//  OPERATORS
		Type& operator=(const Type& value)
		{
			proxy->~FunctionProxy();
			proxy = value.proxy->DupeTo(storage);
			return *this;
		}

		template<typename _callableType>
		Type& operator=(const _callableType& function)
		{
			proxy->~FunctionProxy();
			if (sizeof(FunctionProxyOf<_callableType>) > sizeof(storage))
				throw std::overflow_error("function proxy storage buffer overflow");
			proxy = (new((FunctionProxyOf<_callableType>*)storage)  FunctionProxyOf<_callableType>(function));
			return *this;
		}

		operator bool() const
		{
			return !proxy->IsEmpty();
		}

		_RetType operator()(const _ArgTypes&... arguments) const
		{
			return proxy->Call(arguments...);
		}
	};
};

template<typename _T>
using  StorageFunction = GetStorageFunctionImpl<24>::Type<_T>;


#endif // UNSAFE_FUNCTION_H
